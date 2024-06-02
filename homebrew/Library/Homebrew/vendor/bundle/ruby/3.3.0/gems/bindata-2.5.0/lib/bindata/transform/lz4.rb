require 'extlz4'

module BinData
  module Transform
    # Transforms a LZ4 compressed data stream.
    #
    #     gem install extlz4
    class LZ4 < BinData::IO::Transform
      transform_changes_stream_length!

      def initialize(read_length)
        super()
        @length = read_length
      end

      def read(n)
        @read ||= ::LZ4::decode(chain_read(@length))
        @read.slice!(0...n)
      end

      def write(data)
        @write ||= create_empty_binary_string
        @write << data
      end

      def after_read_transform
        raise IOError, "didn't read all data" unless @read.empty?
      end

      def after_write_transform
        chain_write(::LZ4::encode(@write))
      end
    end
  end
end
