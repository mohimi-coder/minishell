# frozen_string_literal: true

require "cmd/doctor"
require "cmd/shared_examples/args_parse"

RSpec.describe Homebrew::Cmd::Doctor do
  it_behaves_like "parseable arguments"

  specify "check_integration_test", :integration_test do
    expect { brew "doctor", "check_integration_test" }
      .to output(/This is an integration test/).to_stderr
  end
end
