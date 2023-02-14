include(${CMAKE_CURRENT_SOURCE_DIR}/source_list.cmake)
add_executable(${PROJECT_NAME}_test_integ ${SOURCE_FILES})
target_add_catch2(${PROJECT_NAME}_test_integ)

add_executable(${PROJECT_NAME}_test_integ_unity EXCLUDE_FROM_ALL
               unity_source.cpp)
target_include_directories(${PROJECT_NAME}_test_integ_unity
                           PRIVATE ${${UPPER_PROJECT_NAME}_TEST_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME}_test_integ_unity
                      PRIVATE Catch2::Catch2WithMain ${PROJECT_NAME})

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME}_test_integ PRIVATE Threads::Threads)
