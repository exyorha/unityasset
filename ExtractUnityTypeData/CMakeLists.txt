add_executable(ExtractUnityTypeData extract_types.cpp)
target_link_libraries(ExtractUnityTypeData PRIVATE UnitySerialization)

set_target_properties(ExtractUnityTypeData PROPERTIES
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN TRUE
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
    POSITION_INDEPENDENT_CODE TRUE
)
