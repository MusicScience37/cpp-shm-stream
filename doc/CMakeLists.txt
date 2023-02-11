message("Configure documentation of ${FULL_PROJECT_NAME}")

add_custom_target(${PROJECT_NAME}_doc)

set(${UPPER_PROJECT_NAME}_HTML_ROOT ${CMAKE_CURRENT_BINARY_DIR}/html)

add_subdirectory(doxygen)

option(${UPPER_PROJECT_NAME}_BUILD_DOC_SPHINX
       "build documentation of ${FULL_PROJECT_NAME} with Sphinx" ON)
if(${UPPER_PROJECT_NAME}_BUILD_DOC_SPHINX)
    add_subdirectory(sphinx)
endif()
