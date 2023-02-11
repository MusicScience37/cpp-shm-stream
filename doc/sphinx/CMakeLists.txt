if(DEFINED ENV{PLANTUML_JAR_PATH})
    set(DEFAULT_PLANTUML_JAR_PATH $ENV{PLANTUML_JAR_PATH})
else()
    set(DEFAULT_PLANTUML_JAR_PATH "")
endif()
set(PLANTUML_JAR_PATH
    "${DEFAULT_PLANTUML_JAR_PATH}"
    CACHE FILEPATH "path of plantuml.jar")

set(${UPPER_PROJECT_NAME}_SPHINX_OPTIONS
    -D "plantuml=java -jar ${PLANTUML_JAR_PATH}" -D
    "release=${PROJECT_VERSION}")

add_custom_target(
    ${PROJECT_NAME}_doc_sphinx_html ALL
    COMMAND
        ${POETRY_EXECUTABLE} run sphinx-build -b html -W
        ${CMAKE_CURRENT_SOURCE_DIR}/src ${${UPPER_PROJECT_NAME}_HTML_ROOT}
        ${${UPPER_PROJECT_NAME}_SPHINX_OPTIONS} -q
    COMMENT "Generate Sphinx html API documentation for ${FULL_PROJECT_NAME}"
    WORKING_DIRECTORY ${${UPPER_PROJECT_NAME}_SOURCE_DIR})

add_dependencies(${PROJECT_NAME}_doc ${PROJECT_NAME}_doc_sphinx_html)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/start_auto_build.sh.in
               ${CMAKE_CURRENT_BINARY_DIR}/start_auto_build.sh)
