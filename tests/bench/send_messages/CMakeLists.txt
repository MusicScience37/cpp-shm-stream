add_executable(bench_send_messages no_wait_stream_test.cpp udp_test.cpp
                                   main.cpp)
target_link_libraries(
    bench_send_messages PRIVATE ${PROJECT_NAME} cpp_stat_bench::stat_bench
                                Threads::Threads)
target_include_directories(bench_send_messages
                           PRIVATE ${${UPPER_PROJECT_NAME}_TEST_INCLUDE_DIR})
