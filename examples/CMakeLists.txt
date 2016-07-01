include_directories(${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "-std=c++0x -Wall -pthread -lrt")

link_directories(${CMAKE_INSTALL_PREFIX}/lib/
        ${PROJECT_SOURCE_DIR}/lib/
        ${PROJECT_BINARY_DIR}/lib/
        )

add_executable(reader_writer reader_writer/reader_writer.cpp)
target_link_libraries(reader_writer zlreactor)

add_executable(simpleServers simpleServers/simpleServers.cpp)
target_link_libraries(simpleServers zlreactor)

add_executable(roundtriptime roundtriptime/RoundTripTime.cpp)
target_link_libraries(roundtriptime zlreactor)

add_executable(twisted_finger twisted_finger/twisted_finger.cpp)
target_link_libraries(twisted_finger zlreactor)

add_executable(test_httpserver test_httpserver/test_httpserver.cpp)
target_link_libraries(test_httpserver zlreactor)

add_executable(maxconnection maxconnection/MaxConnection.cpp)
target_link_libraries(maxconnection zlreactor)

add_executable(idleconnection idleconnection/idleconnection.cpp)
target_link_libraries(idleconnection zlreactor)

add_executable(echo_test echo/main.cpp echo/EchoServer.cpp echo/EchoClient.cpp)
target_link_libraries(echo_test zlreactor)

add_executable(thread_bench thread_bench/thread_bench.cpp)
target_link_libraries(thread_bench zlreactor)

add_executable(test_websocketserver test_websocketserver/test_websocketserver.cpp)
target_link_libraries(test_websocketserver zlreactor)
