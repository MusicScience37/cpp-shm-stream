find_package(Doxygen REQUIRED dot)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
               ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

file(MAKE_DIRECTORY ${${UPPER_PROJECT_NAME}_HTML_ROOT}/api)

add_custom_target(
    ${PROJECT_NAME}_doc_doxygen ALL
    COMMAND doxygen ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    COMMENT "Generate Doxygen HTML API documentation for ${FULL_PROJECT_NAME}"
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${${UPPER_PROJECT_NAME}_SOURCE_DIR})

add_dependencies(${PROJECT_NAME}_doc ${PROJECT_NAME}_doc_doxygen)
