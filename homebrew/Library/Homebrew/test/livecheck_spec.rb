# frozen_string_literal: true

require "formula"
require "livecheck"

RSpec.describe Livecheck do
  let(:f) do
    formula do
      homepage "https://brew.sh"
      url "https://brew.sh/test-0.0.1.tgz"
      head "https://github.com/Homebrew/brew.git"
    end
  end
  let(:livecheckable_f) { described_class.new(f.class) }

  let(:c) do
    Cask::CaskLoader.load(+<<-RUBY)
      cask "test" do
        version "0.0.1,2"

        url "https://brew.sh/test-0.0.1.dmg"
        name "Test"
        desc "Test cask"
        homepage "https://brew.sh"
      end
    RUBY
  end
  let(:livecheckable_c) { described_class.new(c) }

  describe "#formula" do
    it "returns nil if not set" do
      expect(livecheckable_f.formula).to be_nil
    end

    it "returns the String if set" do
      livecheckable_f.formula("other-formula")
      expect(livecheckable_f.formula).to eq("other-formula")
    end

    it "raises a TypeError if the argument isn't a String" do
      expect do
        livecheckable_f.formula(123)
      end.to raise_error TypeError
    end
  end

  describe "#cask" do
    it "returns nil if not set" do
      expect(livecheckable_c.cask).to be_nil
    end

    it "returns the String if set" do
      livecheckable_c.cask("other-cask")
      expect(livecheckable_c.cask).to eq("other-cask")
    end
  end

  describe "#regex" do
    it "returns nil if not set" do
      expect(livecheckable_f.regex).to be_nil
    end

    it "returns the Regexp if set" do
      livecheckable_f.regex(/foo/)
      expect(livecheckable_f.regex).to eq(/foo/)
    end
  end

  describe "#skip" do
    it "sets @skip to true when no argument is provided" do
      expect(livecheckable_f.skip).to be true
      expect(livecheckable_f.instance_variable_get(:@skip)).to be true
      expect(livecheckable_f.instance_variable_get(:@skip_msg)).to be_nil
    end

    it "sets @skip to true and @skip_msg to the provided String" do
      expect(livecheckable_f.skip("foo")).to be true
      expect(livecheckable_f.instance_variable_get(:@skip)).to be true
      expect(livecheckable_f.instance_variable_get(:@skip_msg)).to eq("foo")
    end
  end

  describe "#skip?" do
    it "returns the value of @skip" do
      expect(livecheckable_f.skip?).to be false

      livecheckable_f.skip
      expect(livecheckable_f.skip?).to be true
    end
  end

  describe "#strategy" do
    it "returns nil if not set" do
      expect(livecheckable_f.strategy).to be_nil
    end

    it "returns the Symbol if set" do
      livecheckable_f.strategy(:page_match)
      expect(livecheckable_f.strategy).to eq(:page_match)
    end
  end

  describe "#throttle" do
    it "returns nil if not set" do
      expect(livecheckable_f.throttle).to be_nil
    end

    it "returns the Integer if set" do
      livecheckable_f.throttle(10)
      expect(livecheckable_f.throttle).to eq(10)
    end
  end

  describe "#url" do
    let(:url_string) { "https://brew.sh" }

    it "returns nil if not set" do
      expect(livecheckable_f.url).to be_nil
    end

    it "returns a string when set to a string" do
      livecheckable_f.url(url_string)
      expect(livecheckable_f.url).to eq(url_string)
    end

    it "returns the URL symbol if valid" do
      livecheckable_f.url(:head)
      expect(livecheckable_f.url).to eq(:head)

      livecheckable_f.url(:homepage)
      expect(livecheckable_f.url).to eq(:homepage)

      livecheckable_f.url(:stable)
      expect(livecheckable_f.url).to eq(:stable)

      livecheckable_c.url(:url)
      expect(livecheckable_c.url).to eq(:url)
    end

    it "raises an ArgumentError if the argument isn't a valid Symbol" do
      expect do
        livecheckable_f.url(:not_a_valid_symbol)
      end.to raise_error ArgumentError
    end
  end

  describe "#to_hash" do
    it "returns a Hash of all instance variables" do
      expect(livecheckable_f.to_hash).to eq(
        {
          "cask"     => nil,
          "formula"  => nil,
          "regex"    => nil,
          "skip"     => false,
          "skip_msg" => nil,
          "strategy" => nil,
          "throttle" => nil,
          "url"      => nil,
        },
      )
    end
  end
end
