# typed: strict
# frozen_string_literal: true

module Utils
  module UID
    sig { type_parameters(:U).params(_block: T.proc.returns(T.type_parameter(:U))).returns(T.type_parameter(:U)) }
    def self.drop_euid(&_block)
      return yield if Process.euid == Process.uid

      original_euid = Process.euid
      begin
        Process::Sys.seteuid(Process.uid)
        yield
      ensure
        Process::Sys.seteuid(original_euid)
      end
    end
  end
end
