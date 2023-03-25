add_executable(bench_ping_pong_server server/light_stream_server.cpp
                                      server/command_server.cpp server/main.cpp)
target_link_libraries(bench_ping_pong_server PRIVATE ${PROJECT_NAME}
                                                     rpclib::rpclib)
target_include_directories(bench_ping_pong_server
                           PRIVATE ${${UPPER_PROJECT_NAME}_TEST_INCLUDE_DIR})

add_executable(bench_ping_pong_client client/light_stream_test.cpp
                                      client/command_client.cpp client/main.cpp)
target_link_libraries(
    bench_ping_pong_client PRIVATE ${PROJECT_NAME} cpp_stat_bench::stat_bench
                                   rpclib::rpclib)
target_include_directories(bench_ping_pong_client
                           PRIVATE ${${UPPER_PROJECT_NAME}_TEST_INCLUDE_DIR})
