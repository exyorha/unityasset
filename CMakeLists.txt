cmake_minimum_required(VERSION 3.20)
project(UnityAsset)

if(NOT TARGET lz4)
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(LZ4 REQUIRED IMPORTED_TARGET liblz4)
    add_library(lz4 ALIAS PkgConfig::LZ4)
endif()

find_package(LibLZMA)
find_package(ZLIB REQUIRED)

find_program(RUBY ruby REQUIRED)

add_library(UnityAsset STATIC
    include/UnityAsset/Environment/Downcastable.h
    Environment/Downcastable.cpp

    include/UnityAsset/Environment/ExternalAssetData.h
    Environment/ExternalAssetData.cpp

    include/UnityAsset/Environment/LinkedEnvironment.h
    Environment/LinkedEnvironment.cpp

    include/UnityAsset/Environment/LoadedSerializedAsset.h
    Environment/LoadedSerializedAsset.cpp

    include/UnityAsset/Environment/ObjectPointer.h

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

    bcdec.cpp
    bcdec.h

    include/UnityAsset/ExtractedTextureImage.h
    ExtractedTextureImage.cpp

    include/UnityAsset/FileDescriptor.h
    FileDescriptor.cpp

    include/UnityAsset/ShaderBlob.h
    ShaderBlob.cpp

    stb_image_write.h
    stb_image_write_config.cpp
    stb_image_write_config.h

    include/UnityAsset/StreamedResourceManipulator.h
    StreamedResourceManipulator.cpp

    include/UnityAsset/UnityCompression.h
    UnityCompression.cpp

    include/UnityAsset/UnityTextureTypes.h
    UnityTextureTypes.cpp

    include/UnityAsset/UnityTypeSerializer.h
    UnityTypeSerializer.cpp

    ${CMAKE_CURRENT_BINARY_DIR}/include/UnityAsset/UnityTypes.h
    ${CMAKE_CURRENT_BINARY_DIR}/UnityTypes.cpp
)

if(NOT LibLZMA_FOUND)
    target_sources(UnityAsset PRIVATE
        lzmadec/7zTypes.h
        lzmadec/LzmaDec.c
        lzmadec/LzmaDec.h
        lzmadec/Precomp.h
    )
endif()

configure_file(UnityAssetConfig.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/UnityAsset/UnityAssetConfig.h @ONLY)

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
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN TRUE
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
    POSITION_INDEPENDENT_CODE TRUE
)
target_link_libraries(UnityAsset PRIVATE lz4 ZLIB::ZLIB)

if(LibLZMA_FOUND)
    target_link_libraries(UnityAsset PRIVATE LibLZMA::LibLZMA)
endif()

if(NOT MSVC)
    target_compile_options(UnityAsset PRIVATE -Wall -W)
endif()

if(NOT WIN32)
    find_library(TBB tbb)
    if(TBB)
        target_link_libraries(UnityAsset PRIVATE ${TBB})
    endif()
endif()

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/include/UnityAsset/UnityTypes.h
        ${CMAKE_CURRENT_BINARY_DIR}/UnityTypes.cpp
    COMMAND cmake -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/classdb ${CMAKE_CURRENT_BINARY_DIR}/include/UnityAsset
    COMMAND ${RUBY}
        ${CMAKE_CURRENT_SOURCE_DIR}/classdb/unpack_classdata_tpk.rb
        ${CMAKE_CURRENT_SOURCE_DIR}/classdb/classdata.tpk
        ${CMAKE_CURRENT_BINARY_DIR}/classdb
    COMMAND ${RUBY}
        ${CMAKE_CURRENT_SOURCE_DIR}/classdb/make_cldb_code.rb
        ${CMAKE_CURRENT_BINARY_DIR}/classdb/U2019.4.9f1.cldb
        ${CMAKE_CURRENT_BINARY_DIR}/include/UnityAsset/UnityTypes.h
        ${CMAKE_CURRENT_BINARY_DIR}/UnityTypes.cpp
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/classdb/classdata.tpk
        ${CMAKE_CURRENT_SOURCE_DIR}/classdb/unpack_classdata_tpk.rb
        ${CMAKE_CURRENT_SOURCE_DIR}/classdb/class_database_types.rb
        ${CMAKE_CURRENT_SOURCE_DIR}/classdb/make_cldb_code.rb
    VERBATIM
)
