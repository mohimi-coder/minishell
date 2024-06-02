# frozen_string_literal: true

require_relative "shared_examples"

RSpec.describe UnpackStrategy::Cvs do
  let(:repo) do
    mktmpdir.tap do |repo|
      FileUtils.touch repo/"test"
      (repo/"CVS").mkpath
    end
  end
  let(:path) { repo }

  include_examples "UnpackStrategy::detect"
  include_examples "#extract", children: ["CVS", "test"]
end
