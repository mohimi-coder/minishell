# typed: strict
# frozen_string_literal: true

require "livecheck/constants"
require "livecheck/error"
require "livecheck/livecheck_version"
require "livecheck/skip_conditions"
require "livecheck/strategy"
require "addressable"
require "uri"

module Homebrew
  # The {Livecheck} module consists of methods used by the `brew livecheck`
  # command. These methods print the requested livecheck information
  # for formulae.
  module Livecheck
    module_function

    GITEA_INSTANCES = T.let(%w[
      codeberg.org
      gitea.com
      opendev.org
      tildegit.org
    ].freeze, T::Array[String])

    GOGS_INSTANCES = T.let(%w[
      lolg.it
    ].freeze, T::Array[String])

    STRATEGY_SYMBOLS_TO_SKIP_PREPROCESS_URL = T.let([
      :extract_plist,
      :github_latest,
      :header_match,
      :json,
      :page_match,
      :sparkle,
      :xml,
      :yaml,
    ].freeze, T::Array[Symbol])

    UNSTABLE_VERSION_KEYWORDS = T.let(%w[
      alpha
      beta
      bpo
      dev
      experimental
      prerelease
      preview
      rc
    ].freeze, T::Array[String])

    sig { returns(T::Hash[T::Class[T.anything], String]) }
    def livecheck_strategy_names
      return T.must(@livecheck_strategy_names) if defined?(@livecheck_strategy_names)

      # Cache demodulized strategy names, to avoid repeating this work
      @livecheck_strategy_names = T.let({}, T.nilable(T::Hash[T::Class[T.anything], String]))
      Strategy.constants.sort.each do |const_symbol|
        constant = Strategy.const_get(const_symbol)
        next unless constant.is_a?(Class)

        T.must(@livecheck_strategy_names)[constant] = Utils.demodulize(T.must(constant.name))
      end
      T.must(@livecheck_strategy_names).freeze
    end

    # Uses `formulae_and_casks_to_check` to identify taps in use other than
    # homebrew/core and homebrew/cask and loads strategies from them.
    sig { params(formulae_and_casks_to_check: T::Array[T.any(Formula, Cask::Cask)]).void }
    def load_other_tap_strategies(formulae_and_casks_to_check)
      other_taps = {}
      formulae_and_casks_to_check.each do |formula_or_cask|
        next if formula_or_cask.tap.blank?
        next if formula_or_cask.tap.core_tap?
        next if formula_or_cask.tap.core_cask_tap?
        next if other_taps[formula_or_cask.tap.name]

        other_taps[formula_or_cask.tap.name] = formula_or_cask.tap
      end
      other_taps = other_taps.sort.to_h

      other_taps.each_value do |tap|
        tap_strategy_path = "#{tap.path}/livecheck/strategy"
        Dir["#{tap_strategy_path}/*.rb"].each { require(_1) } if Dir.exist?(tap_strategy_path)
      end
    end

    # Resolve formula/cask references in `livecheck` blocks to a final formula
    # or cask.
    sig {
      params(
        formula_or_cask:       T.any(Formula, Cask::Cask),
        first_formula_or_cask: T.any(Formula, Cask::Cask),
        references:            T::Array[T.any(Formula, Cask::Cask)],
        full_name:             T::Boolean,
        debug:                 T::Boolean,
      ).returns(T.nilable(T::Array[T.untyped]))
    }
    def resolve_livecheck_reference(
      formula_or_cask,
      first_formula_or_cask = formula_or_cask,
      references = [],
      full_name: false,
      debug: false
    )
      # Check the livecheck block for a formula or cask reference
      livecheck = formula_or_cask.livecheck
      livecheck_formula = livecheck.formula
      livecheck_cask = livecheck.cask
      return [nil, references] if livecheck_formula.blank? && livecheck_cask.blank?

      # Load the referenced formula or cask
      referenced_formula_or_cask = Homebrew.with_no_api_env do
        if livecheck_formula
          Formulary.factory(livecheck_formula)
        elsif livecheck_cask
          Cask::CaskLoader.load(livecheck_cask)
        end
      end

      # Error if a `livecheck` block references a formula/cask that was already
      # referenced (or itself)
      if referenced_formula_or_cask == first_formula_or_cask ||
         referenced_formula_or_cask == formula_or_cask ||
         references.include?(referenced_formula_or_cask)
        if debug
          # Print the chain of references for debugging
          puts "Reference Chain:"
          puts package_or_resource_name(first_formula_or_cask, full_name:)

          references << referenced_formula_or_cask
          references.each do |ref_formula_or_cask|
            puts package_or_resource_name(ref_formula_or_cask, full_name:)
          end
        end

        raise "Circular formula/cask reference encountered"
      end
      references << referenced_formula_or_cask

      # Check the referenced formula/cask for a reference
      next_referenced_formula_or_cask, next_references = resolve_livecheck_reference(
        referenced_formula_or_cask,
        first_formula_or_cask,
        references,
        full_name:,
        debug:,
      )

      # Returning references along with the final referenced formula/cask
      # allows us to print the chain of references in the debug output
      [
        next_referenced_formula_or_cask || referenced_formula_or_cask,
        next_references,
      ]
    end

    # Executes the livecheck logic for each formula/cask in the
    # `formulae_and_casks_to_check` array and prints the results.
    sig {
      params(
        formulae_and_casks_to_check: T::Array[T.any(Formula, Cask::Cask)],
        full_name:                   T::Boolean,
        handle_name_conflict:        T::Boolean,
        check_resources:             T::Boolean,
        json:                        T::Boolean,
        newer_only:                  T::Boolean,
        extract_plist:               T::Boolean,
        debug:                       T::Boolean,
        quiet:                       T::Boolean,
        verbose:                     T::Boolean,
      ).void
    }
    def run_checks(
      formulae_and_casks_to_check,
      full_name: false, handle_name_conflict: false, check_resources: false, json: false, newer_only: false,
      extract_plist: false, debug: false, quiet: false, verbose: false
    )
      load_other_tap_strategies(formulae_and_casks_to_check)

      ambiguous_casks = []
      if handle_name_conflict
        ambiguous_casks = formulae_and_casks_to_check
                          .group_by { |item| package_or_resource_name(item, full_name: true) }
                          .values
                          .select { |items| items.length > 1 }
                          .flatten
                          .select { |item| item.is_a?(Cask::Cask) }
      end

      ambiguous_names = []
      unless full_name
        ambiguous_names =
          (formulae_and_casks_to_check - ambiguous_casks).group_by { |item| package_or_resource_name(item) }
                                                         .values
                                                         .select { |items| items.length > 1 }
                                                         .flatten
      end

      has_a_newer_upstream_version = T.let(false, T::Boolean)

      formulae_and_casks_total = formulae_and_casks_to_check.count
      if json && !quiet && $stderr.tty?
        Tty.with($stderr) do |stderr|
          stderr.puts Formatter.headline("Running checks", color: :blue)
        end

        require "ruby-progressbar"
        progress = ProgressBar.create(
          total:          formulae_and_casks_total,
          progress_mark:  "#",
          remainder_mark: ".",
          format:         " %t: [%B] %c/%C ",
          output:         $stderr,
        )
      end

      # Allow ExtractPlist strategy if only one formula/cask is being checked.
      extract_plist = true if formulae_and_casks_total == 1

      formulae_checked = formulae_and_casks_to_check.map.with_index do |formula_or_cask, i|
        formula = formula_or_cask if formula_or_cask.is_a?(Formula)
        cask = formula_or_cask if formula_or_cask.is_a?(Cask::Cask)

        use_full_name = full_name || ambiguous_names.include?(formula_or_cask)
        name = package_or_resource_name(formula_or_cask, full_name: use_full_name)

        referenced_formula_or_cask, livecheck_references =
          resolve_livecheck_reference(formula_or_cask, full_name: use_full_name, debug:)

        if debug && i.positive?
          puts <<~EOS

            ----------

          EOS
        elsif debug
          puts
        end

        # Check skip conditions for a referenced formula/cask
        if referenced_formula_or_cask
          skip_info = SkipConditions.referenced_skip_information(
            referenced_formula_or_cask,
            name,
            full_name:     use_full_name,
            verbose:,
            extract_plist:,
          )
        end

        skip_info ||= SkipConditions.skip_information(
          formula_or_cask,
          full_name:     use_full_name,
          verbose:,
          extract_plist:,
        )
        if skip_info.present?
          next skip_info if json && !newer_only

          SkipConditions.print_skip_information(skip_info) if !newer_only && !quiet
          next
        end

        formula&.head&.downloader&.quiet!

        # Use the `stable` version for comparison except for installed
        # head-only formulae. A formula with `stable` and `head` that's
        # installed using `--head` will still use the `stable` version for
        # comparison.
        current = if formula
          if formula.head_only?
            formula.any_installed_version.version.commit
          else
            T.must(formula.stable).version
          end
        else
          Version.new(formula_or_cask.version)
        end

        current_str = current.to_s
        current = LivecheckVersion.create(formula_or_cask, current)

        latest = if formula&.head_only?
          T.must(formula.head).downloader.fetch_last_commit
        else
          version_info = latest_version(
            formula_or_cask,
            referenced_formula_or_cask:,
            livecheck_references:,
            json:, full_name: use_full_name, verbose:, debug:
          )
          version_info[:latest] if version_info.present?
        end

        check_for_resources = check_resources && formula_or_cask.is_a?(Formula) && formula_or_cask.resources.present?
        if check_for_resources
          resource_version_info = formula_or_cask.resources.map do |resource|
            res_skip_info ||= SkipConditions.skip_information(resource, verbose:)
            if res_skip_info.present?
              res_skip_info
            else
              res_version_info = resource_version(
                resource,
                latest.to_s,
                json:,
                debug:,
                quiet:,
                verbose:,
              )
              if res_version_info.empty?
                status_hash(resource, "error", ["Unable to get versions"], verbose:)
              else
                res_version_info
              end
            end
          end.compact_blank
          Homebrew.failed = true if resource_version_info.any? { |info| info[:status] == "error" }
        end

        if latest.blank?
          no_versions_msg = "Unable to get versions"
          raise Livecheck::Error, no_versions_msg unless json
          next if quiet

          next version_info if version_info.is_a?(Hash) && version_info[:status] && version_info[:messages]

          latest_info = status_hash(formula_or_cask, "error", [no_versions_msg], full_name: use_full_name,
                                                                                 verbose:)
          if check_for_resources
            unless verbose
              resource_version_info.map! do |info|
                info.delete(:meta)
                info
              end
            end
            latest_info[:resources] = resource_version_info
          end

          next latest_info
        end

        if (m = latest.to_s.match(/(.*)-release$/)) && !current.to_s.match(/.*-release$/)
          latest = Version.new(m[1])
        end

        latest_str = latest.to_s
        latest = LivecheckVersion.create(formula_or_cask, latest)

        is_outdated = if formula&.head_only?
          # A HEAD-only formula is considered outdated if the latest upstream
          # commit hash is different than the installed version's commit hash
          (current != latest)
        else
          (current < latest)
        end

        is_newer_than_upstream = (formula&.stable? || cask) && (current > latest)

        info = {}
        info[:formula] = name if formula
        info[:cask] = name if cask
        info[:version] = {
          current:             current_str,
          latest:              latest_str,
          latest_throttled:    version_info&.dig(:latest_throttled),
          outdated:            is_outdated,
          newer_than_upstream: is_newer_than_upstream,
        }.compact
        info[:meta] = {
          livecheckable: formula_or_cask.livecheckable?,
        }
        info[:meta][:head_only] = true if formula&.head_only?
        info[:meta].merge!(version_info[:meta]) if version_info.present? && version_info.key?(:meta)

        info[:resources] = resource_version_info if check_for_resources

        next if newer_only && !info[:version][:outdated]

        has_a_newer_upstream_version ||= true

        if json
          progress&.increment
          info.delete(:meta) unless verbose
          if check_for_resources && !verbose
            resource_version_info.map! do |info|
              info.delete(:meta)
              info
            end
          end
          next info
        end
        puts if debug
        print_latest_version(info, verbose:, ambiguous_cask: ambiguous_casks.include?(formula_or_cask))
        print_resources_info(resource_version_info, verbose:) if check_for_resources
        nil
      rescue => e
        Homebrew.failed = true
        use_full_name = full_name || ambiguous_names.include?(formula_or_cask)

        if json
          progress&.increment
          unless quiet
            status_hash(formula_or_cask, "error", [e.to_s], full_name: use_full_name,
                                                            verbose:)
          end
        elsif !quiet
          name = package_or_resource_name(formula_or_cask, full_name: use_full_name)
          name += " (cask)" if ambiguous_casks.include?(formula_or_cask)

          onoe "#{Tty.blue}#{name}#{Tty.reset}: #{e}"
          $stderr.puts Utils::Backtrace.clean(e) if debug && !e.is_a?(Livecheck::Error)
          print_resources_info(resource_version_info, verbose:) if check_for_resources
          nil
        end
      end

      puts "No newer upstream versions." if newer_only && !has_a_newer_upstream_version && !debug && !json && !quiet

      return unless json

      if progress
        progress.finish
        Tty.with($stderr) do |stderr|
          stderr.print "#{Tty.up}#{Tty.erase_line}" * 2
        end
      end

      puts JSON.pretty_generate(formulae_checked.compact)
    end

    sig { params(package_or_resource: T.any(Formula, Cask::Cask, Resource), full_name: T::Boolean).returns(String) }
    def package_or_resource_name(package_or_resource, full_name: false)
      case package_or_resource
      when Formula
        formula_name(package_or_resource, full_name:)
      when Cask::Cask
        cask_name(package_or_resource, full_name:)
      when Resource
        package_or_resource.name
      else
        T.absurd(package_or_resource)
      end
    end

    # Returns the fully-qualified name of a cask if the `full_name` argument is
    # provided; returns the name otherwise.
    sig { params(cask: Cask::Cask, full_name: T::Boolean).returns(String) }
    def cask_name(cask, full_name: false)
      full_name ? cask.full_name : cask.token
    end

    # Returns the fully-qualified name of a formula if the `full_name` argument is
    # provided; returns the name otherwise.
    sig { params(formula: Formula, full_name: T::Boolean).returns(String) }
    def formula_name(formula, full_name: false)
      full_name ? formula.full_name : formula.name
    end

    sig {
      params(
        package_or_resource: T.any(Formula, Cask::Cask, Resource),
        status_str:          String,
        messages:            T.nilable(T::Array[String]),
        full_name:           T::Boolean,
        verbose:             T::Boolean,
      ).returns(T::Hash[Symbol, T.untyped])
    }
    def status_hash(package_or_resource, status_str, messages = nil, full_name: false, verbose: false)
      formula = package_or_resource if package_or_resource.is_a?(Formula)
      cask = package_or_resource if package_or_resource.is_a?(Cask::Cask)
      resource = package_or_resource if package_or_resource.is_a?(Resource)

      status_hash = {}
      if formula
        status_hash[:formula] = formula_name(formula, full_name:)
      elsif cask
        status_hash[:cask] = cask_name(cask, full_name:)
      elsif resource
        status_hash[:resource] = resource.name
      end
      status_hash[:status] = status_str
      status_hash[:messages] = messages if messages.is_a?(Array)

      status_hash[:meta] = {
        livecheckable: package_or_resource.livecheckable?,
      }
      status_hash[:meta][:head_only] = true if formula&.head_only?

      status_hash
    end

    # Formats and prints the livecheck result for a formula/cask/resource.
    sig { params(info: T::Hash[Symbol, T.untyped], verbose: T::Boolean, ambiguous_cask: T::Boolean).void }
    def print_latest_version(info, verbose: false, ambiguous_cask: false)
      package_or_resource_s = info[:resource].present? ? "  " : ""
      package_or_resource_s += "#{Tty.blue}#{info[:formula] || info[:cask] || info[:resource]}#{Tty.reset}"
      package_or_resource_s += " (cask)" if ambiguous_cask
      package_or_resource_s += " (guessed)" if verbose && !info[:meta][:livecheckable]

      current_s = if info[:version][:newer_than_upstream]
        "#{Tty.red}#{info[:version][:current]}#{Tty.reset}"
      else
        info[:version][:current]
      end

      latest_s = if info[:version][:outdated]
        "#{Tty.green}#{info[:version][:latest]}#{Tty.reset}"
      else
        info[:version][:latest]
      end

      puts "#{package_or_resource_s}: #{current_s} ==> #{latest_s}"
    end

    # Prints the livecheck result for the resources of a given Formula.
    sig { params(info: T::Array[T::Hash[Symbol, T.untyped]], verbose: T::Boolean).void }
    def print_resources_info(info, verbose: false)
      info.each do |r_info|
        if r_info[:status] && r_info[:messages]
          SkipConditions.print_skip_information(r_info)
        else
          print_latest_version(r_info, verbose:)
        end
      end
    end

    sig {
      params(
        livecheck_url:       T.any(String, Symbol),
        package_or_resource: T.any(Formula, Cask::Cask, Resource),
      ).returns(T.nilable(String))
    }
    def livecheck_url_to_string(livecheck_url, package_or_resource)
      case livecheck_url
      when String
        livecheck_url
      when :url
        package_or_resource.url&.to_s if package_or_resource.is_a?(Cask::Cask) || package_or_resource.is_a?(Resource)
      when :head, :stable
        package_or_resource.send(livecheck_url)&.url if package_or_resource.is_a?(Formula)
      when :homepage
        package_or_resource.homepage unless package_or_resource.is_a?(Resource)
      end
    end

    # Returns an Array containing the formula/cask/resource URLs that can be used by livecheck.
    sig { params(package_or_resource: T.any(Formula, Cask::Cask, Resource)).returns(T::Array[String]) }
    def checkable_urls(package_or_resource)
      urls = []

      case package_or_resource
      when Formula
        if package_or_resource.stable
          urls << T.must(package_or_resource.stable).url
          urls.concat(T.must(package_or_resource.stable).mirrors)
        end
        urls << T.must(package_or_resource.head).url if package_or_resource.head
        urls << package_or_resource.homepage if package_or_resource.homepage
      when Cask::Cask
        urls << package_or_resource.url.to_s if package_or_resource.url
        urls << package_or_resource.homepage if package_or_resource.homepage
      when Resource
        urls << package_or_resource.url
      else
        T.absurd(package_or_resource)
      end

      urls.compact.uniq
    end

    # Preprocesses and returns the URL used by livecheck.
    sig { params(url: String).returns(String) }
    def preprocess_url(url)
      begin
        uri = Addressable::URI.parse url
      rescue Addressable::URI::InvalidURIError
        return url
      end

      host = uri.host
      path = uri.path
      return url if host.nil? || path.nil?

      host = "github.com" if host == "github.s3.amazonaws.com"
      path = path.delete_prefix("/").delete_suffix(".git")
      scheme = uri.scheme

      if host == "github.com"
        return url if path.match? %r{/releases/latest/?$}

        owner, repo = path.delete_prefix("downloads/").split("/")
        url = "#{scheme}://#{host}/#{owner}/#{repo}.git"
      elsif GITEA_INSTANCES.include?(host)
        return url if path.match? %r{/releases/latest/?$}

        owner, repo = path.split("/")
        url = "#{scheme}://#{host}/#{owner}/#{repo}.git"
      elsif GOGS_INSTANCES.include?(host)
        owner, repo = path.split("/")
        url = "#{scheme}://#{host}/#{owner}/#{repo}.git"
      # sourcehut
      elsif host == "git.sr.ht"
        owner, repo = path.split("/")
        url = "#{scheme}://#{host}/#{owner}/#{repo}"
      # GitLab (gitlab.com or self-hosted)
      elsif path.include?("/-/archive/")
        url = url.sub(%r{/-/archive/.*$}i, ".git")
      end

      url
    end

    # livecheck should fetch a URL using brewed curl if the formula/cask
    # contains a `stable`/`url` or `head` URL `using: :homebrew_curl` that
    # shares the same root domain.
    sig { params(formula_or_cask: T.any(Formula, Cask::Cask), url: String).returns(T::Boolean) }
    def use_homebrew_curl?(formula_or_cask, url)
      url_root_domain = Addressable::URI.parse(url)&.domain
      return false if url_root_domain.blank?

      # Collect root domains of URLs with `using: :homebrew_curl`
      homebrew_curl_root_domains = []
      case formula_or_cask
      when Formula
        [:stable, :head].each do |spec_name|
          next unless (spec = formula_or_cask.send(spec_name))
          next if spec.using != :homebrew_curl

          domain = Addressable::URI.parse(spec.url)&.domain
          homebrew_curl_root_domains << domain if domain.present?
        end
      when Cask::Cask
        return false if formula_or_cask.url.using != :homebrew_curl

        domain = Addressable::URI.parse(formula_or_cask.url.to_s)&.domain
        homebrew_curl_root_domains << domain if domain.present?
      end

      homebrew_curl_root_domains.include?(url_root_domain)
    end

    # Identifies the latest version of the formula/cask and returns a Hash containing
    # the version information. Returns nil if a latest version couldn't be found.
    sig {
      params(
        formula_or_cask:            T.any(Formula, Cask::Cask),
        referenced_formula_or_cask: T.nilable(T.any(Formula, Cask::Cask)),
        livecheck_references:       T::Array[T.any(Formula, Cask::Cask)],
        json:                       T::Boolean,
        full_name:                  T::Boolean,
        verbose:                    T::Boolean,
        debug:                      T::Boolean,
      ).returns(T.nilable(T::Hash[Symbol, T.untyped]))
    }
    def latest_version(
      formula_or_cask,
      referenced_formula_or_cask: nil,
      livecheck_references: [],
      json: false, full_name: false, verbose: false, debug: false
    )
      formula = formula_or_cask if formula_or_cask.is_a?(Formula)
      cask = formula_or_cask if formula_or_cask.is_a?(Cask::Cask)

      has_livecheckable = formula_or_cask.livecheckable?
      livecheck = formula_or_cask.livecheck
      referenced_livecheck = referenced_formula_or_cask&.livecheck

      livecheck_url = livecheck.url || referenced_livecheck&.url
      livecheck_regex = livecheck.regex || referenced_livecheck&.regex
      livecheck_strategy = livecheck.strategy || referenced_livecheck&.strategy
      livecheck_strategy_block = livecheck.strategy_block || referenced_livecheck&.strategy_block
      livecheck_throttle = livecheck.throttle || referenced_livecheck&.throttle

      livecheck_url_string = livecheck_url_to_string(
        livecheck_url,
        referenced_formula_or_cask || formula_or_cask,
      )

      urls = [livecheck_url_string] if livecheck_url_string
      urls ||= checkable_urls(referenced_formula_or_cask || formula_or_cask)

      if debug
        if formula
          puts "Formula:          #{formula_name(formula, full_name:)}"
          puts "Head only?:       true" if formula.head_only?
        elsif cask
          puts "Cask:             #{cask_name(formula_or_cask, full_name:)}"
        end
        puts "Livecheckable?:   #{has_livecheckable ? "Yes" : "No"}"
        puts "Throttle:         #{livecheck_throttle}" if livecheck_throttle

        livecheck_references.each do |ref_formula_or_cask|
          case ref_formula_or_cask
          when Formula
            puts "Formula Ref:      #{formula_name(ref_formula_or_cask, full_name:)}"
          when Cask::Cask
            puts "Cask Ref:         #{cask_name(ref_formula_or_cask, full_name:)}"
          end
        end
      end

      checked_urls = []
      urls.each_with_index do |original_url, i|
        # Only preprocess the URL when it's appropriate
        url = if STRATEGY_SYMBOLS_TO_SKIP_PREPROCESS_URL.include?(livecheck_strategy)
          original_url
        else
          preprocess_url(original_url)
        end
        next if checked_urls.include?(url)

        strategies = Strategy.from_url(
          url,
          livecheck_strategy:,
          url_provided:       livecheck_url.present?,
          regex_provided:     livecheck_regex.present?,
          block_provided:     livecheck_strategy_block.present?,
        )
        strategy = Strategy.from_symbol(livecheck_strategy) || strategies.first
        strategy_name = livecheck_strategy_names[strategy]

        if debug
          puts
          if livecheck_url.is_a?(Symbol)
            # This assumes the URL symbol will fit within the available space
            puts "URL (#{livecheck_url}):".ljust(18, " ") + original_url
          else
            puts "URL:              #{original_url}"
          end
          puts "URL (processed):  #{url}" if url != original_url
          if strategies.present? && verbose
            puts "Strategies:       #{strategies.map { |s| livecheck_strategy_names[s] }.join(", ")}"
          end
          puts "Strategy:         #{strategy.blank? ? "None" : strategy_name}"
          puts "Regex:            #{livecheck_regex.inspect}" if livecheck_regex.present?
        end

        if livecheck_strategy.present?
          if livecheck_url.blank? && strategy.method(:find_versions).parameters.include?([:keyreq, :url])
            odebug "#{strategy_name} strategy requires a URL"
            next
          elsif livecheck_strategy != :page_match && strategies.exclude?(strategy)
            odebug "#{strategy_name} strategy does not apply to this URL"
            next
          end
        end

        next if strategy.blank?

        homebrew_curl = case strategy_name
        when "PageMatch", "HeaderMatch"
          use_homebrew_curl?((referenced_formula_or_cask || formula_or_cask), url)
        end
        puts "Homebrew curl?:   Yes" if debug && homebrew_curl.present?

        strategy_args = {
          regex:         livecheck_regex,
          homebrew_curl:,
        }
        # TODO: Set `cask`/`url` args based on the presence of the keyword arg
        # in the strategy's `#find_versions` method once we figure out why
        # `strategy.method(:find_versions).parameters` isn't working as
        # expected.
        strategy_args[:cask] = cask if strategy_name == "ExtractPlist" && cask.present?
        strategy_args[:url] = url
        strategy_args.compact!

        strategy_data = strategy.find_versions(**strategy_args, &livecheck_strategy_block)
        match_version_map = strategy_data[:matches]
        regex = strategy_data[:regex]
        messages = strategy_data[:messages]
        checked_urls << url

        if messages.is_a?(Array) && match_version_map.blank?
          puts messages unless json
          next if i + 1 < urls.length

          return status_hash(formula_or_cask, "error", messages, full_name:, verbose:)
        end

        if debug
          if strategy_data[:url].present? && strategy_data[:url] != url
            puts "URL (strategy):   #{strategy_data[:url]}"
          end
          puts "URL (final):      #{strategy_data[:final_url]}" if strategy_data[:final_url].present?
          if strategy_data[:regex].present? && strategy_data[:regex] != livecheck_regex
            puts "Regex (strategy): #{strategy_data[:regex].inspect}"
          end
          puts "Cached?:          Yes" if strategy_data[:cached] == true
        end

        match_version_map.delete_if do |_match, version|
          next true if version.blank?
          next false if has_livecheckable

          UNSTABLE_VERSION_KEYWORDS.any? do |rejection|
            version.to_s.include?(rejection)
          end
        end
        next if match_version_map.blank?

        if debug
          puts
          puts "Matched Versions:"

          if verbose
            match_version_map.each do |match, version|
              puts "#{match} => #{version.inspect}"
            end
          else
            puts match_version_map.values.join(", ")
          end
        end

        version_info = {
          latest: Version.new(match_version_map.values.max_by { |v| LivecheckVersion.create(formula_or_cask, v) }),
        }

        if livecheck_throttle
          match_version_map.keep_if { |_match, version| version.patch.to_i.modulo(livecheck_throttle).zero? }
          version_info[:latest_throttled] = if match_version_map.blank?
            nil
          else
            Version.new(match_version_map.values.max_by { |v| LivecheckVersion.create(formula_or_cask, v) })
          end

          if debug
            puts
            puts "Matched Throttled Versions:"

            if verbose
              match_version_map.each do |match, version|
                puts "#{match} => #{version.inspect}"
              end
            else
              puts match_version_map.values.join(", ")
            end
          end
        end

        if json && verbose
          version_info[:meta] = {}

          if livecheck_references.present?
            version_info[:meta][:references] = livecheck_references.map do |ref_formula_or_cask|
              case ref_formula_or_cask
              when Formula
                { formula: formula_name(ref_formula_or_cask, full_name:) }
              when Cask::Cask
                { cask: cask_name(ref_formula_or_cask, full_name:) }
              end
            end
          end

          version_info[:meta][:url] = {}
          version_info[:meta][:url][:symbol] = livecheck_url if livecheck_url.is_a?(Symbol) && livecheck_url_string
          version_info[:meta][:url][:original] = original_url
          version_info[:meta][:url][:processed] = url if url != original_url
          if strategy_data[:url].present? && strategy_data[:url] != url
            version_info[:meta][:url][:strategy] = strategy_data[:url]
          end
          version_info[:meta][:url][:final] = strategy_data[:final_url] if strategy_data[:final_url]
          version_info[:meta][:url][:homebrew_curl] = homebrew_curl if homebrew_curl.present?

          version_info[:meta][:strategy] = strategy.present? ? strategy_name : nil
          version_info[:meta][:strategies] = strategies.map { |s| livecheck_strategy_names[s] } if strategies.present?
          version_info[:meta][:regex] = regex.inspect if regex.present?
          version_info[:meta][:cached] = true if strategy_data[:cached] == true
          version_info[:meta][:throttle] = livecheck_throttle if livecheck_throttle
        end

        return version_info
      end
      nil
    end

    # Identifies the latest version of a resource and returns a Hash containing the
    # version information. Returns nil if a latest version couldn't be found.
    sig {
      params(
        resource:       Resource,
        formula_latest: String,
        json:           T::Boolean,
        debug:          T::Boolean,
        quiet:          T::Boolean,
        verbose:        T::Boolean,
      ).returns(T::Hash[Symbol, T.untyped])
    }
    def resource_version(
      resource,
      formula_latest,
      json: false,
      debug: false,
      quiet: false,
      verbose: false
    )
      has_livecheckable = resource.livecheckable?

      if debug
        puts "\n\n"
        puts "Resource:         #{resource.name}"
        puts "Livecheckable?:   #{has_livecheckable ? "Yes" : "No"}"
      end

      resource_version_info = {}

      livecheck = resource.livecheck
      livecheck_url = livecheck.url
      livecheck_regex = livecheck.regex
      livecheck_strategy = livecheck.strategy
      livecheck_strategy_block = livecheck.strategy_block

      livecheck_url_string = livecheck_url_to_string(livecheck_url, resource)

      urls = [livecheck_url_string] if livecheck_url_string
      urls ||= checkable_urls(resource)

      checked_urls = []
      urls.each_with_index do |original_url, i|
        url = original_url.gsub(Constants::LATEST_VERSION, formula_latest)

        # Only preprocess the URL when it's appropriate
        url = preprocess_url(url) unless STRATEGY_SYMBOLS_TO_SKIP_PREPROCESS_URL.include?(livecheck_strategy)

        next if checked_urls.include?(url)

        strategies = Strategy.from_url(
          url,
          livecheck_strategy:,
          url_provided:       livecheck_url.present?,
          regex_provided:     livecheck_regex.present?,
          block_provided:     livecheck_strategy_block.present?,
        )
        strategy = Strategy.from_symbol(livecheck_strategy) || strategies.first
        strategy_name = livecheck_strategy_names[strategy]

        if debug
          puts
          if livecheck_url.is_a?(Symbol)
            # This assumes the URL symbol will fit within the available space
            puts "URL (#{livecheck_url}):".ljust(18, " ") + original_url
          else
            puts "URL:              #{original_url}"
          end
          puts "URL (processed):  #{url}" if url != original_url
          if strategies.present? && verbose
            puts "Strategies:       #{strategies.map { |s| livecheck_strategy_names[s] }.join(", ")}"
          end
          puts "Strategy:         #{strategy.blank? ? "None" : strategy_name}"
          puts "Regex:            #{livecheck_regex.inspect}" if livecheck_regex.present?
        end

        if livecheck_strategy.present?
          if livecheck_url.blank? && strategy.method(:find_versions).parameters.include?([:keyreq, :url])
            odebug "#{strategy_name} strategy requires a URL"
            next
          elsif livecheck_strategy != :page_match && strategies.exclude?(strategy)
            odebug "#{strategy_name} strategy does not apply to this URL"
            next
          end
        end
        puts if debug && strategy.blank?
        next if strategy.blank?

        strategy_args = {
          url:,
          regex:         livecheck_regex,
          homebrew_curl: false,
        }.compact

        strategy_data = strategy.find_versions(**strategy_args, &livecheck_strategy_block)
        match_version_map = strategy_data[:matches]
        regex = strategy_data[:regex]
        messages = strategy_data[:messages]
        checked_urls << url

        if messages.is_a?(Array) && match_version_map.blank?
          puts messages unless json
          next if i + 1 < urls.length

          return status_hash(resource, "error", messages, verbose:)
        end

        if debug
          if strategy_data[:url].present? && strategy_data[:url] != url
            puts "URL (strategy):   #{strategy_data[:url]}"
          end
          puts "URL (final):      #{strategy_data[:final_url]}" if strategy_data[:final_url].present?
          if strategy_data[:regex].present? && strategy_data[:regex] != livecheck_regex
            puts "Regex (strategy): #{strategy_data[:regex].inspect}"
          end
          puts "Cached?:          Yes" if strategy_data[:cached] == true
        end

        match_version_map.delete_if do |_match, version|
          next true if version.blank?
          next false if has_livecheckable

          UNSTABLE_VERSION_KEYWORDS.any? do |rejection|
            version.to_s.include?(rejection)
          end
        end
        next if match_version_map.blank?

        if debug
          puts
          puts "Matched Versions:"

          if verbose
            match_version_map.each do |match, version|
              puts "#{match} => #{version.inspect}"
            end
          else
            puts match_version_map.values.join(", ")
          end
        end

        res_current = T.must(resource.version)
        res_latest = Version.new(match_version_map.values.max_by { |v| LivecheckVersion.create(resource, v) })

        return status_hash(resource, "error", ["Unable to get versions"], verbose:) if res_latest.blank?

        is_outdated = res_current < res_latest
        is_newer_than_upstream = res_current > res_latest

        resource_version_info = {
          resource: resource.name,
          version:  {
            current:             res_current.to_s,
            latest:              res_latest.to_s,
            outdated:            is_outdated,
            newer_than_upstream: is_newer_than_upstream,
          },
        }

        resource_version_info[:meta] = { livecheckable: has_livecheckable, url: {} }
        if livecheck_url.is_a?(Symbol) && livecheck_url_string
          resource_version_info[:meta][:url][:symbol] = livecheck_url
        end
        resource_version_info[:meta][:url][:original] = original_url
        resource_version_info[:meta][:url][:processed] = url if url != original_url
        if strategy_data[:url].present? && strategy_data[:url] != url
          resource_version_info[:meta][:url][:strategy] = strategy_data[:url]
        end
        resource_version_info[:meta][:url][:final] = strategy_data[:final_url] if strategy_data[:final_url]
        resource_version_info[:meta][:strategy] = strategy.present? ? strategy_name : nil
        if strategies.present?
          resource_version_info[:meta][:strategies] = strategies.map { |s| livecheck_strategy_names[s] }
        end
        resource_version_info[:meta][:regex] = regex.inspect if regex.present?
        resource_version_info[:meta][:cached] = true if strategy_data[:cached] == true
      rescue => e
        Homebrew.failed = true
        if json
          status_hash(resource, "error", [e.to_s], verbose:)
        elsif !quiet
          onoe "#{Tty.blue}#{resource.name}#{Tty.reset}: #{e}"
          $stderr.puts Utils::Backtrace.clean(e) if debug && !e.is_a?(Livecheck::Error)
          nil
        end
      end
      resource_version_info
    end
  end
end
