cmake_minimum_required(VERSION 3.12)

set(SHORT_PROJECT_NAME "shm_stream")
set(FULL_PROJECT_NAME "cpp-shm-stream")
string(TOUPPER ${SHORT_PROJECT_NAME} UPPER_PROJECT_NAME)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(ParseVersion)
parse_version(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${SHORT_PROJECT_NAME}/version.h
    ${UPPER_PROJECT_NAME})

project(
    ${SHORT_PROJECT_NAME}
    VERSION ${${UPPER_PROJECT_NAME}_VERSION}
    DESCRIPTION "A C++ library of streams on shared memory."
    LANGUAGES C CXX)
message(STATUS "${FULL_PROJECT_NAME} version ${PROJECT_VERSION}")
message(STATUS "build type: ${CMAKE_BUILD_TYPE}")

string(TOUPPER ${PROJECT_NAME} UPPER_PROJECT_NAME)

set(BUILDNAME
    "${CMAKE_HOST_SYSTEM_NAME}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}-${CMAKE_BUILD_TYPE}"
)
include(CTest)

option(${UPPER_PROJECT_NAME}_BUILD_DOC
       "build documentation of ${FULL_PROJECT_NAME}" OFF)
option(${UPPER_PROJECT_NAME}_TESTING "enable tests of ${FULL_PROJECT_NAME}" OFF)

set(CMAKE_CXX_STANDARD
    "14"
    CACHE STRING "C++ standard")

set(BUILD_SHARED_LIBS
    ON
    CACHE BOOL "Build shared libraries.")
set(CMAKE_POSITION_INDEPENDENT_CODE
    ON
    CACHE BOOL "Build position independent code.")

include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/lib>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/lib>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin>)

find_package(Boost REQUIRED COMPONENTS atomic)
find_package(fmt REQUIRED)
if(${UPPER_PROJECT_NAME}_TESTING)
    find_package(Catch2 REQUIRED)
    find_package(cpp-stat-bench REQUIRED)
    find_package(Threads REQUIRED)
    find_package(httplib REQUIRED)
    find_package(asio REQUIRED)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS
    ON
    CACHE BOOL "export compile commands" FORCE)

include(CppWarningFlags)
include(ConfigureSanitizer)
include(ConfigureCcache)
include(poetry)

set(${UPPER_PROJECT_NAME}_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(${UPPER_PROJECT_NAME}_GENERATED_SRC_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)

add_subdirectory(src)

if(${UPPER_PROJECT_NAME}_BUILD_DOC)
    add_subdirectory(doc)
endif()

if(${UPPER_PROJECT_NAME}_TESTING)
    add_subdirectory(tests)
endif()
