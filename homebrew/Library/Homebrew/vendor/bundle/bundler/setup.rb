require 'rbconfig'
module Kernel
  remove_method(:gem) if private_method_defined?(:gem)

  def gem(*)
  end

  private :gem
end
unless defined?(Gem)
  module Gem
    def self.ruby_api_version
      RbConfig::CONFIG["ruby_version"]
    end

    def self.extension_api_version
      if 'no' == RbConfig::CONFIG['ENABLE_SHARED']
        "#{ruby_api_version}-static"
      else
        ruby_api_version
      end
    end
  end
end
if Gem.respond_to?(:discover_gems_on_require=)
  Gem.discover_gems_on_require = false
else
  [::Kernel.singleton_class, ::Kernel].each do |k|
    if k.private_method_defined?(:gem_original_require)
      private_require = k.private_method_defined?(:require)
      k.send(:remove_method, :require)
      k.send(:define_method, :require, k.instance_method(:gem_original_require))
      k.send(:private, :require) if private_require
    end
  end
end
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/public_suffix-5.0.5/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/addressable-2.8.6/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/ast-2.4.2/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/extensions/arm64-darwin-20/#{Gem.extension_api_version}/bigdecimal-3.1.8")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/bigdecimal-3.1.8/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/bindata-2.5.0/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/extensions/arm64-darwin-20/#{Gem.extension_api_version}/msgpack-1.7.2")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/msgpack-1.7.2/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/extensions/arm64-darwin-20/#{Gem.extension_api_version}/bootsnap-1.18.3")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/bootsnap-1.18.3/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/coderay-1.1.3/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/highline-2.0.3/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/commander-4.6.0/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/diff-lcs-1.5.1/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/docile-1.4.0/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/elftools-1.3.1/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/erubi-1.12.0/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/hana-1.3.7/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/extensions/arm64-darwin-20/#{Gem.extension_api_version}/json-2.7.2")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/json-2.7.2/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/regexp_parser-2.9.2/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/simpleidn-0.2.3/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/json_schemer-2.3.0/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/extensions/arm64-darwin-20/#{Gem.extension_api_version}/strscan-3.1.0")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/strscan-3.1.0/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/rexml-3.2.8/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/kramdown-2.4.0/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/language_server-protocol-3.17.0.3/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/method_source-1.1.0/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/minitest-5.23.1/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/netrc-0.11.0/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/parallel-1.24.0/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/parallel_tests-4.7.1/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/extensions/arm64-darwin-20/#{Gem.extension_api_version}/racc-1.8.0")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/racc-1.8.0/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/parser-3.3.2.0/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/rainbow-3.1.1/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/sorbet-runtime-0.5.11406/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/parlour-8.1.0/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/patchelf-1.5.1/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/plist-3.7.1/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/extensions/arm64-darwin-20/#{Gem.extension_api_version}/prism-0.29.0")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/prism-0.29.0/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/pry-0.14.2/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/rbi-0.1.13/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/extensions/arm64-darwin-20/#{Gem.extension_api_version}/redcarpet-3.6.0")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/redcarpet-3.6.0/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/rspec-support-3.13.1/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/rspec-core-3.13.0/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/rspec-expectations-3.13.0/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/rspec-mocks-3.13.1/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/rspec-3.13.0/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/rspec-github-2.4.0/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/rspec-retry-0.6.2/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/rspec-sorbet-1.9.2/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/rspec_junit_formatter-0.6.0/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/rubocop-ast-1.31.3/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/ruby-progressbar-1.13.0/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/unicode-display_width-2.5.0/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/rubocop-1.64.1/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/rubocop-capybara-2.20.0/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/rubocop-factory_bot-2.25.1/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/rubocop-md-1.2.2/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/rubocop-performance-1.21.0/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/rubocop-rspec_rails-2.28.3/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/rubocop-rspec-2.29.2/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/rubocop-sorbet-0.8.3/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/ruby-macho-4.0.1/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/extensions/arm64-darwin-20/#{Gem.extension_api_version}/ruby-prof-1.7.0")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/ruby-prof-1.7.0/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/simplecov-html-0.12.3/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/simplecov_json_formatter-0.1.4/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/simplecov-0.22.0/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/simplecov-cobertura-2.1.0/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/sorbet-static-0.5.11406-universal-darwin/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/sorbet-0.5.11406/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/sorbet-static-and-runtime-0.5.11406/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/thor-1.3.1/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/spoom-1.3.2/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/extensions/arm64-darwin-20/#{Gem.extension_api_version}/stackprof-0.2.26")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/stackprof-0.2.26/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/yard-0.9.36/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/yard-sorbet-0.8.1/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/tapioca-0.14.3/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/extensions/arm64-darwin-20/#{Gem.extension_api_version}/vernier-1.0.1")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/vernier-1.0.1/lib")
$:.unshift File.expand_path("#{__dir__}/../#{RUBY_ENGINE}/#{Gem.ruby_api_version}/gems/warning-1.4.0/lib")
