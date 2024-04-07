require_relative 'class_database_types'

require 'rexml/document'

def cldb_to_xml(inf, outf)
    header = ClassDatabaseFileHeader.new
    header.read inf

    document = REXML::Document.new
    root = REXML::Element.new "unityClasses"
    document << root

    header.unity_versions.each do |version|
        version_node = REXML::Element.new 'unityVersion'
        version_node.add_attribute 'version', version.version
        root << version_node
    end

    start_of_body = inf.pos
    inf.seek header.string_table_offset

    StringTableString.string_table = inf.read header.string_table_length

    inf.seek start_of_body

    body = ClassDatabaseBody.new
    body.read inf

    sorted = body.classes.to_a
    sorted.sort_by! { |class_def| class_def.class_id }

    sorted.each do |class_def|

        class_node = REXML::Element.new 'class'
        class_node.add_attribute 'classId', class_def.class_id.to_s
        if class_def.base_class_id >= 0
            class_node.add_attribute 'baseClassId', class_def.base_class_id.to_s
        end

        class_node.add_attribute 'name', class_def.class_name.to_str

        field_stack = []

        class_def.class_fields.each do |field|

            field_node = REXML::Element.new 'field'
            field_node.add_attribute 'type', field.type_name.to_str
            field_node.add_attribute 'name', field.field_name.to_str

            if field.depth > field_stack.size
                raise "invalid field depth"
            end

            while field_stack.size > field.depth
                field_stack.pop
            end

            parent_node =
                if field_stack.empty?
                    class_node
                else
                    field_stack.last
                end

            parent_node << field_node

            field_stack.push field_node

            if field.is_array != 0
                field_node.add_attribute 'array', 'array'
            end

            if field.field_size >= 0
                field_node.add_attribute 'size', field.field_size.to_s
            end

            if field.flags2 != 0
                field_node.add_attribute 'flags', field.flags2.to_s
            end
        end

        root << class_node
    end

    document.write outf, 2
end

class UnityClassDatabase
    attr_reader :unity_versions

    def initialize(inf)

        @unity_versions = header.unity_versions.map { |v| v.version.to_str }


       # pp body
    end
end

if ARGV.size != 2
    warn "Usage: cldb_to_xml <INPUT CLDB FILE> <OUTPUT XML FILE>"
    exit 1
end

File.open(ARGV[0], "rb") do |inf|
    File.open(ARGV[1], "wb") do |outf|
        cldb_to_xml inf, outf
    end
end



#pp classdb

