# typed: true
# frozen_string_literal: true

# Helper functions for updating PyPI resources.
module PyPI
  PYTHONHOSTED_URL_PREFIX = "https://files.pythonhosted.org/packages/"
  private_constant :PYTHONHOSTED_URL_PREFIX

  # Represents a Python package.
  # This package can be a PyPI package (either by name/version or PyPI distribution URL),
  # or it can be a non-PyPI URL.
  class Package
    sig { params(package_string: String, is_url: T::Boolean, python_name: String).void }
    def initialize(package_string, is_url: false, python_name: "python")
      @pypi_info = nil
      @package_string = package_string
      @is_url = is_url
      @is_pypi_url = package_string.start_with? PYTHONHOSTED_URL_PREFIX
      @python_name = python_name
    end

    sig { returns(String) }
    def name
      basic_metadata if @name.blank?
      @name
    end

    sig { returns(T::Array[T.nilable(String)]) }
    def extras
      basic_metadata if @extras.blank?
      @extras
    end

    sig { returns(T.nilable(String)) }
    def version
      basic_metadata if @version.blank?
      @version
    end

    sig { params(new_version: String).void }
    def version=(new_version)
      raise ArgumentError, "can't update version for non-PyPI packages" unless valid_pypi_package?

      @version = new_version
    end

    sig { returns(T::Boolean) }
    def valid_pypi_package?
      @is_pypi_url || !@is_url
    end

    # Get name, URL, SHA-256 checksum and latest version for a given package.
    # This only works for packages from PyPI or from a PyPI URL; packages
    # derived from non-PyPI URLs will produce `nil` here.
    sig { params(new_version: T.nilable(T.any(String, Version))).returns(T.nilable(T::Array[String])) }
    def pypi_info(new_version: nil)
      return unless valid_pypi_package?
      return @pypi_info if @pypi_info.present? && new_version.blank?

      new_version ||= version
      metadata_url = if new_version.present?
        "https://pypi.org/pypi/#{name}/#{new_version}/json"
      else
        "https://pypi.org/pypi/#{name}/json"
      end
      out, _, status = Utils::Curl.curl_output metadata_url, "--location", "--fail"

      return unless status.success?

      begin
        json = JSON.parse out
      rescue JSON::ParserError
        return
      end

      sdist = json["urls"].find { |url| url["packagetype"] == "sdist" }
      return if sdist.nil?

      @pypi_info = [
        PyPI.normalize_python_package(json["info"]["name"]), sdist["url"],
        sdist["digests"]["sha256"], json["info"]["version"]
      ]
    end

    sig { returns(String) }
    def to_s
      if valid_pypi_package?
        out = name
        out += "[#{extras.join(",")}]" if extras.present?
        out += "==#{version}" if version.present?
        out
      else
        @package_string
      end
    end

    sig { params(other: Package).returns(T::Boolean) }
    def same_package?(other)
      # These names are pre-normalized, so we can compare them directly.
      name == other.name
    end

    # Compare only names so we can use .include? and .uniq on a Package array
    sig { params(other: Package).returns(T::Boolean) }
    def ==(other)
      same_package?(other)
    end
    alias eql? ==

    sig { returns(Integer) }
    def hash
      name.hash
    end

    sig { params(other: Package).returns(T.nilable(Integer)) }
    def <=>(other)
      name <=> other.name
    end

    private

    # Returns [name, [extras], version] for this package.
    def basic_metadata
      if @is_pypi_url
        match = File.basename(@package_string).match(/^(.+)-([a-z\d.]+?)(?:.tar.gz|.zip)$/)
        raise ArgumentError, "Package should be a valid PyPI URL" if match.blank?

        @name ||= PyPI.normalize_python_package match[1]
        @extras ||= []
        @version ||= match[2]
      elsif @is_url
        ensure_formula_installed!(@python_name)

        # The URL might be a source distribution hosted somewhere;
        # try and use `pip install -q --no-deps --dry-run --report ...` to get its
        # name and version.
        # Note that this is different from the (similar) `pip install --report` we
        # do below, in that it uses `--no-deps` because we only care about resolving
        # this specific URL's project metadata.
        command =
          [Formula[@python_name].opt_libexec/"bin/python", "-m", "pip", "install", "-q", "--no-deps",
           "--dry-run", "--ignore-installed", "--report", "/dev/stdout", @package_string]
        pip_output = Utils.popen_read({ "PIP_REQUIRE_VIRTUALENV" => "false" }, *command)
        unless $CHILD_STATUS.success?
          raise ArgumentError, <<~EOS
            Unable to determine metadata for "#{@package_string}" because of a failure when running
            `#{command.join(" ")}`.
          EOS
        end

        metadata = JSON.parse(pip_output)["install"].first["metadata"]

        @name ||= PyPI.normalize_python_package metadata["name"]
        @extras ||= []
        @version ||= metadata["version"]
      else
        if @package_string.include? "=="
          name, version = @package_string.split("==")
        else
          name = @package_string
          version = nil
        end

        if (match = T.must(name).match(/^(.*?)\[(.+)\]$/))
          name = match[1]
          extras = T.must(match[2]).split ","
        else
          extras = []
        end

        @name ||= PyPI.normalize_python_package name
        @extras ||= extras
        @version ||= version
      end
    end
  end

  sig { params(url: String, version: T.any(String, Version)).returns(T.nilable(String)) }
  def self.update_pypi_url(url, version)
    package = Package.new url, is_url: true

    return unless package.valid_pypi_package?

    _, url = package.pypi_info(new_version: version)
    url
  rescue ArgumentError
    nil
  end

  # Return true if resources were checked (even if no change).
  sig {
    params(
      formula:                  Formula,
      version:                  T.nilable(String),
      package_name:             T.nilable(String),
      extra_packages:           T.nilable(T::Array[String]),
      exclude_packages:         T.nilable(T::Array[String]),
      dependencies:             T.nilable(T::Array[String]),
      install_dependencies:     T.nilable(T::Boolean),
      print_only:               T.nilable(T::Boolean),
      silent:                   T.nilable(T::Boolean),
      verbose:                  T.nilable(T::Boolean),
      ignore_non_pypi_packages: T.nilable(T::Boolean),
    ).returns(T.nilable(T::Boolean))
  }
  def self.update_python_resources!(formula, version: nil, package_name: nil, extra_packages: nil,
                                    exclude_packages: nil, dependencies: nil, install_dependencies: false,
                                    print_only: false, silent: false, verbose: false,
                                    ignore_non_pypi_packages: false)

    auto_update_list = formula.tap&.pypi_formula_mappings
    if auto_update_list.present? && auto_update_list.key?(formula.full_name) &&
       package_name.blank? && extra_packages.blank? && exclude_packages.blank?

      list_entry = auto_update_list[formula.full_name]
      case list_entry
      when false
        unless print_only
          odie "The resources for \"#{formula.name}\" need special attention. Please update them manually."
        end
      when String
        package_name = list_entry
      when Hash
        package_name = list_entry["package_name"]
        extra_packages = list_entry["extra_packages"]
        exclude_packages = list_entry["exclude_packages"]
        dependencies = list_entry["dependencies"]
      end
    end

    missing_dependencies = Array(dependencies).reject do |dependency|
      Formula[dependency].any_version_installed?
    rescue FormulaUnavailableError
      odie "Formula \"#{dependency}\" not found but it is a dependency to update \"#{formula.name}\" resources."
    end
    if missing_dependencies.present?
      missing_msg = "formulae required to update \"#{formula.name}\" resources: #{missing_dependencies.join(", ")}"
      odie "Missing #{missing_msg}" unless install_dependencies
      ohai "Installing #{missing_msg}"
      missing_dependencies.each(&method(:ensure_formula_installed!))
    end

    python_deps = formula.deps
                         .select { |d| d.name.match?(/^python(@.+)?$/) }
                         .map(&:to_formula)
                         .sort_by(&:version)
                         .reverse
    python_name = if python_deps.empty?
      "python"
    else
      (python_deps.find(&:any_version_installed?) || python_deps.first).name
    end

    main_package = if package_name.present?
      package_string = package_name
      package_string += "==#{formula.version}" if version.blank? && formula.version.present?
      Package.new(package_string, python_name:)
    elsif package_name == ""
      nil
    else
      stable = T.must(formula.stable)
      url = if stable.specs[:tag].present?
        url = "git+#{stable.url}@#{stable.specs[:tag]}"
      else
        stable.url
      end
      Package.new(url, is_url: true, python_name:)
    end

    if main_package.nil?
      odie "The main package was skipped but no PyPI `extra_packages` were provided." if extra_packages.blank?
    elsif version.present?
      if main_package.valid_pypi_package?
        main_package.version = version
      else
        return if ignore_non_pypi_packages

        odie "The main package is not a PyPI package, meaning that version-only updates cannot be \
          performed. Please update its URL manually."
      end
    end

    extra_packages = (extra_packages || []).map { |p| Package.new p }
    exclude_packages = (exclude_packages || []).map { |p| Package.new p }
    exclude_packages += %w[argparse pip wsgiref].map { |p| Package.new p }
    if (newest_python = python_deps.first) && newest_python.version < Version.new("3.12")
      exclude_packages.append(Package.new("setuptools"))
    end
    # remove packages from the exclude list if we've explicitly requested them as an extra package
    exclude_packages.delete_if { |package| extra_packages.include?(package) }

    input_packages = Array(main_package)
    extra_packages.each do |extra_package|
      if !extra_package.valid_pypi_package? && !ignore_non_pypi_packages
        odie "\"#{extra_package}\" is not available on PyPI."
      end

      input_packages.each do |existing_package|
        if existing_package.same_package?(extra_package) && existing_package.version != extra_package.version
          odie "Conflicting versions specified for the `#{extra_package.name}` package: " \
               "#{existing_package.version}, #{extra_package.version}"
        end
      end

      input_packages << extra_package unless input_packages.include? extra_package
    end

    formula.resources.each do |resource|
      if !print_only && !resource.url.start_with?(PYTHONHOSTED_URL_PREFIX)
        odie "\"#{formula.name}\" contains non-PyPI resources. Please update the resources manually."
      end
    end

    ensure_formula_installed!(python_name)

    # Resolve the dependency tree of all input packages
    show_info = !print_only && !silent
    ohai "Retrieving PyPI dependencies for \"#{input_packages.join(" ")}\"..." if show_info
    found_packages = pip_report(input_packages, python_name:, print_stderr: verbose && show_info)
    # Resolve the dependency tree of excluded packages to prune the above
    exclude_packages.delete_if { |package| found_packages.exclude? package }
    ohai "Retrieving PyPI dependencies for excluded \"#{exclude_packages.join(" ")}\"..." if show_info
    exclude_packages = pip_report(exclude_packages, python_name:, print_stderr: verbose && show_info)
    exclude_packages += [Package.new(main_package.name)] unless main_package.nil?

    new_resource_blocks = ""
    found_packages.sort.each do |package|
      if exclude_packages.include? package
        ohai "Excluding \"#{package}\"" if show_info
        exclude_packages.delete package
        next
      end

      ohai "Getting PyPI info for \"#{package}\"" if show_info
      name, url, checksum = package.pypi_info
      # Fail if unable to find name, url or checksum for any resource
      if name.blank?
        odie "Unable to resolve some dependencies. Please update the resources for \"#{formula.name}\" manually."
      elsif url.blank? || checksum.blank?
        odie <<~EOS
          Unable to find the URL and/or sha256 for the "#{name}" resource.
          Please update the resources for "#{formula.name}" manually.
        EOS
      end

      # Append indented resource block
      new_resource_blocks += <<-EOS
  resource "#{name}" do
    url "#{url}"
    sha256 "#{checksum}"
  end

      EOS
    end

    odie "Excluded superfluous packages: #{exclude_packages.join(", ")}" if exclude_packages.any?

    if print_only
      puts new_resource_blocks.chomp
      return
    end

    # Check whether resources already exist (excluding virtualenv dependencies)
    if formula.resources.all? { |resource| resource.name.start_with?("homebrew-") }
      # Place resources above install method
      inreplace_regex = /  def install/
      new_resource_blocks += "  def install"
    else
      # Replace existing resource blocks with new resource blocks
      inreplace_regex = /
        \ \ (
        resource\ .*\ do\s+
          url\ .*\s+
          sha256\ .*\s+
          ((\#.*\s+)*
          patch\ (.*\ )?do\s+
            url\ .*\s+
            sha256\ .*\s+
          end\s+)*
        end\s+)+
      /x
      new_resource_blocks += "  "
    end

    ohai "Updating resource blocks" unless silent
    Utils::Inreplace.inreplace formula.path do |s|
      if T.must(s.inreplace_string.split(/^  test do\b/, 2).first).scan(inreplace_regex).length > 1
        odie "Unable to update resource blocks for \"#{formula.name}\" automatically. Please update them manually."
      end
      s.sub! inreplace_regex, new_resource_blocks
    end

    true
  end

  def self.normalize_python_package(name)
    # This normalization is defined in the PyPA packaging specifications;
    # https://packaging.python.org/en/latest/specifications/name-normalization/#name-normalization
    name.gsub(/[-_.]+/, "-").downcase
  end

  def self.pip_report(packages, python_name: "python", print_stderr: false)
    return [] if packages.blank?

    command = [
      Formula[python_name].opt_libexec/"bin/python", "-m", "pip", "install", "-q", "--disable-pip-version-check",
      "--dry-run", "--ignore-installed", "--report=/dev/stdout", *packages.map(&:to_s)
    ]
    options = {}
    options[:err] = :err if print_stderr
    pip_output = Utils.popen_read({ "PIP_REQUIRE_VIRTUALENV" => "false" }, *command, **options)
    unless $CHILD_STATUS.success?
      odie <<~EOS
        Unable to determine dependencies for "#{packages.join(" ")}" because of a failure when running
        `#{command.join(" ")}`.
        Please update the resources manually.
      EOS
    end
    pip_report_to_packages(JSON.parse(pip_output)).uniq
  end

  def self.pip_report_to_packages(report)
    return [] if report.blank?

    report["install"].filter_map do |package|
      name = normalize_python_package(package["metadata"]["name"])
      version = package["metadata"]["version"]

      Package.new "#{name}==#{version}"
    end
  end
end
