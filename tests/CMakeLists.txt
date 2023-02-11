option(${UPPER_PROJECT_NAME}_WRITE_JUNIT
       "write JUnit XML files in tests of ${FULL_PROJECT_NAME}" OFF)
set(${UPPER_PROJECT_NAME}_JUNIT_DIR
    "${CMAKE_BINARY_DIR}/junit"
    CACHE PATH "directory to which JUnit XML files are written" FORCE)
if(${UPPER_PROJECT_NAME}_WRITE_JUNIT)
    file(MAKE_DIRECTORY ${${UPPER_PROJECT_NAME}_JUNIT_DIR})
endif()

set(${UPPER_PROJECT_NAME}_TEMP_TEST_DIR
    "${CMAKE_BINARY_DIR}/temp_test"
    CACHE PATH "directory used in test" FORCE)
file(MAKE_DIRECTORY ${${UPPER_PROJECT_NAME}_TEMP_TEST_DIR})

set(${UPPER_PROJECT_NAME}_TEST_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

macro(target_add_catch2 TARGET)
    target_link_libraries(${TARGET} PRIVATE Catch2::Catch2WithMain
                                            ${PROJECT_NAME})
    target_include_directories(
        ${TARGET} PRIVATE ${${UPPER_PROJECT_NAME}_TEST_INCLUDE_DIR})

    if(${UPPER_PROJECT_NAME}_WRITE_JUNIT)
        set(CATCH2_REPORT_OPTIONS
            -r junit -o ${${UPPER_PROJECT_NAME}_JUNIT_DIR}/${TARGET}.xml)
    else()
        set(CATCH2_REPORT_OPTIONS -r console)
    endif()

    add_test(
        NAME ${TARGET}
        COMMAND ${TARGET} ${CATCH2_REPORT_OPTIONS}
        WORKING_DIRECTORY ${${UPPER_PROJECT_NAME}_TEMP_TEST_DIR})
endmacro()

option(${UPPER_PROJECT_NAME}_ENABLE_UNIT_TESTS
       "enable unit tests of ${FULL_PROJECT_NAME}" ON)
if(${UPPER_PROJECT_NAME}_ENABLE_UNIT_TESTS)
    add_subdirectory(units)
endif()

option(${UPPER_PROJECT_NAME}_ENABLE_INTEG_TESTS
       "enable integration tests of ${FULL_PROJECT_NAME}" ON)
if(${UPPER_PROJECT_NAME}_ENABLE_INTEG_TESTS)
    # add_subdirectory(integ)
endif()

option(${UPPER_PROJECT_NAME}_ENABLE_BENCH
       "enable benchmarks of ${FULL_PROJECT_NAME}" OFF)
if(${UPPER_PROJECT_NAME}_ENABLE_BENCH)
    # add_subdirectory(bench)
endif()
