# typed: true
# frozen_string_literal: true

class Keg
  undef binary_executable_or_library_files

  def binary_executable_or_library_files
    elf_files
  end
end
