include(${CMAKE_CURRENT_SOURCE_DIR}/source_list.cmake)
add_library(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(
    ${PROJECT_NAME} PUBLIC ${${UPPER_PROJECT_NAME}_SOURCE_DIR}/include
                           ${${UPPER_PROJECT_NAME}_GENERATED_SRC_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_cpp_warnings)
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::atomic fmt::fmt)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES C_VISIBILITY_PRESET hidden
               CXX_VISIBILITY_PRESET hidden
               VISIBILITY_INLINES_HIDDEN 1
               DEBUG_POSTFIX d
               VERSION ${PROJECT_VERSION})

include(GenerateExportHeader)
generate_export_header(
    ${PROJECT_NAME}
    EXPORT_FILE_NAME
        "${${UPPER_PROJECT_NAME}_GENERATED_SRC_DIR}/shm_stream/details/shm_stream_export.h"
)
