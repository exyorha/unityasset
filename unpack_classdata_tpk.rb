require 'bindata'
require 'tempfile'
require 'pp'

class ClassDatabasePackageFileHeader < BinData::Record
    endian :little

    string :signature, length: 4, assert: "CLPK"
    uint8 :file_version, assert: 1
    uint8 :compression_type, assert: 130 # Single block LZMA
    uint32 :string_table_offset
    uint32 :string_table_len_uncompressed
    uint32 :string_table_len_compressed
    uint32 :file_block_size
    uint32 :file_count

    array :files, initial_length: :file_count do
        uint32 :file_offset
        uint32 :file_length
        string :file_name, length: 15, trim_padding: true
    end
end

class ClassDatabaseFileHeader < BinData::Record
    endian :little

    string :signature, length: 4, assert: "cldb"
    uint8 :file_version
    uint32 :flags, onlyif: proc { file_version >= 4 }
    uint8 :compression_type, onlyif: proc { file_version >= 2 }, assert: 0
    uint32 :compressed_size, onlyif: proc { file_version >= 2 }
    uint32 :uncompressed_size, onlyif: proc { file_version >= 2 }
    uint8 :unity_version_count
    array :unity_versions, initial_length: :unity_version_count do
        uint8 :version_length
        string :version, read_length: :version_length
    end
    uint32 :string_table_length
    uint32 :string_table_offset
end

def lzma_decompress(inf, compressed_length, uncompressed_length, outf)
    IO.popen([ "lzmadec" ], "wb", out: outf) do |stream|
        IO.copy_stream inf, stream, 5

        stream.write [ uncompressed_length ].pack("q<")

        IO.copy_stream inf, stream, compressed_length - 5
    end

    raise "lzmadec has failed" unless $?.success?

    nil
end

db = ClassDatabasePackageFileHeader.new
File.open("classdata.tpk", "rb") do |inf|
    db.read inf

    end_of_header = inf.pos

    Tempfile.create('classdata', nil, mode: File::RDWR | File::CREAT | File::TRUNC | File::BINARY) do |classfile|
        lzma_decompress inf, db.string_table_offset - end_of_header, db.file_block_size, classfile

        inf.seek db.string_table_offset

        Tempfile.create('stringtable', nil, mode: File::RDWR | File::CREAT | File::TRUNC | File::BINARY) do |stringtable|
            lzma_decompress inf, db.string_table_len_compressed, db.string_table_len_uncompressed, stringtable

            db.files.each do |file|
                File.open(file.file_name + ".cldb", "wb+") do |outf|
                   IO.copy_stream classfile, outf, file.file_length, file.file_offset

                   outf.rewind

                   header = ClassDatabaseFileHeader.new
                   header.read outf

                   if header.string_table_length == 0

                       stringtable.rewind
                       outf.seek file.file_length
                       IO.copy_stream stringtable, outf

                       header.string_table_offset = file.file_length
                       header.string_table_length = db.string_table_len_uncompressed
                       outf.rewind

                       header.write outf
                   end
                end
            end
        end
    end
end

