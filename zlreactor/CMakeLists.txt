######     build .so and .a     ######
MESSAGE(STATUS "This is BINARY dir " ${PROJECT_BINARY_DIR})
MESSAGE(STATUS "This is SOURCE dir " ${PROJECT_SOURCE_DIR})
MESSAGE(STATUS "This is PREFIX dir " ${CMAKE_INSTALL_PREFIX})

include_directories(${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "-std=c++0x -fPIC -Wall -pthread -lrt")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

aux_source_directory(base SRC_LIST)
aux_source_directory(thread SRC_LIST)
aux_source_directory(net SRC_LIST)
#aux_source_directory(net/poller SRC_LIST)
aux_source_directory(net/http SRC_LIST)
#aux_source_directory(net/websocket SRC_LIST)
#aux_source_directory(utility SRC_LIST)
if(WIN32)
set(net_poller_SRCS
    net/poller/Poller.cpp
	net/poller/SelectPoller.cpp
    )
else()
set(net_poller_SRCS
    net/poller/Poller.cpp
	net/poller/SelectPoller.cpp
    net/poller/PollPoller.cpp
    net/poller/EpollPoller.cpp
    )
endif()
set(net_websocket_SRCS
    net/websocket/WebSocket.cpp
    net/websocket/WebSocketServer.cpp
    )
set(utility_SRCS
    utility/Base64.cpp
	utility/Md5.cpp
    utility/SHA1.cpp
    utility/StringHash.cpp
    )

## set target of .so and .a
#ADD_LIBRARY(zlreactor SHARED ${SRC_LIST})
#SET_TARGET_PROPERTIES(zlreactor static PROPERTIES OUTPUT_NAME "zlreactor")
set(TARGET "zlreactor")
set(SHARED_T "libzlreactor_dynamic")
set(STATIC_T "libzlreactor_static")
add_library("${SHARED_T}" SHARED ${SRC_LIST} ${net_poller_SRCS} ${net_websocket_SRCS} ${utility_SRCS})
add_library("${STATIC_T}" STATIC ${SRC_LIST} ${net_poller_SRCS} ${net_websocket_SRCS} ${utility_SRCS})
set_target_properties("${SHARED_T}" PROPERTIES OUTPUT_NAME "${TARGET}")
set_target_properties("${STATIC_T}" PROPERTIES OUTPUT_NAME "${TARGET}")

## install .so and .a
#install(TARGETS libzlreactor_dynamic DESTINATION ${PROJECT_SOURCE_DIR}/lib)
#install(TARGETS libzlreactor_static DESTINATION ${PROJECT_SOURCE_DIR}lib)
install(TARGETS libzlreactor_dynamic DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(TARGETS libzlreactor_static DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

## install headers
#file(GLOB HEADERS "*.h")  # ok
set(HEADERS
    OsDefine.h
	Define.h
    )
set(base_HEADERS
    base/AsyncLogger.h
    base/ConfigFile.h
	base/Daemonize.h
	base/Demangle.h
    base/Date.h
    base/DateTimeUtil.h
	base/SmartAssert.h
	base/Exception.h
    base/File.h
	base/FileUtil.h
	base/GBKtoUTF8.h
	base/Logger.h
	base/LogFile.h
	base/MasterWorkerProcess.h
    base/MathUtil.h
	base/NonCopy.h
	base/ProcessUtil.h
	base/Singleton.h
	base/StopWatch.h
	base/StlUtil.h
    base/StringPiece.h
	base/StringUtil.h
	base/Timestamp.h
    )
set(thread_HEADERS
    thread/Atomic.h
	thread/BlockingQueue.h
    thread/BoundedBlockingQueue.h
    thread/BlockingQueueWorker.h
    thread/ConcurrentQueue.h
    thread/ConcurrentHashMap.h
	thread/Condition.h
	thread/CountDownLatch.h
	thread/CyclicBarrier.h
	thread/FastMutex.h
	thread/Mutex.h
	thread/RWMutex.h
	thread/Semaphore.h
    thread/Thread.h
	thread/ThreadGroup.h
	thread/ThreadLocal.h
	thread/ThreadPool.h
    )
set(net_HEADERS
    net/CallBacks.h
    net/Channel.h
    net/DomainResolver.h
    net/Eventfd.h
    net/EventLoop.h
    net/EventLoopThreadPool.h
    net/InetAddress.h
    net/NetUtil.h
    net/PipePairFactory.h
    net/Signalfd.h
    net/Socket.h
    net/SocketUtil.h
    net/TcpAcceptor.h
    net/TcpClient.h
    net/TcpConnection.h
    net/TcpConnector.h
    net/TcpServer.h
    net/Timer.h
    net/Timerfd.h
    net/TimerQueue.h
    )
set(net_http_HEADERS
    net/http/HttpContext.h
    net/http/HttpKeyValue.h
    net/http/HttpProtocol.h
    net/http/HttpRequest.h
    net/http/HttpResponse.h
    net/http/HttpServer.h
    net/http/UriQueryParams.h
    net/http/UriUtil.h
    )
set(net_websocket_HEADERS
    net/websocket/WebSocket.h
    net/websocket/WebSocketServer.h
    )
set(utility_HEADERS
    utility/Base64.h
	utility/Hex2String.h
	utility/Md5.h
	utility/StringHash.h
	utility/UUID.h
    )
install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/libzlreactor/)
install(FILES ${base_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/libzlreactor/base/)
install(FILES ${thread_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/libzlreactor/thread/)
install(FILES ${net_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/libzlreactor/net/)
install(FILES ${net_http_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/libzlreactor/net/http/)
install(FILES ${net_websocket_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/libzlreactor/net/websocket/)
install(FILES ${utility_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/libzlreactor/utility/)

add_subdirectory(base/tests)
add_subdirectory(thread/tests)
add_subdirectory(net/tests)
