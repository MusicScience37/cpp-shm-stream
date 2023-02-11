include(${CMAKE_CURRENT_SOURCE_DIR}/source_list.cmake)
add_executable(${PROJECT_NAME}_test_units ${SOURCE_FILES})
target_add_catch2(${PROJECT_NAME}_test_units)

add_executable(${PROJECT_NAME}_test_units_unity EXCLUDE_FROM_ALL
               unity_source.cpp)
target_link_libraries(${PROJECT_NAME}_test_units_unity
                      PRIVATE Catch2::Catch2WithMain ${PROJECT_NAME})
