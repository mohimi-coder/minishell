# frozen_string_literal: true

require "tapioca/dsl"
require "sorbet/tapioca/compilers/args"

RSpec.describe Tapioca::Compilers::Args do
  let(:compiler) { described_class.new(Tapioca::Dsl::Pipeline.new(requested_constants: []), RBI::Tree.new, Homebrew) }
  let(:list_parser) do
    require "cmd/list"
    Homebrew::Cmd::List.parser
  end

  # Good testing candidate because it has multiple for each of `switch`, `flag` and `comma_array` args:
  let(:update_python_resources_parser) do
    require "dev-cmd/update-python-resources"
    Homebrew::DevCmd::UpdatePythonResources.parser
  end

  describe "#args_table" do
    it "returns a mapping of list args to default values" do
      expect(compiler.args_table(list_parser).keys).to contain_exactly(
        :"1?", :cask?, :casks?, :d?, :debug?, :formula?, :formulae?,
        :full_name?, :h?, :help?, :installed_as_dependency?,
        :installed_on_request?, :l?, :multiple?, :pinned?,
        :q?, :quiet?, :r?, :t?, :v?, :verbose?, :versions?
      )
    end

    it "returns a mapping of update-python-resources args to default values" do
      expect(compiler.args_table(update_python_resources_parser)).to eq({
        d?:                        false,
        debug?:                    false,
        exclude_packages:          nil,
        extra_packages:            nil,
        h?:                        false,
        help?:                     false,
        ignore_non_pypi_packages?: false,
        install_dependencies?:     false,
        p?:                        false,
        package_name:              nil,
        print_only?:               false,
        q?:                        false,
        quiet?:                    false,
        s?:                        false,
        silent?:                   false,
        v?:                        false,
        verbose?:                  false,
        version:                   nil,
      })
    end
  end

  describe "#comma_arrays" do
    it "returns an empty list when there are no comma_array args" do
      expect(compiler.comma_arrays(list_parser)).to eq([])
    end

    it "returns the comma_array args when they exist" do
      expect(compiler.comma_arrays(update_python_resources_parser)).to eq([:extra_packages, :exclude_packages])
    end
  end

  describe "#get_return_type" do
    let(:comma_arrays) { compiler.comma_arrays(update_python_resources_parser) }

    it "returns the correct type for switches" do
      expect(compiler.get_return_type(:silent?, false, comma_arrays)).to eq("T::Boolean")
    end

    it "returns the correct type for flags" do
      expect(compiler.get_return_type(:package_name, nil, comma_arrays)).to eq("T.nilable(String)")
    end

    it "returns the correct type for comma_arrays" do
      expect(compiler.get_return_type(:extra_packages, nil, comma_arrays)).to eq("T.nilable(T::Array[String])")
    end
  end
end
