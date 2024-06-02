# typed: strict
# frozen_string_literal: true

require "abstract_command"

module Homebrew
  module DevCmd
    class Ruby < AbstractCommand
      cmd_args do
        usage_banner "`ruby` [<options>] (`-e` <text>|<file>)"
        description <<~EOS
          Run a Ruby instance with Homebrew's libraries loaded. For example,
          `brew ruby -e "puts :gcc.f.deps"` or `brew ruby script.rb`.

          Run e.g. `brew ruby -- --version` to pass arbitrary arguments to `ruby`.
        EOS
        flag "-r=",
             description: "Load a library using `require`."
        flag "-e=",
             description: "Execute the given text string as a script."

        named_args :file
      end

      sig { override.void }
      def run
        ruby_sys_args = []
        ruby_sys_args << "-r#{args.r}" if args.r
        ruby_sys_args << "-e #{args.e}" if args.e
        ruby_sys_args += args.named

        exec(*HOMEBREW_RUBY_EXEC_ARGS,
             "-I", $LOAD_PATH.join(File::PATH_SEPARATOR),
             "-rglobal", "-rdev-cmd/irb",
             *ruby_sys_args)
      end
    end
  end
end
