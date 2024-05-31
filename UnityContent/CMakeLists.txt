set(UNITY_CONTENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "" FORCE)

function(unity_content_generate_library target_name unity_version)

    set(bindir "${CMAKE_CURRENT_BINARY_DIR}/generated_${target_name}")

    add_library(${target_name} STATIC
        ${UNITY_CONTENT_SOURCE_DIR}/include/UnityAsset/Environment/LinkedEnvironment.h
        ${UNITY_CONTENT_SOURCE_DIR}/Environment/LinkedEnvironment.cpp

        ${UNITY_CONTENT_SOURCE_DIR}/include/UnityAsset/Environment/LoadedSerializedAsset.h
        ${UNITY_CONTENT_SOURCE_DIR}/Environment/LoadedSerializedAsset.cpp

        ${UNITY_CONTENT_SOURCE_DIR}/include/UnityAsset/Environment/ObjectFactory.h
        ${UNITY_CONTENT_SOURCE_DIR}/Environment/ObjectFactory.cpp

        ${UNITY_CONTENT_SOURCE_DIR}/include/UnityAsset/Environment/ObjectPointer.h

        ${UNITY_CONTENT_SOURCE_DIR}/bcdec.cpp
        ${UNITY_CONTENT_SOURCE_DIR}/bcdec.h

        ${UNITY_CONTENT_SOURCE_DIR}/include/UnityAsset/ExtractedTextureImage.h
        ${UNITY_CONTENT_SOURCE_DIR}/ExtractedTextureImage.cpp

        ${UNITY_CONTENT_SOURCE_DIR}/include/UnityAsset/ShaderBlob.h
        ${UNITY_CONTENT_SOURCE_DIR}/ShaderBlob.cpp

        ${UNITY_CONTENT_SOURCE_DIR}/stb_image_write.h
        ${UNITY_CONTENT_SOURCE_DIR}/stb_image_write_config.cpp
        ${UNITY_CONTENT_SOURCE_DIR}/stb_image_write_config.h

        ${UNITY_CONTENT_SOURCE_DIR}/include/UnityAsset/UnityTextureTypes.h
        ${UNITY_CONTENT_SOURCE_DIR}/UnityTextureTypes.cpp

        ${bindir}/include/UnityAsset/UnityTypes.h
        ${bindir}/UnityTypes.cpp
    )

    target_include_directories(${target_name} PUBLIC ${UNITY_CONTENT_SOURCE_DIR}/include ${bindir}/include)
    target_link_libraries(${target_name} PUBLIC UnitySerialization)

    set_target_properties(${target_name} PROPERTIES
        C_VISIBILITY_PRESET hidden
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN TRUE
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED TRUE
        POSITION_INDEPENDENT_CODE TRUE
    )


    add_custom_command(
        OUTPUT
            ${bindir}/include/UnityAsset/UnityTypes.h
            ${bindir}/UnityTypes.cpp
        COMMAND cmake -E make_directory ${bindir}/classdb ${bindir}/include/UnityAsset
        COMMAND ${RUBY}
            ${UnityAsset_SOURCE_DIR}/classdb/unpack_classdata_tpk.rb
            ${UnityAsset_SOURCE_DIR}/classdb/classdata.tpk
            ${bindir}/classdb
        COMMAND ${RUBY}
            ${UnityAsset_SOURCE_DIR}/classdb/make_cldb_code.rb
            ${bindir}/classdb/U2019.4.9f1.cldb
            ${bindir}/include/UnityAsset/UnityTypes.h
            ${bindir}/UnityTypes.cpp
        DEPENDS
            ${UnityAsset_SOURCE_DIR}/classdb/classdata.tpk
            ${UnityAsset_SOURCE_DIR}/classdb/unpack_classdata_tpk.rb
            ${UnityAsset_SOURCE_DIR}/classdb/class_database_types.rb
            ${UnityAsset_SOURCE_DIR}/classdb/make_cldb_code.rb
        VERBATIM
    )

endfunction()

