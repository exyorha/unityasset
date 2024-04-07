require_relative 'class_database_types'

BUILTIN_TYPES = {
    "SInt8" => "int8_t",
    "UInt8" => "uint8_t",
    "char" => "char",
    "bool" => "bool",

    "SInt16" => "int16_t",
    "UInt16" => "uint16_t",

    "int" => "int32_t",
    "unsigned int" => "uint32_t",
    "float" => "float",
    "Type*" => "uint32_t", # no idea what this is, but apparently it's supposed to be 4 byte long

    "SInt64" => "int64_t",
    "UInt64" => "uint64_t",
    "double" => "double",

    "string" => "std::string",
    "pair" => "std::pair",
    "vector" => "std::vector",
    "staticvector" => "std::vector",
    "fixed_bitset" => "std::vector",
    "map" => "UnityMap",
    "set" => "UnitySet"
}

if ARGV.size != 2
    warn "Usage: make_cldb_code <INPUT CLDB FILE> <OUTPUT HEADER FILE>"
    exit 1
end

database =
    File.open(ARGV[0], "rb") do |inf|
        ClassDatabase.new inf
    end

header = File.open(ARGV[1], "wb")

header.write <<EOF
/*
 * This is an automatically-generated Unity serialization type definition
 * class targeting the following versions:
EOF

database.unity_versions.each do |version|
    header.puts " * - #{version}"
end

header.write <<EOF
 */

#ifndef UNITY_ASSET_UNITY_CLASSES_H
#define UNITY_ASSET_UNITY_CLASSES_H

#include <cstdint>
#include <utility>
#include <vector>
#include <string>

namespace UnityAsset {

    class UnityTypeSerializer;

}

namespace UnityAsset::UnityTypes {

  template<typename K, typename V> using UnityMap = std::vector<std::pair<K, V>>;
  template<typename T> using UnitySet = std::vector<T>;

EOF

def compose_type_ref(field)
    ref = BUILTIN_TYPES.fetch(field.type.type_name, field.type.type_name)

    if field.type.template_argument_count != 0

        arguments = field.template_arguments.map do |argument|
            if argument.kind_of? String
                argument
            else
                compose_type_ref argument
            end
        end

        ref = "#{ref}<#{arguments.join(", ")}>"
    end

    unless field.array_size.nil?
        ref = "std::array<#{ref}, #{field.array_size}>"
    end

    ref
end

database.types.forward_declares.each do |type_name|
    header.puts "  struct #{type_name};"
end

database.types.types.each do |type|
    next if BUILTIN_TYPES.include?(type.type_name) || type.type_name == "Array"

    if type.template_argument_count != 0
        header.write "  template<"

        (0...type.template_argument_count).each do |index|
            if index != 0
                header.write ", "
            end
            header.write "typename T#{index + 1}"
        end
        header.puts ">"
    end

    header.puts "  struct #{type.type_name} {"

    type.fields.each do |field|
        header.puts "    #{compose_type_ref field} #{field.field_name};"
    end

    header.write <<EOF

    void serialize(UnityTypeSerializer &serializer) {
EOF

    type.fields.each do |field|
        header.puts "      serializer.serialize(#{field.field_name}, #{field.flags});"
    end

    header.write <<EOF
    }
  };

EOF
end

header.write <<EOF
}

#endif

EOF
