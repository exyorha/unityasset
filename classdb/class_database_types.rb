require 'bindata'
require 'set'

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
    uint8 :unity_version_count, value: proc { unity_versions.size }
    array :unity_versions, initial_length: :unity_version_count do
        uint8 :version_length, value: proc { version.size }
        string :version, read_length: :version_length
    end
    uint32 :string_table_length
    uint32 :string_table_offset
end


class StringTableString < BinData::Primitive
    class << self
        attr_accessor :string_table
    end

    endian :little

    uint32 :offset

    def get
        @string ||= begin
            table = self.class.string_table

            endpos = table.index "\x00", offset
            raise "unterminated string" if endpos.nil?

            table[offset...endpos]
        end
    end

    def set(v)
        string = v.to_str
        terminated = "#{string}\x00"
        startpos = self.class.string_table.index(terminated)
        if startpos.nil?
            startpos = self.class.string_table.size
            self.class.string_table << terminated
        end

        self.offset = startpos

        @string = string

        string
    end


end

class ClassDatabaseClass < BinData::Record

    endian :little

    uint32 :class_id
    int32 :base_class_id
    string_table_string :class_name
    uint32 :field_count, value: proc { class_fields.size }
    array :class_fields, initial_length: :field_count do
        string_table_string :type_name
        string_table_string :field_name
        uint8 :depth
        uint8 :is_array
        int32 :field_size
        uint16 :version
        uint32 :flags2
    end
end

class ClassDatabaseBody < BinData::Record
    endian :little

    uint32 :class_count, value: proc { classes.size }
    array :classes, initial_length: :class_count, type: ClassDatabaseClass
end

class ClassDatabase

    class FieldIterator
        def initialize(list)
            @list = list
            @pos = 0
        end

        def at_end?
            @pos == @list.size
        end

        def get
            raise "field iterator is already at the end" if at_end?

            field = @list[@pos]
            @pos += 1

            field
        end

        def peek
            if @pos >= @list.size
                return nil
            end

            # Intended to return nil once we're past the end
            @list[@pos]
        end
    end

    class ClassDefinition
        attr_reader :class_id, :class_name, :parent_class
        attr_accessor :toplevel

        def initialize(class_id, class_name, parent_class)
            @class_id = class_id
            @class_name = class_name
            @parent_class = parent_class
            @toplevel = nil
        end

        def sanitized_class_name

            class_name = self.class_name

            if class_name == "void" || class_name == "int" || class_name == "bool" || class_name == "float"
                class_name = "BuiltinType_#{class_name}"
            end

            class_name
        end
    end

    class CompleteFieldDefinition
        attr_reader :type_name, :field_name, :is_array, :field_size, :version, :flags, :fields

        def initialize(iterator)
            field_info = iterator.get

            @type_name = field_info.type_name.to_str
            @field_name = field_info.field_name.to_str
            @is_array = field_info.is_array != 0
            @field_size = field_info.field_size
            @version = field_info.version
            @flags = field_info.flags2
            @fields = []

            while true
                next_field = iterator.peek
                break if next_field.nil? || next_field.depth <= field_info.depth

                field = CompleteFieldDefinition.new(iterator)
                @fields.push field

                # Workaround for the ClassInfo definition not being quite correct: string m_NamespaceName follows m_ClassName
                if type_name == "ClassInfo" && field.field_name == "m_ClassName"
                    newfield = CompleteFieldDefinition.allocate
                    newfield.instance_variable_set :@type_name, "string"
                    newfield.instance_variable_set :@field_name, "m_NamespaceName"
                    newfield.instance_variable_set :@is_array, false
                    newfield.instance_variable_set :@field_size, -1
                    newfield.instance_variable_set :@version, 0
                    newfield.instance_variable_set :@flags, 49152
                    newfield.instance_variable_set :@fields, []
                    @fields.push newfield
                end
            end
        end
    end

    class TypeRegistry

        attr_reader :forward_declares

        def initialize
            @types = {}
            @sorted_types = []
            @forward_declares = Set.new
            @muldef_type_count = 0
        end

        def types
            @sorted_types
        end

        def unify_type(complete_def)
            unification_name = complete_def.type_name
            template_arguments = []
            container_arguments = nil
            array_container = false

            if unification_name =~ /\APPtr<(.+)>\Z/
                unification_name = "PPtr"
                template_arguments = [ "UnityClasses::#{$1}" ]

                unless @types.include? $1
                    @forward_declares.add $1
                end

            elsif unification_name == 'pair'
                container_arguments = 2
            elsif unification_name == 'vector' || unification_name == 'staticvector' || unification_name == 'fixed_bitset' || unification_name == 'set'
                container_arguments = 1
                array_container = true
            elsif unification_name == 'map'
                container_arguments = 2
                array_container = true
            elsif unification_name == 'PackedBitVector' || unification_name == 'Google' ||
                    unification_name == "OffsetPtr"
                # Each instance of PackedBitVector is its own type. It would be nicer to make that an inner type to the outer type,
                # but that's a lot more work.
                @muldef_type_count += 1
                unification_name = unification_name + @muldef_type_count.to_s
            end

            unless container_arguments.nil?

                container_item_def = complete_def

                if array_container
                    if container_item_def.fields.size != 1
                        raise "expected Array as a sole child of a container object, got #{container_item_def.inspect}"
                    end

                    container_item_def = container_item_def.fields[0]

                    if !container_item_def.is_array || container_item_def.type_name != 'Array'
                        raise "unexpected configuration of an Array"
                    end

                    if container_item_def.fields.size != 2
                        raise "unexpected number of children in an Array"
                    end

                    length_item = container_item_def.fields[0]
                    if length_item.type_name != 'int' || length_item.field_name != 'size'
                        raise "unxpected configuration of the length field of an Array"
                    end

                    container_item_def = container_item_def.fields[1]
                end

                if array_container && container_arguments == 1
                    template_arguments = [ FieldDefinition.new(container_item_def, self) ]
                elsif container_item_def.fields.size != container_arguments
                    raise "unexpected number of container arguments for #{unification_name}: got #{container_item_def.fields.size}, expected #{container_arguments}"

                else
                    template_arguments = container_item_def.fields.map do |field|
                        FieldDefinition.new field, self
                    end
                end
            end

            existing = @types[unification_name]
            unless existing.nil?
                return existing, template_arguments
            end

            #puts "unifying a type: #{complete_def.type_name}"

            type = TypeDefinition.new complete_def, unification_name, template_arguments.size, self

            @types[unification_name] = type

            @sorted_types.push type

            [ type, template_arguments ]
        end
    end

    class FieldDefinition
        attr_reader :type
        attr_reader :template_arguments
        attr_accessor :field_name
        attr_reader :flags
        attr_reader :version
        attr_accessor :array_size

        def initialize(complete_definition, type_registry)
            @type, @template_arguments = type_registry.unify_type complete_definition
            @field_name = complete_definition.field_name
            @flags = complete_definition.flags
            @version = complete_definition.version
            @array_size = nil
        end
    end

    class TypeDefinition
        attr_reader :type_name, :fields, :template_argument_count

        def initialize(complete_definition, type_name, template_argument_count, type_registry)
            @type_name = type_name
            @template_argument_count = template_argument_count

            @fields = complete_definition.fields.map do |field|
                FieldDefinition.new(field, type_registry)
            end

            index = 0
            indices_to_remove = Set.new
            while index < @fields.size
                field = @fields[index]
                if field.field_name =~ /\A(.+)\[([\s0-9]+)\]\Z/
                    field_index = Integer($2)
                    if field_index != 0
                        raise "array doesn't start with the item 0"
                    end

                    field.field_name = $1
                    field.array_size = 1

                    index += 1

                    while index < @fields.size
                        next_field = @fields[index]
                        if next_field.field_name =~ /\A(.+)\[([\s0-9]+)\]\Z/ && $1 == field.field_name && Integer($2) == field.array_size
                            indices_to_remove.add index
                            index += 1
                            field.array_size += 1
                        else
                            break
                        end
                    end
                else
                    index += 1
                end
            end

            unless indices_to_remove.empty?
                @fields.reject!.with_index do |value, index|
                    indices_to_remove.include? index
                end
            end

            @fields.each do |field|
                field.field_name = clean_field_name field.field_name
                if field.field_name == field.type.type_name
                    field.field_name = "v" + field.type.type_name
                end
            end
        end

        private

        def clean_field_name(name)
            name = name.gsub /[^0-9_a-zA-Z]/, "_"

            name.sub!(/\A([0-9])/) { "v#{$1}" }

            name
        end
    end

    attr_reader :unity_versions
    attr_reader :classes
    attr_reader :types

    def initialize(inf)

        body = nil

        header = ClassDatabaseFileHeader.new
        header.read inf

        @unity_versions = header.unity_versions.map { |version| version.version.to_str }

        start_of_body = inf.pos
        inf.seek header.string_table_offset

        StringTableString.string_table = inf.read header.string_table_length

        inf.seek start_of_body

        body = ClassDatabaseBody.new
        body.read inf

        @classes = []
        @types = TypeRegistry.new

        @class_def_map = {}
        @emitted_class_defs = {}

        body.classes.each do |classdef|
            @class_def_map[classdef.class_id.to_i] = classdef
        end

        @class_def_map.each do |class_id, classdef|
            emit_class_def class_id.to_i
        end

        @emitted_class_defs = nil
        @class_def_map = nil

    end

    private

    def emit_class_def(class_id)
        existing = @emitted_class_defs[class_id]

        unless existing.nil?
            return existing
        end

        class_def = @class_def_map.fetch(class_id)

        parent_def = nil

        if class_def.base_class_id >= 0
            parent_def = emit_class_def class_def.base_class_id.to_i
        end

        class_definition = ClassDefinition.new(class_id, class_def.class_name.to_str, parent_def)

        @emitted_class_defs[class_id] = class_definition

        @classes.push class_definition

        iterator = FieldIterator.new(class_def.class_fields)
        until iterator.at_end?
            unless class_definition.toplevel.nil?
                raise "the class #{class_definition.class_name} already has a toplevel field"
            end

            class_definition.toplevel = FieldDefinition.new(CompleteFieldDefinition.new(iterator), @types)
        end

        class_definition
    end
end


class UnityTypeTreeNode < BinData::Record
    endian :little

    uint16 :m_Version
    uint8 :m_Level
    uint8 :m_TypeFlags
    uint32 :m_TypeStrOffset
    uint32 :m_NameStrOffset
    int32 :m_ByteSize
    int32 :m_Index
    int32 :m_MetaFlag
    uint64 :m_RefTypeHash
end

class UnityTypeTree < BinData::Record
    endian :little

    int32 :numberOfNodes
    int32 :stringBufferSize

    array :m_Nodes, type: UnityTypeTreeNode, initial_length: :numberOfNodes
    string :m_StringBuffer, read_length: :stringBufferSize

    int32 :numberOfDependencies
    array :m_TypeDependencies, type: :int32, initial_length: :numberOfDependencies

    def pooled_string(idx)
        source =
            if (idx & 0x80000000) == 0
                source = m_StringBuffer.value
            else
                source = DICTIONARY
            end

        idx &= 0x7FFFFFFF

        endpos = source.index("\x00".b, idx)
        if endpos.nil?
            raise "unterminated string"
        end

        source[idx...endpos]
    end

    private

    DICTIONARY = File.binread(File.expand_path("../unity_dictionary.bin", __FILE__))
end

class UnityTypeInfo < BinData::Record
    endian :little

    int32 :class_id
    uint8 :m_StrippedType, assert: 0
    int16 :m_ScriptTypeIndex
    string :m_ScriptID, length: 16, onlyif: proc { class_id == 114 } # only for MonoBehavior
    string :m_OldTypeHash, length: 16

    unity_type_tree :m_Type

end

def make_cldb_from_unity_types(out, types)
    string_table = "\x00"

    StringTableString.string_table = string_table

    header = ClassDatabaseFileHeader.new
    header.signature = "cldb"
    header.file_version = 0

    header.unity_versions.push({ version: "detached" })

    # Will be rewritten once the string pool location is finalized
    header.write out

    body = ClassDatabaseBody.new

    types.each do |type|
        body.classes.push({})
        classdef = body.classes[-1]
        classdef.class_id = type.class_id
        classdef.base_class_id = -1

        type.m_Type.m_Nodes.each do |node|
            classdef.class_fields.push({})
            field = classdef.class_fields[-1]

            field.type_name = type.m_Type.pooled_string(node.m_TypeStrOffset)
            field.field_name = type.m_Type.pooled_string(node.m_NameStrOffset)
            field.depth = node.m_Level
            field.is_array = node.m_TypeFlags
            field.field_size = node.m_ByteSize
            field.version = node.m_Version
            field.flags2 = node.m_MetaFlag

            if field.depth == 0
                classdef.class_name = field.type_name
            end
        end
    end

    body.write out

    header.string_table_offset = out.pos
    header.string_table_length = string_table.size

    out.write string_table

    out.seek 0
    header.write out

    out.seek 0, IO::SEEK_END

    nil

ensure
    StringTableString.string_table = nil

end
