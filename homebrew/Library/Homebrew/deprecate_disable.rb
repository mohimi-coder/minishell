# typed: true
# frozen_string_literal: true

# Helper module for handling `disable!` and `deprecate!`.
module DeprecateDisable
  module_function

  FORMULA_DEPRECATE_DISABLE_REASONS = {
    does_not_build:      "does not build",
    no_license:          "has no license",
    repo_archived:       "has an archived upstream repository",
    repo_removed:        "has a removed upstream repository",
    unmaintained:        "is not maintained upstream",
    unsupported:         "is not supported upstream",
    deprecated_upstream: "is deprecated upstream",
    versioned_formula:   "is a versioned formula",
    checksum_mismatch:   "was built with an initially released source file that had " \
                         "a different checksum than the current one. " \
                         "Upstream's repository might have been compromised. " \
                         "We can re-package this once upstream has confirmed that they retagged their release",
  }.freeze

  CASK_DEPRECATE_DISABLE_REASONS = {
    discontinued:             "is discontinued upstream",
    moved_to_mas:             "is now exclusively distributed on the Mac App Store",
    no_longer_available:      "is no longer available upstream",
    no_longer_meets_criteria: "no longer meets the criteria for acceptable casks",
    unmaintained:             "is not maintained upstream",
    unsigned:                 "is unsigned or does not meet signature requirements",
  }.freeze

  def type(formula_or_cask)
    return :deprecated if formula_or_cask.deprecated?

    :disabled if formula_or_cask.disabled?
  end

  def message(formula_or_cask)
    return if type(formula_or_cask).blank?

    reason = if formula_or_cask.deprecated?
      formula_or_cask.deprecation_reason
    elsif formula_or_cask.disabled?
      formula_or_cask.disable_reason
    end

    reason = if formula_or_cask.is_a?(Formula) && FORMULA_DEPRECATE_DISABLE_REASONS.key?(reason)
      FORMULA_DEPRECATE_DISABLE_REASONS[reason]
    elsif formula_or_cask.is_a?(Cask::Cask) && CASK_DEPRECATE_DISABLE_REASONS.key?(reason)
      CASK_DEPRECATE_DISABLE_REASONS[reason]
    else
      reason
    end

    return "#{type(formula_or_cask)} because it #{reason}!" if reason.present?

    "#{type(formula_or_cask)}!"
  end

  def to_reason_string_or_symbol(string, type:)
    if (type == :formula && FORMULA_DEPRECATE_DISABLE_REASONS.key?(string&.to_sym)) ||
       (type == :cask && CASK_DEPRECATE_DISABLE_REASONS.key?(string&.to_sym))
      return string.to_sym
    end

    string
  end
end
