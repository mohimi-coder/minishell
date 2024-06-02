# typed: true
# frozen_string_literal: true

# This file is included before any other files. It intentionally has typing disabled and has minimal use of `require`.

required_ruby_major, required_ruby_minor, = ENV.fetch("HOMEBREW_REQUIRED_RUBY_VERSION", "").split(".").map(&:to_i)
gems_vendored = if required_ruby_minor.nil?
  # We're likely here if running RuboCop etc, so just assume we don't need to install gems as we likely already have
  true
else
  ruby_major, ruby_minor, = RUBY_VERSION.split(".").map(&:to_i)
  raise "Could not parse Ruby requirements" if !ruby_major || !ruby_minor || !required_ruby_major

  if ruby_major < required_ruby_major || (ruby_major == required_ruby_major && ruby_minor < required_ruby_minor)
    raise "Homebrew must be run under Ruby #{required_ruby_major}.#{required_ruby_minor}! " \
          "You're running #{RUBY_VERSION}."
  end

  # This list should match .gitignore
  vendored_versions = ["3.3"].freeze
  vendored_versions.include?("#{ruby_major}.#{ruby_minor}")
end.freeze

# We trust base Ruby to provide what we need.
# Don't look into the user-installed sitedir, which may contain older versions of RubyGems.
require "rbconfig"
$LOAD_PATH.reject! { |path| path.start_with?(RbConfig::CONFIG["sitedir"]) }

require "pathname"
dir = __dir__ || raise("__dir__ is not defined")
HOMEBREW_LIBRARY_PATH = Pathname(dir).parent.realpath.freeze

require_relative "../utils/gems"
Homebrew.setup_gem_environment!(setup_path: false)

# Install gems for Rubies we don't vendor for.
if !gems_vendored && !ENV["HOMEBREW_SKIP_INITIAL_GEM_INSTALL"]
  Homebrew.install_bundler_gems!(setup_path: false)
  ENV["HOMEBREW_SKIP_INITIAL_GEM_INSTALL"] = "1"
end

if Pathname.new(RbConfig.ruby).to_s.include?("/vendor/portable-ruby/")
  ruby_version = RbConfig::CONFIG["ruby_version"]
  ruby_path = "#{RbConfig::CONFIG["rubylibprefix"]}/gems/#{ruby_version}"

  $LOAD_PATH.unshift "#{ruby_path}/extensions/#{Gem::Platform.local}/#{ruby_version}-static/debug-1.9.1"
  $LOAD_PATH.unshift "#{ruby_path}/gems/debug-1.9.1/lib"
end

unless $LOAD_PATH.include?(HOMEBREW_LIBRARY_PATH.to_s)
  # Insert the path after any existing Homebrew paths (e.g. those inserted by tests and parent processes)
  last_homebrew_path_idx = $LOAD_PATH.rindex do |path|
    path.start_with?(HOMEBREW_LIBRARY_PATH.to_s) && !path.include?("vendor/portable-ruby")
  end || -1
  $LOAD_PATH.insert(last_homebrew_path_idx + 1, HOMEBREW_LIBRARY_PATH.to_s)
end
require_relative "../vendor/bundle/bundler/setup"
$LOAD_PATH.unshift "#{HOMEBREW_LIBRARY_PATH}/vendor/bundle/#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/" \
                   "bundler-#{Homebrew::HOMEBREW_BUNDLER_VERSION}/lib"
$LOAD_PATH.uniq!
