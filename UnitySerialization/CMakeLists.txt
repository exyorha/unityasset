add_library(UnitySerialization STATIC
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

    include/UnityAsset/SerializedAsset/AssetLinker.h
    SerializedAsset/AssetLinker.cpp

    include/UnityAsset/SerializedAsset/Downcastable.h
    SerializedAsset/Downcastable.cpp

    include/UnityAsset/SerializedAsset/ExternalAssetData.h
    SerializedAsset/ExternalAssetData.cpp

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

    include/UnityAsset/StreamedResourceManipulator.h
    StreamedResourceManipulator.cpp

    include/UnityAsset/UnityCompression.h
    UnityCompression.cpp

    include/UnityAsset/UnityTypeSerializer.h
    UnityTypeSerializer.cpp
)

if(NOT LibLZMA_FOUND)
    target_sources(UnitySerialization PRIVATE
        lzmadec/7zTypes.h
        lzmadec/LzmaDec.c
        lzmadec/LzmaDec.h
        lzmadec/Precomp.h
    )
endif()


configure_file(UnityAssetConfig.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/UnityAsset/UnityAssetConfig.h @ONLY)

if(WIN32)
    target_sources(UnitySerialization PRIVATE
        include/UnityAsset/WindowsError.h
        WindowsError.cpp

        include/UnityAsset/WindowsHandle.h
        WindowsHandle.cpp
    )
    target_compile_options(UnitySerialization PRIVATE -DWIN32_LEAN_AND_MEAN -D_VC_EXTRALEAN -DNOMINMAX -DUNICODE -D_UNICODE)
endif()

target_include_directories(UnitySerialization PUBLIC include ${CMAKE_CURRENT_BINARY_DIR}/include)

set_target_properties(UnitySerialization PROPERTIES
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN TRUE
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
    POSITION_INDEPENDENT_CODE TRUE
)
target_link_libraries(UnitySerialization PRIVATE ZLIB::ZLIB)
target_link_libraries(UnitySerialization PUBLIC lz4)

if(LibLZMA_FOUND)
    target_link_libraries(UnitySerialization PRIVATE LibLZMA::LibLZMA)
endif()

if(NOT MSVC)
    target_compile_options(UnitySerialization PRIVATE -Wall -W)
endif()

if(NOT WIN32)
    target_link_libraries(UnitySerialization PRIVATE tbb)
endif()

