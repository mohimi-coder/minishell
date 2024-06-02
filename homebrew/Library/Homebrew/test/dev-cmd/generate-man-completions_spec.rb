# frozen_string_literal: true

require "cmd/shared_examples/args_parse"
require "dev-cmd/generate-man-completions"

RSpec.describe Homebrew::DevCmd::GenerateManCompletions do
  it_behaves_like "parseable arguments"
end
