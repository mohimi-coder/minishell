# typed: true
# frozen_string_literal: true

module Stdenv
  sig {
    params(
      formula:         T.nilable(Formula),
      cc:              T.nilable(String),
      build_bottle:    T.nilable(T::Boolean),
      bottle_arch:     T.nilable(String),
      testing_formula: T::Boolean,
      debug_symbols:   T.nilable(T::Boolean),
    ).void
  }
  def setup_build_environment(formula: nil, cc: nil, build_bottle: false, bottle_arch: nil, testing_formula: false,
                              debug_symbols: false)
    generic_setup_build_environment(formula:, cc:, build_bottle:, bottle_arch:,
                                    testing_formula:, debug_symbols:)

    prepend_path "CPATH", HOMEBREW_PREFIX/"include"
    prepend_path "LIBRARY_PATH", HOMEBREW_PREFIX/"lib"
    prepend_path "LD_RUN_PATH", HOMEBREW_PREFIX/"lib"

    return unless @formula

    prepend_path "CPATH", @formula.include
    prepend_path "LIBRARY_PATH", @formula.lib
    prepend_path "LD_RUN_PATH", @formula.lib
  end

  def libxml2
    append "CPPFLAGS", "-I#{Formula["libxml2"].include/"libxml2"}"
  rescue FormulaUnavailableError
    nil
  end
end
