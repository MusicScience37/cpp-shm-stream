add_executable(test_integ_no_wait_writer writer.cpp)
target_link_libraries(test_integ_no_wait_writer
                      PRIVATE ${PROJECT_NAME} ${PROJECT_NAME}_cpp_warnings)

add_executable(test_integ_no_wait_reader reader.cpp)
target_link_libraries(test_integ_no_wait_reader
                      PRIVATE ${PROJECT_NAME} ${PROJECT_NAME}_cpp_warnings)

if(POETRY_EXECUTABLE)
    add_test(
        NAME ${PROJECT_NAME}_test_integ_no_wait
        COMMAND ${POETRY_EXECUTABLE} run pytest --build ${CMAKE_BINARY_DIR}
                ${CMAKE_CURRENT_SOURCE_DIR}/test_no_wait.py
        WORKING_DIRECTORY ${${UPPER_PROJECT_NAME}_SOURCE_DIR})
endif()
