# typed: true

# DO NOT EDIT MANUALLY
# This is an autogenerated file for dynamic methods in `Homebrew::DevCmd::TapNew`.
# Please instead update this file by running `bin/tapioca dsl Homebrew::DevCmd::TapNew`.


class Homebrew::DevCmd::TapNew
  sig { returns(Homebrew::DevCmd::TapNew::Args) }
  def args; end
end

class Homebrew::DevCmd::TapNew::Args < Homebrew::CLI::Args
  sig { returns(T.nilable(String)) }
  def branch; end

  sig { returns(T::Boolean) }
  def github_packages?; end

  sig { returns(T::Boolean) }
  def no_git?; end

  sig { returns(T.nilable(String)) }
  def pull_label; end
end
