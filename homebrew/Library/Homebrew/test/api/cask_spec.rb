# frozen_string_literal: true

require "api"

RSpec.describe Homebrew::API::Cask do
  let(:cache_dir) { mktmpdir }

  before do
    stub_const("Homebrew::API::HOMEBREW_CACHE_API", cache_dir)
  end

  def mock_curl_download(stdout:)
    allow(Utils::Curl).to receive(:curl_download) do |*_args, **kwargs|
      kwargs[:to].write stdout
    end
    allow(Homebrew::API).to receive(:verify_and_parse_jws) do |json_data|
      [true, json_data]
    end
  end

  describe "::all_casks" do
    let(:casks_json) do
      <<~EOS
        [{
          "token": "foo",
          "url": "https://brew.sh/foo"
        }, {
          "token": "bar",
          "url": "https://brew.sh/bar"
        }]
      EOS
    end
    let(:casks_hash) do
      {
        "foo" => { "url" => "https://brew.sh/foo" },
        "bar" => { "url" => "https://brew.sh/bar" },
      }
    end

    it "returns the expected cask JSON list" do
      mock_curl_download stdout: casks_json
      casks_output = described_class.all_casks
      expect(casks_output).to eq casks_hash
    end
  end

  describe "::source_download", :needs_macos do
    let(:cask) do
      cask = Cask::CaskLoader::FromAPILoader.new(
        "everything",
        from_json: JSON.parse((TEST_FIXTURE_DIR/"cask/everything.json").read.strip),
      ).load(config: nil)
      cask
    end

    before do
      allow_any_instance_of(Homebrew::API::Download).to receive(:fetch)
      allow_any_instance_of(Homebrew::API::Download).to receive(:symlink_location).and_return(
        TEST_FIXTURE_DIR/"cask/Casks/everything.rb",
      )
    end

    it "specifies the correct URL and sha256" do
      expect(Homebrew::API::Download).to receive(:new).with(
        "https://raw.githubusercontent.com/Homebrew/homebrew-cask/abcdef1234567890abcdef1234567890abcdef12/Casks/everything.rb",
        Checksum.new("d8d0d6b2e5ff65388eccb82236fd3aa157b4a29bb043a1f72b97f0e9b70e8320"),
        any_args,
      ).and_call_original
      described_class.source_download(cask)
    end
  end
end
