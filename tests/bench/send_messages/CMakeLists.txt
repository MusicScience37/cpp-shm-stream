add_executable(bench_send_messages no_wait_stream_test.cpp udp_test.cpp
                                   main.cpp)
target_add_to_benchmark(bench_send_messages)
