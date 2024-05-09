require_relative 'class_database_types'

BUILTIN_TYPES = {
    "SInt8" => "int8_t",
    "UInt8" => "uint8_t",
    "char" => "uint8_t", # Probably UTF-8 byte
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
    "set" => "UnitySet",
    "TypelessData" => "UnityTypelessData"
}

if ARGV.size != 3
    warn "Usage: make_cldb_code <INPUT CLDB FILE> <OUTPUT HEADER FILE> <OUTPUT SORCE FILE>"
    exit 1
end

database =
    File.open(ARGV[0], "rb") do |inf|
        ClassDatabase.new inf
    end

header = File.open(ARGV[1], "wb")
source = File.open(ARGV[2], "wb")

[ header, source ].each do |file|
    file.write <<EOF
/*
 * This is an automatically-generated Unity serialization type definition
 * class targeting the following versions:
EOF

database.unity_versions.each do |version|
    file.puts " * - #{version}"
end

file.write <<EOF
 */

EOF
end

source.write <<EOF
#include <UnityAsset/UnityTypes.h>
#include <UnityAsset/UnityTypeSerializer.h>

namespace UnityAsset {
EOF

header.write <<EOF
#ifndef UNITY_ASSET_UNITY_CLASSES_H
#define UNITY_ASSET_UNITY_CLASSES_H

#include <cstdint>
#include <utility>
#include <vector>
#include <string>
#include <array>

#include <UnityAsset/Environment/ObjectPointer.h>
#include <UnityAsset/Environment/Downcastable.h>
#include <UnityAsset/Environment/ExternalAssetData.h>

namespace UnityAsset::UnityClasses {
EOF

database.classes.each do |classdef|

    header.puts "  struct #{classdef.sanitized_class_name};"

end


header.write <<EOF

}

namespace UnityAsset {
    class UnityTypeSerializer;
    class LoadedSerializedAsset;
}

namespace UnityAsset::UnityTypes {

  template<typename K, typename V> using UnityMap = std::vector<std::pair<K, V>>;
  template<typename T> using UnitySet = std::vector<T>;
  using UnityTypelessData = std::vector<uint8_t>;

EOF

def compose_type_ref(field)
    ref = BUILTIN_TYPES.fetch(field.type.type_name, "UnityTypes::#{field.type.type_name}")

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

def write_template(type, file)
    if type.template_argument_count != 0
        file.write "  template<"

        (0...type.template_argument_count).each do |index|
            if index != 0
                file.write ", "
            end
            file.write "typename T#{index + 1}"
        end
        file.puts ">"
    end
end

def template_args(type)
    if type.template_argument_count == 0
        ""
    else
        "<#{(1..type.template_argument_count).map { |index| "T#{index}" }.join(", ")}>"
    end
end

database.types.forward_declares.each do |type_name|
    header.puts "  struct #{type_name};"
end

database.types.types.each do |type|
    next if BUILTIN_TYPES.include?(type.type_name) || type.type_name == "Array"

    write_template type, header

    header.write "  struct #{type.type_name}";

    if type.type_name == "PPtr"
        header.write " final : public ObjectPointer<T1>"
    elsif type.type_name == "StreamingInfo"
        header.write " final : public ExternalAssetData"
    end

    header.puts " {";

    type.fields.each do |field|
        header.puts "    #{compose_type_ref field} #{field.field_name};"
    end


    header.puts "    void serialize(UnityTypeSerializer &serializer);"

    write_template type, source

    source.puts "    void UnityTypes::#{type.type_name}#{template_args type}::serialize(UnityTypeSerializer &serializer) {"

    type.fields.each do |field|
        source.puts "      serializer.serialize(#{field.field_name}, #{field.flags});"
    end

    if type.type_name == "PPtr"
        source.puts "      serializer.bindPointer<T1>(*this);"
    end

    if type.type_name == "StreamingInfo"
        source.puts "      serializer.bindExternalAssetData(*this);"
    end

    source.puts "    }";

    header.puts "};";
end

header.write <<EOF
}

namespace UnityAsset::UnityClasses {
EOF

concrete_implementations = Hash.new { |h, k| h[k] = [] }

database.classes.each do |classdef|
    chain = classdef
    until chain.nil?
        concrete_implementations[chain].push classdef
        chain = chain.parent_class
    end
end

database.classes.each do |classdef|
    ref = nil

    contents = classdef.toplevel
    unless contents.nil?
        ref = compose_type_ref contents
    end

    name = classdef.sanitized_class_name

    header.write "struct #{name}"

    parent_classes = []

    if concrete_implementations[classdef].size <= 1
        header.write " final"
    end

    if classdef.parent_class.nil?
        parent_classes.push "public Downcastable"
    else
        parent_classes.push "public UnityClasses::#{classdef.parent_class.sanitized_class_name}"
    end

    unless ref.nil?
        parent_classes.push "public virtual #{ref}"
    end

    unless parent_classes.empty?
        header.write ": #{parent_classes.join ", "}"
    end

    header.write <<EOF
{
    static constexpr uint32_t ClassID = #{classdef.class_id};

    #{name}();
    ~#{name}() override;

    int32_t classId() const override;

    bool canBeCastTo(int32_t classId) const override;
EOF

    source.write <<EOF
UnityClasses::#{name}::#{name}() = default;

UnityClasses::#{name}::~#{name}() = default;

int32_t UnityClasses::#{name}::classId() const {
    return ClassID;
}

bool UnityClasses::#{name}::canBeCastTo(int32_t classId) const {
    return classId == ClassID
EOF

    unless classdef.parent_class.nil?
        source.write " || UnityClasses::#{classdef.parent_class.class_name}::canBeCastTo(classId)"
    end

    source.puts ";"
    source.puts "}"

    if !ref.nil? || classdef.parent_class.nil?

        header.puts "  void deserialize(const Stream &stream);"
        source.puts "void UnityClasses::#{name}::deserialize(const Stream &stream) {"
        if ref.nil?
            source.puts "    (void)stream;"
        else
            source.puts "    UnityTypeSerializer::deserializeObject(stream, #{contents.flags}, static_cast<#{ref} &>(*this));"
        end
        source.puts "  }"

        header.puts "  void serialize(Stream &stream);"
        source.puts "  void UnityClasses::#{name}::serialize(Stream &stream) {"
        if ref.nil?
            source.puts "    (void)stream;"
        else
            source.puts "    UnityTypeSerializer::serializeObject(static_cast<#{ref} &>(*this), #{contents.flags}, stream);"
        end
        source.puts "  }"


        header.puts "void link(LoadedSerializedAsset *asset) override;"
        source.puts "void UnityClasses::#{name}::link(LoadedSerializedAsset *asset) {"
        if ref.nil?
            source.puts "    (void)asset;"
        else
            source.puts "    UnityTypeSerializer::linkObject(asset, static_cast<#{ref} &>(*this), #{contents.flags});"
        end
        source.puts "}"
    end

    header.puts "};"
end

header.write <<EOF
}

#endif

EOF

source.puts "}"

