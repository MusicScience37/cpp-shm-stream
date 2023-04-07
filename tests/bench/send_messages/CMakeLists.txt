add_executable(
    bench_send_messages light_stream_test.cpp blocking_stream_test.cpp
                        udp_test.cpp main.cpp)
target_link_libraries(bench_send_messages PRIVATE asio::asio)
target_add_to_benchmark(bench_send_messages)
