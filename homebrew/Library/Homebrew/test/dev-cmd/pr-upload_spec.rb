# frozen_string_literal: true

require "cmd/shared_examples/args_parse"
require "dev-cmd/pr-upload"

RSpec.describe Homebrew::DevCmd::PrUpload do
  it_behaves_like "parseable arguments"
end
