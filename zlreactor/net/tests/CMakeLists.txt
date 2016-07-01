include_directories(${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "-std=c++0x -Wall -pthread")

link_directories(${CMAKE_INSTALL_PREFIX}/lib/ 
    ${PROJECT_SOURCE_DIR}/lib/
    ${PROJECT_BINARY_DIR}/lib/
    )

add_executable(ByteBuffer_test ByteBuffer_test.cpp)
target_link_libraries(ByteBuffer_test zlreactor)

add_executable(DomainResolver_test DomainResolver_test.cpp)
target_link_libraries(DomainResolver_test zlreactor)

add_executable(Eventfd_test Eventfd_test.cpp)
target_link_libraries(Eventfd_test zlreactor)

add_executable(EventLoopThreadPool_test EventLoopThreadPool_test.cpp)
target_link_libraries(EventLoopThreadPool_test zlreactor)

add_executable(EventLoop_test EventLoop_test.cpp)
target_link_libraries(EventLoop_test zlreactor)

add_executable(HttpRequestParser_test HttpRequestParser_test.cpp)
target_link_libraries(HttpRequestParser_test zlreactor)

add_executable(InetAddress_test InetAddress_test.cpp)
target_link_libraries(InetAddress_test zlreactor)

add_executable(Signalfd_test Signalfd_test.cpp)
target_link_libraries(Signalfd_test zlreactor)

add_executable(Socket_test Socket_test.cpp)
target_link_libraries(Socket_test zlreactor)

add_executable(TcpAcceptor_test TcpAcceptor_test.cpp)
target_link_libraries(TcpAcceptor_test zlreactor)

add_executable(TcpClient_test TcpClient_test.cpp)
target_link_libraries(TcpClient_test zlreactor)

add_executable(Timerfd_test Timerfd_test.cpp)
target_link_libraries(Timerfd_test zlreactor)

add_executable(Timer_test Timer_test.cpp)
target_link_libraries(Timer_test zlreactor)

add_executable(UriDecode_test UriDecode_test.cpp)
target_link_libraries(UriDecode_test zlreactor)

add_executable(UriQueryParams_test UriQueryParams_test.cpp)
target_link_libraries(UriQueryParams_test zlreactor)
