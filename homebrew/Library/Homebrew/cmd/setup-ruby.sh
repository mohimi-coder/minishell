#:  * `setup-ruby [command]`
#:
#:  Installs and configures Homebrew's Ruby.
#:  If `command` is passed, it will only run Bundler if necessary for that
#:  command.
#:

# HOMEBREW_LIBRARY is set by brew.sh
# HOMEBREW_BREW_FILE is set by extend/ENV/super.rb
# shellcheck disable=SC2154
homebrew-setup-ruby() {
  source "${HOMEBREW_LIBRARY}/Homebrew/utils/ruby.sh"
  setup-ruby-path

  if [[ -z "${HOMEBREW_DEVELOPER}" ]]
  then
    return
  fi

  # Avoid running Bundler if the command doesn't need it.
  local command="$1"
  if [[ -n "${command}" ]]
  then
    source "${HOMEBREW_LIBRARY}/Homebrew/command_path.sh"

    command_path="$(homebrew-command-path "${command}")"
    if [[ -n "${command_path}" && "${command_path}" != *"/dev-cmd/"* ]]
    then
      return
    fi

    if ! grep -q "Homebrew.install_bundler_gems\!" "${command_path}"
    then
      return
    fi
  fi

  setup-gem-home-bundle-gemfile

  if ! bundle check &>/dev/null
  then
    "${HOMEBREW_BREW_FILE}" install-bundler-gems
  fi
}
