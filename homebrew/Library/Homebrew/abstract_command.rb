# typed: strong
# frozen_string_literal: true

require "cli/parser"

module Homebrew
  # Subclass this to implement a `brew` command. This is preferred to declaring a named function in the `Homebrew`
  # module, because:
  #
  # - Each Command lives in an isolated namespace.
  # - Each Command implements a defined interface.
  # - `args` is available as an instance method and thus does not need to be passed as an argument to helper methods.
  # - Subclasses no longer need to reference `CLI::Parser` or parse args explicitly.
  #
  # To subclass, implement a `run` method and provide a `cmd_args` block to document the command and its allowed args.
  # To generate method signatures for command args, run `brew typecheck --update`.
  class AbstractCommand
    extend T::Helpers

    abstract!

    class << self
      sig { returns(T.nilable(T.class_of(CLI::Args))) }
      attr_reader :args_class

      sig { returns(String) }
      def command_name = Utils.underscore(T.must(name).split("::").fetch(-1)).tr("_", "-").delete_suffix("-cmd")

      # @return the AbstractCommand subclass associated with the brew CLI command name.
      sig { params(name: String).returns(T.nilable(T.class_of(AbstractCommand))) }
      def command(name) = subclasses.find { _1.command_name == name }

      sig { returns(T::Boolean) }
      def dev_cmd? = T.must(name).start_with?("Homebrew::DevCmd")

      sig { returns(CLI::Parser) }
      def parser = CLI::Parser.new(self, &@parser_block)

      private

      sig { params(block: T.proc.bind(CLI::Parser).void).void }
      def cmd_args(&block)
        @parser_block = T.let(block, T.nilable(T.proc.void))
        @args_class = T.let(const_set(:Args, Class.new(CLI::Args)), T.nilable(T.class_of(CLI::Args)))
      end
    end

    sig { returns(CLI::Args) }
    attr_reader :args

    sig { params(argv: T::Array[String]).void }
    def initialize(argv = ARGV.freeze)
      @args = T.let(self.class.parser.parse(argv), CLI::Args)
    end

    sig { abstract.void }
    def run; end
  end
end
