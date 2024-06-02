# frozen_string_literal: true

require "cmd/shared_examples/args_parse"
require "dev-cmd/update-maintainers"

RSpec.describe Homebrew::DevCmd::UpdateMaintainers do
  it_behaves_like "parseable arguments"
end
