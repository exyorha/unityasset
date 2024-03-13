require 'bindata'

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
