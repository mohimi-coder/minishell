# frozen_string_literal: true

require "rubocops/bottle"

RSpec.describe RuboCop::Cop::FormulaAudit::BottleDigestIndentation do
  subject(:cop) { described_class.new }

  it "reports no offenses for `bottle :unneeded`" do
    expect_no_offenses(<<~RUBY)
      class Foo < Formula
        url "https://brew.sh/foo-1.0.tgz"

        bottle :unneeded
      end
    RUBY
  end

  it "reports no offenses for properly aligned digests in `bottle` blocks without cellars" do
    expect_no_offenses(<<~RUBY)
      class Foo < Formula
        url "https://brew.sh/foo-1.0.tgz"

        bottle do
          rebuild 4
          sha256 arm64_big_sur: "aaaaaaaa"
          sha256 big_sur:       "faceb00c"
          sha256 catalina:      "deadbeef"
        end
      end
    RUBY

    expect_no_offenses(<<~RUBY)
      class Foo < Formula
        url "https://brew.sh/foo-1.0.tgz"

        bottle do
          sha256 arm64_big_sur: "aaaaaaaa"
        end
      end
    RUBY
  end

  it "reports no offenses for properly aligned tags in `bottle` blocks with cellars" do
    expect_no_offenses(<<~RUBY)
      class Foo < Formula
        url "https://brew.sh/foo-1.0.tgz"

        bottle do
          rebuild 4
          sha256 cellar: :any,                arm64_big_sur: "aaaaaaaa"
          sha256 cellar: "/usr/local/Cellar", big_sur:       "faceb00c"
          sha256                              catalina:      "deadbeef"
        end
      end
    RUBY

    expect_no_offenses(<<~RUBY)
      class Foo < Formula
        url "https://brew.sh/foo-1.0.tgz"

        bottle do
          sha256 cellar: :any, arm64_big_sur: "aaaaaaaa"
        end
      end
    RUBY
  end

  it "reports and corrects misaligned digests in `bottle` block" do
    expect_offense(<<~RUBY)
      class Foo < Formula
        url "https://brew.sh/foo-1.0.tgz"

        bottle do
          rebuild 4
          sha256 arm64_big_sur: "aaaaaaaa"
          sha256 big_sur: "faceb00c"
                          ^^^^^^^^^^ FormulaAudit/BottleDigestIndentation: Align bottle digests
          sha256 catalina: "deadbeef"
                           ^^^^^^^^^^ FormulaAudit/BottleDigestIndentation: Align bottle digests
        end
      end
    RUBY

    expect_correction(<<~RUBY)
      class Foo < Formula
        url "https://brew.sh/foo-1.0.tgz"

        bottle do
          rebuild 4
          sha256 arm64_big_sur: "aaaaaaaa"
          sha256 big_sur:       "faceb00c"
          sha256 catalina:      "deadbeef"
        end
      end
    RUBY
  end

  it "reports and corrects misaligned digests in `bottle` block with cellars" do
    expect_offense(<<~RUBY)
      class Foo < Formula
        url "https://brew.sh/foo-1.0.tgz"

        bottle do
          rebuild 4
          sha256 cellar: :any,                arm64_big_sur: "aaaaaaaa"
          sha256 cellar: "/usr/local/Cellar", big_sur: "faceb00c"
                                                       ^^^^^^^^^^ FormulaAudit/BottleDigestIndentation: Align bottle digests
          sha256                              catalina: "deadbeef"
                                                        ^^^^^^^^^^ FormulaAudit/BottleDigestIndentation: Align bottle digests
        end
      end
    RUBY

    expect_correction(<<~RUBY)
      class Foo < Formula
        url "https://brew.sh/foo-1.0.tgz"

        bottle do
          rebuild 4
          sha256 cellar: :any,                arm64_big_sur: "aaaaaaaa"
          sha256 cellar: "/usr/local/Cellar", big_sur:       "faceb00c"
          sha256                              catalina:      "deadbeef"
        end
      end
    RUBY
  end
end
