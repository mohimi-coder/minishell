# typed: true
# frozen_string_literal: true

module OS
  module Mac
    # Helper module for querying Xcode information.
    module Xcode
      DEFAULT_BUNDLE_PATH = Pathname("/Applications/Xcode.app").freeze
      BUNDLE_ID = "com.apple.dt.Xcode"
      OLD_BUNDLE_ID = "com.apple.Xcode"
      APPLE_DEVELOPER_DOWNLOAD_URL = "https://developer.apple.com/download/all/"

      # Bump these when a new version is available from the App Store and our
      # CI systems have been updated.
      # This may be a beta version for a beta macOS.
      sig { params(macos: MacOSVersion).returns(String) }
      def self.latest_version(macos: MacOS.version)
        latest_stable = "15.1"
        case macos
        when "14", "13" then latest_stable
        when "12" then "14.2"
        when "11" then "13.2.1"
        when "10.15" then "12.4"
        when "10.14" then "11.3.1"
        when "10.13" then "10.1"
        when "10.12" then "9.2"
        when "10.11" then "8.2.1"
        else
          raise "macOS '#{MacOS.version}' is invalid" unless OS::Mac.version.prerelease?

          # Default to newest known version of Xcode for unreleased macOS versions.
          latest_stable
        end
      end

      # Bump these if things are badly broken (e.g. no SDK for this macOS)
      # without this. Generally this will be the first Xcode release on that
      # macOS version (which may initially be a beta if that version of macOS is
      # also in beta).
      sig { returns(String) }
      def self.minimum_version
        case MacOS.version
        when "14" then "15.0"
        when "13" then "14.1"
        when "12" then "13.1"
        when "11" then "12.2"
        when "10.15" then "11.0"
        when "10.14" then "10.2"
        when "10.13" then "9.0"
        when "10.12" then "8.0"
        else "7.3"
        end
      end

      sig { returns(T::Boolean) }
      def self.below_minimum_version?
        return false unless installed?

        version < minimum_version
      end

      sig { returns(T::Boolean) }
      def self.latest_sdk_version?
        OS::Mac.full_version >= OS::Mac.latest_sdk_version
      end

      sig { returns(T::Boolean) }
      def self.needs_clt_installed?
        return false if latest_sdk_version?

        # With fake El Capitan for Portable Ruby, we want the full 10.11 SDK so that we can link
        # against the correct set of libraries in the SDK sysroot rather than the system's copies.
        # We therefore do not use the CLT under this setup, which installs to /usr/include.
        return false if ENV["HOMEBREW_FAKE_MACOS"]

        without_clt?
      end

      sig { returns(T::Boolean) }
      def self.outdated?
        return false unless installed?

        version < latest_version
      end

      sig { returns(T::Boolean) }
      def self.without_clt?
        !MacOS::CLT.installed?
      end

      # Returns a Pathname object corresponding to Xcode.app's Developer
      # directory or nil if Xcode.app is not installed.
      sig { returns(T.nilable(Pathname)) }
      def self.prefix
        @prefix ||= begin
          dir = MacOS.active_developer_dir

          if dir.empty? || dir == CLT::PKG_PATH || !File.directory?(dir)
            path = bundle_path
            path/"Contents/Developer" if path
          else
            # Use cleanpath to avoid pathological trailing slash
            Pathname.new(dir).cleanpath
          end
        end
      end

      sig { returns(Pathname) }
      def self.toolchain_path
        Pathname("#{prefix}/Toolchains/XcodeDefault.xctoolchain")
      end

      sig { returns(T.nilable(Pathname)) }
      def self.bundle_path
        # Use the default location if it exists.
        return DEFAULT_BUNDLE_PATH if DEFAULT_BUNDLE_PATH.exist?

        # Ask Spotlight where Xcode is. If the user didn't install the
        # helper tools and installed Xcode in a non-conventional place, this
        # is our only option. See: https://superuser.com/questions/390757
        MacOS.app_with_bundle_id(BUNDLE_ID, OLD_BUNDLE_ID)
      end

      sig { returns(T::Boolean) }
      def self.installed?
        odisabled "`MacOS::Xcode.installed?` on Linux" if Homebrew::SimulateSystem.simulating_or_running_on_linux?
        !prefix.nil?
      end

      sig { returns(XcodeSDKLocator) }
      def self.sdk_locator
        @sdk_locator ||= XcodeSDKLocator.new
      end

      sig { params(version: T.nilable(MacOSVersion)).returns(T.nilable(SDK)) }
      def self.sdk(version = nil)
        sdk_locator.sdk_if_applicable(version)
      end

      sig { params(version: T.nilable(MacOSVersion)).returns(T.nilable(Pathname)) }
      def self.sdk_path(version = nil)
        sdk(version)&.path
      end

      sig { returns(String) }
      def self.installation_instructions
        if OS::Mac.version.prerelease?
          <<~EOS
            Xcode can be installed from:
              #{Formatter.url(APPLE_DEVELOPER_DOWNLOAD_URL)}
          EOS
        else
          <<~EOS
            Xcode can be installed from the App Store.
          EOS
        end
      end

      sig { returns(String) }
      def self.update_instructions
        if OS::Mac.version.prerelease?
          <<~EOS
            Xcode can be updated from:
              #{Formatter.url(APPLE_DEVELOPER_DOWNLOAD_URL)}
          EOS
        else
          <<~EOS
            Xcode can be updated from the App Store.
          EOS
        end
      end

      # Get the Xcode version.
      #
      # @api internal
      sig { returns(::Version) }
      def self.version
        odisabled "`MacOS::Xcode.version` on Linux" if Homebrew::SimulateSystem.simulating_or_running_on_linux?
        # may return a version string
        # that is guessed based on the compiler, so do not
        # use it in order to check if Xcode is installed.
        if @version ||= detect_version
          ::Version.new @version
        else
          ::Version::NULL
        end
      end

      sig { returns(T.nilable(String)) }
      def self.detect_version
        # This is a separate function as you can't cache the value out of a block
        # if return is used in the middle, which we do many times in here.
        return if !MacOS::Xcode.installed? && !MacOS::CLT.installed?

        if MacOS::Xcode.installed?
          # Fast path that will probably almost always work unless `xcode-select -p` is misconfigured
          version_plist = T.must(prefix).parent/"version.plist"
          if version_plist.file?
            data = Plist.parse_xml(version_plist, marshal: false)
            version = data["CFBundleShortVersionString"] if data
            return version if version
          end

          %W[
            #{prefix}/usr/bin/xcodebuild
            #{which("xcodebuild")}
          ].uniq.each do |xcodebuild_path|
            next unless File.executable? xcodebuild_path

            xcodebuild_output = Utils.popen_read(xcodebuild_path, "-version")
            next unless $CHILD_STATUS.success?

            xcode_version = xcodebuild_output[/Xcode (\d+(\.\d+)*)/, 1]
            return xcode_version if xcode_version

            # Xcode 2.x's xcodebuild has a different version string
            case xcodebuild_output[/DevToolsCore-(\d+\.\d)/, 1]
            when "798.0" then return "2.5"
            when "515.0" then return "2.0"
            end
          end
        end

        detect_version_from_clang_version
      end

      sig { returns(String) }
      def self.detect_version_from_clang_version
        version = DevelopmentTools.clang_version

        return "dunno" if version.null?

        # This logic provides a fake Xcode version based on the
        # installed CLT version. This is useful as they are packaged
        # simultaneously so workarounds need to apply to both based on their
        # comparable version.
        case version
        when "6.0.0"  then "6.2"
        when "6.1.0"  then "6.4"
        when "7.0.0"  then "7.1"
        when "7.0.2"  then "7.2.1"
        when "7.3.0"  then "7.3.1"
        when "8.0.0"  then "8.2.1"
        when "8.1.0"  then "8.3.3"
        when "9.0.0"  then "9.2"
        when "9.1.0"  then "9.4.1"
        when "10.0.0" then "10.1"
        when "10.0.1" then "10.3"
        when "11.0.0" then "11.3.1"
        when "11.0.3" then "11.7"
        when "12.0.0" then "12.4"
        when "12.0.5" then "12.5.1"
        when "13.0.0" then "13.2.1"
        when "13.1.6" then "13.4.1"
        when "14.0.0" then "14.2"
        when "15.0.0" then "15.1"
        else               "14.3"
        end
      end

      sig { returns(T::Boolean) }
      def self.default_prefix?
        prefix.to_s == "/Applications/Xcode.app/Contents/Developer"
      end
    end

    # Helper module for querying macOS Command Line Tools information.
    module CLT
      # The original Mavericks CLT package ID
      EXECUTABLE_PKG_ID = "com.apple.pkg.CLTools_Executables"
      MAVERICKS_NEW_PKG_ID = "com.apple.pkg.CLTools_Base" # obsolete
      PKG_PATH = "/Library/Developer/CommandLineTools"

      # Returns true even if outdated tools are installed.
      sig { returns(T::Boolean) }
      def self.installed?
        odisabled "`MacOS::CLT.installed?` on Linux" if Homebrew::SimulateSystem.simulating_or_running_on_linux?
        !version.null?
      end

      sig { returns(T::Boolean) }
      def self.separate_header_package?
        version >= "10" && MacOS.version >= "10.14"
      end

      sig { returns(T::Boolean) }
      def self.provides_sdk?
        version >= "8"
      end

      sig { returns(CLTSDKLocator) }
      def self.sdk_locator
        @sdk_locator ||= CLTSDKLocator.new
      end

      sig { params(version: T.nilable(MacOSVersion)).returns(T.nilable(SDK)) }
      def self.sdk(version = nil)
        sdk_locator.sdk_if_applicable(version)
      end

      sig { params(version: T.nilable(MacOSVersion)).returns(T.nilable(Pathname)) }
      def self.sdk_path(version = nil)
        sdk(version)&.path
      end

      sig { returns(String) }
      def self.installation_instructions
        if MacOS.version == "10.14"
          # This is not available from `xcode-select`
          <<~EOS
            Install the Command Line Tools for Xcode 11.3.1 from:
              #{Formatter.url(MacOS::Xcode::APPLE_DEVELOPER_DOWNLOAD_URL)}
          EOS
        else
          <<~EOS
            Install the Command Line Tools:
              xcode-select --install
          EOS
        end
      end

      sig { returns(String) }
      def self.update_instructions
        software_update_location = if MacOS.version >= "13"
          "System Settings"
        elsif MacOS.version >= "10.14"
          "System Preferences"
        else
          "the App Store"
        end

        <<~EOS
          Update them from Software Update in #{software_update_location}.

          If that doesn't show you any updates, run:
            sudo rm -rf /Library/Developer/CommandLineTools
            sudo xcode-select --install

          Alternatively, manually download them from:
            #{Formatter.url(MacOS::Xcode::APPLE_DEVELOPER_DOWNLOAD_URL)}.
          You should download the Command Line Tools for Xcode #{MacOS::Xcode.latest_version}.
        EOS
      end

      # Bump these when the new version is distributed through Software Update
      # and our CI systems have been updated.
      sig { returns(String) }
      def self.latest_clang_version
        case MacOS.version
        when "14", "13" then "1500.1.0.2.5"
        when "12"    then "1400.0.29.202"
        when "11"    then "1300.0.29.30"
        when "10.15" then "1200.0.32.29"
        when "10.14" then "1100.0.33.17"
        when "10.13" then "1000.10.44.2"
        when "10.12" then "900.0.39.2"
        else              "800.0.42.1"
        end
      end

      # Bump these if things are badly broken (e.g. no SDK for this macOS)
      # without this. Generally this will be the first stable CLT release on
      # that macOS version.
      sig { returns(String) }
      def self.minimum_version
        case MacOS.version
        when "14" then "15.0.0"
        when "13" then "14.0.0"
        when "12" then "13.0.0"
        when "11" then "12.5.0"
        when "10.15" then "11.0.0"
        when "10.14" then "10.0.0"
        when "10.13" then "9.0.0"
        when "10.12" then "8.0.0"
        else              "7.3.0"
        end
      end

      sig { returns(T::Boolean) }
      def self.below_minimum_version?
        return false unless installed?

        version < minimum_version
      end

      sig { returns(T::Boolean) }
      def self.outdated?
        clang_version = detect_clang_version
        return false unless clang_version

        ::Version.new(clang_version) < latest_clang_version
      end

      sig { returns(T.nilable(String)) }
      def self.detect_clang_version
        version_output = Utils.popen_read("#{PKG_PATH}/usr/bin/clang", "--version")
        version_output[/clang-(\d+(\.\d+)+)/, 1]
      end

      sig { returns(T.nilable(String)) }
      def self.detect_version_from_clang_version
        detect_clang_version&.sub(/^(\d+)0(\d)\./, "\\1.\\2.")
      end

      # Version string (a pretty long one) of the CLT package.
      # Note that the different ways of installing the CLTs lead to different
      # version numbers.
      #
      # @api internal
      sig { returns(::Version) }
      def self.version
        odisabled "`MacOS::CLT.version` on Linux" if Homebrew::SimulateSystem.simulating_or_running_on_linux?
        if @version ||= detect_version
          ::Version.new @version
        else
          ::Version::NULL
        end
      end

      sig { returns(T.nilable(String)) }
      def self.detect_version
        version = T.let(nil, T.nilable(String))
        [EXECUTABLE_PKG_ID, MAVERICKS_NEW_PKG_ID].each do |id|
          next unless File.exist?("#{PKG_PATH}/usr/bin/clang")

          version = MacOS.pkgutil_info(id)[/version: (.+)$/, 1]
          return version if version
        end

        detect_version_from_clang_version
      end
    end
  end
end
