# typed: true
# frozen_string_literal: true

require "completions"

# Helper functions for commands.
module Commands
  HOMEBREW_CMD_PATH = (HOMEBREW_LIBRARY_PATH/"cmd").freeze
  HOMEBREW_DEV_CMD_PATH = (HOMEBREW_LIBRARY_PATH/"dev-cmd").freeze
  # If you are going to change anything in below hash,
  # be sure to also update appropriate case statement in brew.sh
  HOMEBREW_INTERNAL_COMMAND_ALIASES = {
    "ls"           => "list",
    "homepage"     => "home",
    "-S"           => "search",
    "up"           => "update",
    "ln"           => "link",
    "instal"       => "install", # gem does the same
    "uninstal"     => "uninstall",
    "post_install" => "postinstall",
    "rm"           => "uninstall",
    "remove"       => "uninstall",
    "abv"          => "info",
    "dr"           => "doctor",
    "--repo"       => "--repository",
    "environment"  => "--env",
    "--config"     => "config",
    "-v"           => "--version",
    "lc"           => "livecheck",
    "tc"           => "typecheck",
  }.freeze
  # This pattern is used to split descriptions at full stops. We only consider a
  # dot as a full stop if it is either followed by a whitespace or at the end of
  # the description. In this way we can prevent cutting off a sentence in the
  # middle due to dots in URLs or paths.
  DESCRIPTION_SPLITTING_PATTERN = /\.(?>\s|$)/

  def self.valid_internal_cmd?(cmd)
    require?(HOMEBREW_CMD_PATH/cmd)
  end

  def self.valid_internal_dev_cmd?(cmd)
    require?(HOMEBREW_DEV_CMD_PATH/cmd)
  end

  def self.method_name(cmd)
    cmd.to_s
       .tr("-", "_")
       .downcase
       .to_sym
  end

  def self.args_method_name(cmd_path)
    cmd_path_basename = basename_without_extension(cmd_path)
    cmd_method_prefix = method_name(cmd_path_basename)
    :"#{cmd_method_prefix}_args"
  end

  def self.internal_cmd_path(cmd)
    [
      HOMEBREW_CMD_PATH/"#{cmd}.rb",
      HOMEBREW_CMD_PATH/"#{cmd}.sh",
    ].find(&:exist?)
  end

  def self.internal_dev_cmd_path(cmd)
    [
      HOMEBREW_DEV_CMD_PATH/"#{cmd}.rb",
      HOMEBREW_DEV_CMD_PATH/"#{cmd}.sh",
    ].find(&:exist?)
  end

  # Ruby commands which can be `require`d without being run.
  def self.external_ruby_v2_cmd_path(cmd)
    path = which("#{cmd}.rb", Tap.cmd_directories)
    path if require?(path)
  end

  # Ruby commands which are run by being `require`d.
  def self.external_ruby_cmd_path(cmd)
    which("brew-#{cmd}.rb", PATH.new(ENV.fetch("PATH")).append(Tap.cmd_directories))
  end

  def self.external_cmd_path(cmd)
    which("brew-#{cmd}", PATH.new(ENV.fetch("PATH")).append(Tap.cmd_directories))
  end

  def self.path(cmd)
    internal_cmd = HOMEBREW_INTERNAL_COMMAND_ALIASES.fetch(cmd, cmd)
    path ||= internal_cmd_path(internal_cmd)
    path ||= internal_dev_cmd_path(internal_cmd)
    path ||= external_ruby_v2_cmd_path(cmd)
    path ||= external_ruby_cmd_path(cmd)
    path ||= external_cmd_path(cmd)
    path
  end

  def self.commands(external: true, aliases: false)
    cmds = internal_commands
    cmds += internal_developer_commands
    cmds += external_commands if external
    cmds += internal_commands_aliases if aliases
    cmds.sort
  end

  def self.internal_commands_paths
    find_commands HOMEBREW_CMD_PATH
  end

  def self.internal_developer_commands_paths
    find_commands HOMEBREW_DEV_CMD_PATH
  end

  def self.official_external_commands_paths(quiet:)
    OFFICIAL_CMD_TAPS.flat_map do |tap_name, cmds|
      tap = Tap.fetch(tap_name)
      tap.install(quiet:) unless tap.installed?
      cmds.map(&method(:external_ruby_v2_cmd_path)).compact
    end
  end

  def self.internal_commands
    find_internal_commands(HOMEBREW_CMD_PATH).map(&:to_s)
  end

  def self.internal_developer_commands
    find_internal_commands(HOMEBREW_DEV_CMD_PATH).map(&:to_s)
  end

  def self.internal_commands_aliases
    HOMEBREW_INTERNAL_COMMAND_ALIASES.keys
  end

  def self.find_internal_commands(path)
    find_commands(path).map(&:basename)
                       .map { basename_without_extension(_1) }
  end

  def self.external_commands
    Tap.cmd_directories.flat_map do |path|
      find_commands(path).select(&:executable?)
                         .map { basename_without_extension(_1) }
                         .map { |p| p.to_s.delete_prefix("brew-").strip }
    end.map(&:to_s)
       .sort
  end

  def self.basename_without_extension(path)
    path.basename(path.extname)
  end

  def self.find_commands(path)
    Pathname.glob("#{path}/*")
            .select(&:file?)
            .sort
  end

  def self.rebuild_internal_commands_completion_list
    cmds = internal_commands + internal_developer_commands + internal_commands_aliases
    cmds.reject! { |cmd| Homebrew::Completions::COMPLETIONS_EXCLUSION_LIST.include? cmd }

    file = HOMEBREW_REPOSITORY/"completions/internal_commands_list.txt"
    file.atomic_write("#{cmds.sort.join("\n")}\n")
  end

  def self.rebuild_commands_completion_list
    # Ensure that the cache exists so we can build the commands list
    HOMEBREW_CACHE.mkpath

    cmds = commands(aliases: true) - Homebrew::Completions::COMPLETIONS_EXCLUSION_LIST

    all_commands_file = HOMEBREW_CACHE/"all_commands_list.txt"
    external_commands_file = HOMEBREW_CACHE/"external_commands_list.txt"
    all_commands_file.atomic_write("#{cmds.sort.join("\n")}\n")
    external_commands_file.atomic_write("#{external_commands.sort.join("\n")}\n")
  end

  sig { params(command: String).returns(T.nilable(T::Array[[String, String]])) }
  def self.command_options(command)
    return if command == "help"

    path = self.path(command)
    return if path.blank?

    if (cmd_parser = Homebrew::CLI::Parser.from_cmd_path(path))
      cmd_parser.processed_options.filter_map do |short, long, _, desc, hidden|
        next if hidden

        [long || short, desc]
      end
    else
      options = []
      comment_lines = path.read.lines.grep(/^#:/)
      return options if comment_lines.empty?

      # skip the comment's initial usage summary lines
      comment_lines.slice(2..-1).each do |line|
        match_data = / (?<option>-[-\w]+) +(?<desc>.*)$/.match(line)
        options << [match_data[:option], match_data[:desc]] if match_data
      end
      options
    end
  end

  def self.command_description(command, short: false)
    path = self.path(command)
    return if path.blank?

    if (cmd_parser = Homebrew::CLI::Parser.from_cmd_path(path))
      if short
        cmd_parser.description&.split(DESCRIPTION_SPLITTING_PATTERN)&.first
      else
        cmd_parser.description
      end
    else
      comment_lines = path.read.lines.grep(/^#:/)

      # skip the comment's initial usage summary lines
      comment_lines.slice(2..-1)&.each do |line|
        match_data = /^#:  (?<desc>\w.*+)$/.match(line)
        next unless match_data

        desc = match_data[:desc]
        return T.must(desc).split(DESCRIPTION_SPLITTING_PATTERN).first if short

        return desc
      end
    end
  end

  def self.named_args_type(command)
    path = self.path(command)
    return if path.blank?

    cmd_parser = Homebrew::CLI::Parser.from_cmd_path(path)
    return if cmd_parser.blank?

    Array(cmd_parser.named_args_type)
  end

  # Returns the conflicts of a given `option` for `command`.
  def self.option_conflicts(command, option)
    path = self.path(command)
    return if path.blank?

    cmd_parser = Homebrew::CLI::Parser.from_cmd_path(path)
    return if cmd_parser.blank?

    cmd_parser.conflicts.map do |set|
      set.map! { |s| s.tr "_", "-" }
      set - [option] if set.include? option
    end.flatten.compact
  end
end
