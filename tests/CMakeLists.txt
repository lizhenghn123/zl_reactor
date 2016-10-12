include_directories(${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "-std=c++0x -Wall -pthread -lrt")

link_directories(${CMAKE_INSTALL_PREFIX}/lib/
        ${PROJECT_SOURCE_DIR}/lib/
        ${PROJECT_BINARY_DIR}/lib/
        )

add_executable(universal_timer universal_timer/TriggerTimerTest.cpp universal_timer/TriggerTimer.cpp)
target_link_libraries(universal_timer zlreactor)

add_executable(stress_client stress_client/stress_client.cpp)
target_link_libraries(stress_client zlreactor)

add_executable(timerqueue timerqueue/Timer_Test.cpp timerqueue/TimerQueue.cpp timerqueue/Timer.cpp)
target_link_libraries(timerqueue zlreactor)

add_executable(epoll_echoclient epoll_echoclient/EchoClientTest.cpp epoll_echoclient/EchoClient.cpp epoll_echoclient/EpollPoller.cpp)
target_link_libraries(epoll_echoclient)

add_executable(epoll_echoserver epoll_echoserver/EchoServerTest.cpp
    epoll_echoserver/EchoServer.cpp epoll_echoserver/EpollPoller.cpp)
target_link_libraries(epoll_echoserver)

add_executable(signal_handler signal_handler/signal_handler.cpp)
target_link_libraries(signal_handler)

add_executable(tcpselfconnect1 tcpselfconnect1/tcpselfconnect1.cpp)
target_link_libraries(tcpselfconnect1)

add_executable(tcpselfconnect2 tcpselfconnect2/tcpselfconnect2.cpp)
target_link_libraries(tcpselfconnect2)

add_executable(pipePairTest PipePairFactory/pipePairTest.cpp)
target_link_libraries(pipePairTest zlreactor)

add_executable(master_worker_process master_worker_process/master_worker_process.cpp)
target_link_libraries(master_worker_process zlreactor)

add_executable(socketAcceptThunderingHerd socketAcceptThunderingHerd/socketAcceptThunderingHerd.cpp)
target_link_libraries(socketAcceptThunderingHerd zlreactor)

add_executable(bind_tcp_udp bind_tcp_udp/bind_tcp_udp.cpp)
target_link_libraries(bind_tcp_udp)

add_executable(backtrace_test backtrace_test/backtrace_test.cpp)
target_link_libraries(backtrace_test)

add_executable(ProxyServerByFork proxy_server/ProxyServerByFork.cpp)
target_link_libraries(ProxyServerByFork)

add_executable(ProxyServerByEpoll proxy_server/ProxyServerByEpoll.cpp)
target_link_libraries(ProxyServerByEpoll)

add_executable(server_aio server_aio/server_aio.cpp)
target_link_libraries(server_aio)

add_executable(pingpong_server pingpong/pingpong_server.cpp)
target_link_libraries(pingpong_server zlreactor)

add_executable(pingpong_client pingpong/pingpong_client.cpp)
target_link_libraries(pingpong_client zlreactor)