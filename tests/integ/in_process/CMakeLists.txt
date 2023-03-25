add_executable(${PROJECT_NAME}_test_integ_in_process light_stream_test.cpp
                                                     blocking_stream_test.cpp)
target_add_catch2(${PROJECT_NAME}_test_integ_in_process)
target_link_libraries(${PROJECT_NAME}_test_integ_in_process
                      PRIVATE Threads::Threads)
