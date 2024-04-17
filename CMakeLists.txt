cmake_minimum_required(VERSION 3.20)
project(UnityAsset)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LZ4 REQUIRED IMPORTED_TARGET liblz4)

find_package(ZLIB REQUIRED)

find_program(RUBY ruby REQUIRED)

add_library(UnityAsset STATIC
    include/UnityAsset/FileContainer/AssetBundle/AssetBundleBlock.h
    FileContainer/AssetBundle/AssetBundleBlock.cpp

    include/UnityAsset/FileContainer/AssetBundle/AssetBundleDirectory.h
    FileContainer/AssetBundle/AssetBundleDirectory.cpp

    include/UnityAsset/FileContainer/AssetBundle/AssetBundleDirectoryEntry.h
    FileContainer/AssetBundle/AssetBundleDirectoryEntry.cpp

    include/UnityAsset/FileContainer/AssetBundle/AssetBundleEntry.h
    FileContainer/AssetBundle/AssetBundleEntry.cpp

    include/UnityAsset/FileContainer/AssetBundle/AssetBundleFile.h
    FileContainer/AssetBundle/AssetBundleFile.cpp

    include/UnityAsset/SerializedAsset/FileIdentifier.h
    SerializedAsset/FileIdentifier.cpp

    include/UnityAsset/SerializedAsset/LocalSerializedObjectIdentifier.h
    SerializedAsset/LocalSerializedObjectIdentifier.cpp

    include/UnityAsset/SerializedAsset/SerializedAssetFile.h
    SerializedAsset/SerializedAssetFile.cpp

    include/UnityAsset/SerializedAsset/SerializedObject.h
    SerializedAsset/SerializedObject.cpp

    include/UnityAsset/SerializedAsset/SerializedType.h
    SerializedAsset/SerializedType.cpp

    include/UnityAsset/SerializedAsset/TypeTree.h
    SerializedAsset/TypeTree.cpp

    include/UnityAsset/SerializedAsset/TypeTreeNode.h
    SerializedAsset/TypeTreeNode.cpp

    include/UnityAsset/Streams/FileInputOutput.h
    Streams/FileInputOutput.cpp

    include/UnityAsset/Streams/InMemoryStreamBackingBuffer.h
    Streams/InMemoryStreamBackingBuffer.cpp

    include/UnityAsset/Streams/MappedFileStreamBackingBuffer.h
    Streams/MappedFileStreamBackingBuffer.cpp

    include/UnityAsset/Streams/Stream.h
    Streams/Stream.cpp

    include/UnityAsset/Streams/StreamBackingBuffer.h
    Streams/StreamBackingBuffer.cpp

    include/UnityAsset/FileDescriptor.h
    FileDescriptor.cpp

    include/UnityAsset/ShaderBlob.h
    ShaderBlob.cpp

    include/UnityAsset/StreamedResourceManipulator.h
    StreamedResourceManipulator.cpp

    include/UnityAsset/UnityCompression.h
    UnityCompression.cpp

    include/UnityAsset/UnityTextureTypes.h
    UnityTextureTypes.cpp

    include/UnityAsset/UnityTypeSerializer.h
    UnityTypeSerializer.cpp

    ${CMAKE_CURRENT_BINARY_DIR}/include/UnityAsset/UnityTypes.h
)

if(WIN32)
    target_sources(UnityAsset PRIVATE
        include/UnityAsset/WindowsError.h
        WindowsError.cpp

        include/UnityAsset/WindowsHandle.h
        WindowsHandle.cpp
    )
    target_compile_options(UnityAsset PRIVATE -DWIN32_LEAN_AND_MEAN -D_VC_EXTRALEAN -DNOMINMAX -DUNICODE -D_UNICODE)
endif()

target_include_directories(UnityAsset PUBLIC include ${CMAKE_CURRENT_BINARY_DIR}/include)

set_target_properties(UnityAsset PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)
target_link_libraries(UnityAsset PRIVATE PkgConfig::LZ4 ZLIB::ZLIB LibLZMA::LibLZMA)

if(NOT MSVC)
    target_compile_options(UnityAsset PRIVATE -Wall -W)
    target_link_libraries(UnityAsset PRIVATE tbb)
endif()

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/include/UnityAsset/UnityTypes.h
    COMMAND cmake -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/classdb ${CMAKE_CURRENT_BINARY_DIR}/include/UnityAsset
    COMMAND ${RUBY}
        ${CMAKE_CURRENT_SOURCE_DIR}/classdb/unpack_classdata_tpk.rb
        ${CMAKE_CURRENT_SOURCE_DIR}/classdb/classdata.tpk
        ${CMAKE_CURRENT_BINARY_DIR}/classdb
    COMMAND ${RUBY}
        ${CMAKE_CURRENT_SOURCE_DIR}/classdb/make_cldb_code.rb
        ${CMAKE_CURRENT_BINARY_DIR}/classdb/U2019.4.9f1.cldb
        ${CMAKE_CURRENT_BINARY_DIR}/include/UnityAsset/UnityTypes.h
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/classdb/classdata.tpk
        ${CMAKE_CURRENT_SOURCE_DIR}/classdb/unpack_classdata_tpk.rb
        ${CMAKE_CURRENT_SOURCE_DIR}/classdb/class_database_types.rb
        ${CMAKE_CURRENT_SOURCE_DIR}/classdb/make_cldb_code.rb
    VERBATIM
)
