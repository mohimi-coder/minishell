# typed: true
# frozen_string_literal: true

require "attrable"
require "locale"
require "lazy_object"
require "livecheck"

require "cask/artifact"
require "cask/artifact_set"

require "cask/caskroom"
require "cask/exceptions"

require "cask/dsl/base"
require "cask/dsl/caveats"
require "cask/dsl/conflicts_with"
require "cask/dsl/container"
require "cask/dsl/depends_on"
require "cask/dsl/postflight"
require "cask/dsl/preflight"
require "cask/dsl/uninstall_postflight"
require "cask/dsl/uninstall_preflight"
require "cask/dsl/version"

require "cask/url"
require "cask/utils"

require "extend/on_system"

module Cask
  # Class representing the domain-specific language used for casks.
  class DSL
    ORDINARY_ARTIFACT_CLASSES = [
      Artifact::Installer,
      Artifact::App,
      Artifact::Artifact,
      Artifact::AudioUnitPlugin,
      Artifact::Binary,
      Artifact::Colorpicker,
      Artifact::Dictionary,
      Artifact::Font,
      Artifact::InputMethod,
      Artifact::InternetPlugin,
      Artifact::KeyboardLayout,
      Artifact::Manpage,
      Artifact::Pkg,
      Artifact::Prefpane,
      Artifact::Qlplugin,
      Artifact::Mdimporter,
      Artifact::ScreenSaver,
      Artifact::Service,
      Artifact::StageOnly,
      Artifact::Suite,
      Artifact::VstPlugin,
      Artifact::Vst3Plugin,
      Artifact::Uninstall,
      Artifact::Zap,
    ].freeze

    ACTIVATABLE_ARTIFACT_CLASSES = (ORDINARY_ARTIFACT_CLASSES - [Artifact::StageOnly]).freeze

    ARTIFACT_BLOCK_CLASSES = [
      Artifact::PreflightBlock,
      Artifact::PostflightBlock,
    ].freeze

    DSL_METHODS = Set.new([
      :appcast,
      :arch,
      :artifacts,
      :auto_updates,
      :caveats,
      :conflicts_with,
      :container,
      :desc,
      :depends_on,
      :homepage,
      :language,
      :name,
      :sha256,
      :staged_path,
      :url,
      :version,
      :appdir,
      :deprecate!,
      :deprecated?,
      :deprecation_date,
      :deprecation_reason,
      :disable!,
      :disabled?,
      :disable_date,
      :disable_reason,
      :discontinued?, # TODO: remove once discontinued? is removed (4.5.0)
      :livecheck,
      :livecheckable?,
      :on_system_blocks_exist?,
      *ORDINARY_ARTIFACT_CLASSES.map(&:dsl_key),
      *ACTIVATABLE_ARTIFACT_CLASSES.map(&:dsl_key),
      *ARTIFACT_BLOCK_CLASSES.flat_map { |klass| [klass.dsl_key, klass.uninstall_dsl_key] },
    ]).freeze

    extend Attrable
    include OnSystem::MacOSOnly

    attr_reader :cask, :token, :deprecation_date, :deprecation_reason, :disable_date, :disable_reason

    attr_predicate :on_system_blocks_exist?, :deprecated?, :disabled?, :livecheckable?

    def initialize(cask)
      @cask = cask
      @token = cask.token
    end

    # Specifies the cask's name.
    #
    # NOTE: Multiple names can be specified.
    #
    # ### Example
    #
    # ```ruby
    # name "Visual Studio Code"
    # ```
    #
    # @api public
    def name(*args)
      @name ||= []
      return @name if args.empty?

      @name.concat(args.flatten)
    end

    # Describes the cask.
    #
    # ### Example
    #
    # ```ruby
    # desc "Open-source code editor"
    # ```
    #
    # @api public
    def desc(description = nil)
      set_unique_stanza(:desc, description.nil?) { description }
    end

    def set_unique_stanza(stanza, should_return)
      return instance_variable_get(:"@#{stanza}") if should_return

      unless @cask.allow_reassignment
        if instance_variable_defined?(:"@#{stanza}") && !@called_in_on_system_block
          raise CaskInvalidError.new(cask, "'#{stanza}' stanza may only appear once.")
        end

        if instance_variable_defined?(:"@#{stanza}_set_in_block") && @called_in_on_system_block
          raise CaskInvalidError.new(cask, "'#{stanza}' stanza may only be overridden once.")
        end
      end

      instance_variable_set(:"@#{stanza}_set_in_block", true) if @called_in_on_system_block
      instance_variable_set(:"@#{stanza}", yield)
    rescue CaskInvalidError
      raise
    rescue => e
      raise CaskInvalidError.new(cask, "'#{stanza}' stanza failed with: #{e}")
    end

    # Sets the cask's homepage.
    #
    # ### Example
    #
    # ```ruby
    # homepage "https://code.visualstudio.com/"
    # ```
    #
    # @api public
    def homepage(homepage = nil)
      set_unique_stanza(:homepage, homepage.nil?) { homepage }
    end

    def language(*args, default: false, &block)
      if args.empty?
        language_eval
      elsif block
        @language_blocks ||= {}
        @language_blocks[args] = block

        return unless default

        if !@cask.allow_reassignment && @language_blocks.default.present?
          raise CaskInvalidError.new(cask, "Only one default language may be defined.")
        end

        @language_blocks.default = block
      else
        raise CaskInvalidError.new(cask, "No block given to language stanza.")
      end
    end

    def language_eval
      return @language_eval if defined?(@language_eval)

      return @language_eval = nil if @language_blocks.blank?

      raise CaskInvalidError.new(cask, "No default language specified.") if @language_blocks.default.nil?

      locales = cask.config.languages
                    .filter_map do |language|
                      Locale.parse(language)
                    rescue Locale::ParserError
                      nil
                    end

      locales.each do |locale|
        key = locale.detect(@language_blocks.keys)

        next if key.nil?

        return @language_eval = @language_blocks[key].call
      end

      @language_eval = @language_blocks.default.call
    end

    def languages
      return [] if @language_blocks.nil?

      @language_blocks.keys.flatten
    end

    # Sets the cask's download URL.
    #
    # ### Example
    #
    # ```ruby
    # url "https://update.code.visualstudio.com/#{version}/#{arch}/stable"
    # ```
    #
    # @api public
    def url(*args, **options, &block)
      caller_location = T.must(caller_locations).fetch(0)

      set_unique_stanza(:url, args.empty? && options.empty? && !block) do
        if block
          URL.new(*args, **options, caller_location:, dsl: self, &block)
        else
          URL.new(*args, **options, caller_location:)
        end
      end
    end

    # Sets the cask's container type or nested container path.
    #
    # ### Examples
    #
    # The container is a nested disk image:
    #
    # ```ruby
    # container nested: "orca-#{version}.dmg"
    # ```
    #
    # The container should not be unarchived:
    #
    # ```ruby
    # container type: :naked
    # ```
    #
    # @api public
    def container(**kwargs)
      set_unique_stanza(:container, kwargs.empty?) do
        DSL::Container.new(**kwargs)
      end
    end

    # Sets the cask's version.
    #
    # ### Example
    #
    # ```ruby
    # version "1.88.1"
    # ```
    #
    # @see DSL::Version
    # @api public
    def version(arg = nil)
      set_unique_stanza(:version, arg.nil?) do
        if !arg.is_a?(String) && arg != :latest
          raise CaskInvalidError.new(cask, "invalid 'version' value: #{arg.inspect}")
        end

        DSL::Version.new(arg)
      end
    end

    # Sets the cask's download checksum.
    #
    # ### Example
    #
    # For universal or single-architecture downloads:
    #
    # ```ruby
    # sha256 "7bdb497080ffafdfd8cc94d8c62b004af1be9599e865e5555e456e2681e150ca"
    # ```
    #
    # For architecture-dependent downloads:
    #
    # ```ruby
    # sha256 arm:   "7bdb497080ffafdfd8cc94d8c62b004af1be9599e865e5555e456e2681e150ca",
    #         intel: "b3c1c2442480a0219b9e05cf91d03385858c20f04b764ec08a3fa83d1b27e7b2"
    # ```
    #
    # @api public
    def sha256(arg = nil, arm: nil, intel: nil)
      should_return = arg.nil? && arm.nil? && intel.nil?

      set_unique_stanza(:sha256, should_return) do
        @on_system_blocks_exist = true if arm.present? || intel.present?

        val = arg || on_arch_conditional(arm:, intel:)
        case val
        when :no_check
          val
        when String
          Checksum.new(val)
        else
          raise CaskInvalidError.new(cask, "invalid 'sha256' value: #{val.inspect}")
        end
      end
    end

    # Sets the cask's architecture strings.
    #
    # ### Example
    #
    # ```ruby
    # arch arm: "darwin-arm64", intel: "darwin"
    # ```
    #
    # @api public
    def arch(arm: nil, intel: nil)
      should_return = arm.nil? && intel.nil?

      set_unique_stanza(:arch, should_return) do
        @on_system_blocks_exist = true

        on_arch_conditional(arm:, intel:)
      end
    end

    # Declare dependencies and requirements for a cask.
    #
    # NOTE: Multiple dependencies can be specified.
    #
    # @api public
    def depends_on(**kwargs)
      @depends_on ||= DSL::DependsOn.new
      return @depends_on if kwargs.empty?

      begin
        @depends_on.load(**kwargs)
      rescue RuntimeError => e
        raise CaskInvalidError.new(cask, e)
      end
      @depends_on
    end

    # Declare conflicts that keep a cask from installing or working correctly.
    #
    # @api public
    def conflicts_with(**kwargs)
      # TODO: Remove this constraint and instead merge multiple `conflicts_with` stanzas
      set_unique_stanza(:conflicts_with, kwargs.empty?) { DSL::ConflictsWith.new(**kwargs) }
    end

    def artifacts
      @artifacts ||= ArtifactSet.new
    end

    def caskroom_path
      cask.caskroom_path
    end

    # The staged location for this cask, including version number.
    #
    # @api public
    def staged_path
      return @staged_path if @staged_path

      cask_version = version || :unknown
      @staged_path = caskroom_path.join(cask_version.to_s)
    end

    # Provide the user with cask-specific information at install time.
    #
    # @api public
    def caveats(*strings, &block)
      @caveats ||= DSL::Caveats.new(cask)
      if block
        @caveats.eval_caveats(&block)
      elsif strings.any?
        strings.each do |string|
          @caveats.eval_caveats { string }
        end
      else
        return @caveats.to_s
      end
      @caveats
    end

    def discontinued?
      odeprecated "`discontinued?`", "`deprecated?` or `disabled?`"
      @caveats&.discontinued? == true
    end

    # Asserts that the cask artifacts auto-update.
    #
    # @api public
    def auto_updates(auto_updates = nil)
      set_unique_stanza(:auto_updates, auto_updates.nil?) { auto_updates }
    end

    # Automatically fetch the latest version of a cask from changelogs.
    #
    # @api public
    def livecheck(&block)
      @livecheck ||= Livecheck.new(cask)
      return @livecheck unless block

      if !@cask.allow_reassignment && @livecheckable
        raise CaskInvalidError.new(cask, "'livecheck' stanza may only appear once.")
      end

      @livecheckable = true
      @livecheck.instance_eval(&block)
    end

    # Declare that a cask is no longer functional or supported.
    #
    # NOTE: A warning will be shown when trying to install this cask.
    #
    # @api public
    def deprecate!(date:, because:)
      @deprecation_date = Date.parse(date)
      return if @deprecation_date > Date.today

      @deprecation_reason = because
      @deprecated = true
    end

    # Declare that a cask is no longer functional or supported.
    #
    # NOTE: An error will be thrown when trying to install this cask.
    #
    # @api public
    def disable!(date:, because:)
      @disable_date = Date.parse(date)

      if @disable_date > Date.today
        @deprecation_reason = because
        @deprecated = true
        return
      end

      @disable_reason = because
      @disabled = true
    end

    ORDINARY_ARTIFACT_CLASSES.each do |klass|
      define_method(klass.dsl_key) do |*args, **kwargs|
        T.bind(self, DSL)
        if [*artifacts.map(&:class), klass].include?(Artifact::StageOnly) &&
           artifacts.map(&:class).intersect?(ACTIVATABLE_ARTIFACT_CLASSES)
          raise CaskInvalidError.new(cask, "'stage_only' must be the only activatable artifact.")
        end

        artifacts.add(klass.from_args(cask, *args, **kwargs))
      rescue CaskInvalidError
        raise
      rescue => e
        raise CaskInvalidError.new(cask, "invalid '#{klass.dsl_key}' stanza: #{e}")
      end
    end

    ARTIFACT_BLOCK_CLASSES.each do |klass|
      [klass.dsl_key, klass.uninstall_dsl_key].each do |dsl_key|
        define_method(dsl_key) do |&block|
          T.bind(self, DSL)
          artifacts.add(klass.new(cask, dsl_key => block))
        end
      end
    end

    def method_missing(method, *)
      if method
        Utils.method_missing_message(method, token)
        nil
      else
        super
      end
    end

    def respond_to_missing?(*)
      true
    end

    # The directory `app`s are installed into.
    #
    # @api public
    def appdir
      return HOMEBREW_CASK_APPDIR_PLACEHOLDER if Cask.generating_hash?

      cask.config.appdir
    end
  end
end
