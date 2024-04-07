require 'tempfile'
require 'pp'

require_relative 'class_database_types'

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

