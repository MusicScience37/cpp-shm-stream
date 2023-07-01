option(${UPPER_PROJECT_NAME}_TEST_BENCHMARKS
       "test benchmarks of ${FULL_PROJECT_NAME}" OFF)

set(${UPPER_PROJECT_NAME}_BENCH_DIR
    "${CMAKE_BINARY_DIR}/bench"
    CACHE PATH "directory to which benchmark results are written" FORCE)
file(MAKE_DIRECTORY ${${UPPER_PROJECT_NAME}_BENCH_DIR})

function(target_add_to_benchmark TARGET)
    target_link_libraries(${TARGET} PRIVATE ${PROJECT_NAME}
                                            cpp_stat_bench::stat_bench)
    target_include_directories(
        ${TARGET} PRIVATE ${${UPPER_PROJECT_NAME}_TEST_INCLUDE_DIR})

    if(${${UPPER_PROJECT_NAME}_TEST_BENCHMARKS})
        add_test(
            NAME ${TARGET}
            COMMAND ${TARGET} --plot "${TARGET}/" --json "${TARGET}/result.json"
            WORKING_DIRECTORY ${${UPPER_PROJECT_NAME}_BENCH_DIR})
    endif()
endfunction()

add_subdirectory(send_messages)
# TODO add_subdirectory(ping_pong)
