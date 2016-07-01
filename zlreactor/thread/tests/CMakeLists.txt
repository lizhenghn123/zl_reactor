include_directories(${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "-std=c++0x -Wall -pthread")

link_directories(${CMAKE_INSTALL_PREFIX}/lib/ 
    ${PROJECT_SOURCE_DIR}/lib/
    ${PROJECT_BINARY_DIR}/lib/
    )

add_executable(Atomic_test Atomic_test.cpp)
target_link_libraries(Atomic_test zlreactor)

add_executable(BlockingQueue_test BlockingQueue_test.cpp)
target_link_libraries(BlockingQueue_test zlreactor)

add_executable(CountDownLatch_test CountDownLatch_test.cpp)
target_link_libraries(CountDownLatch_test zlreactor)

add_executable(CyclicBarrier_test CyclicBarrier_test.cpp)
target_link_libraries(CyclicBarrier_test zlreactor)

add_executable(Mutex_test Mutex_test.cpp)
target_link_libraries(Mutex_test zlreactor)

add_executable(Thread_test Thread_test.cpp)
target_link_libraries(Thread_test zlreactor)

add_executable(ThreadGroup_test ThreadGroup_test.cpp)
target_link_libraries(ThreadGroup_test zlreactor)

add_executable(ThreadLocal_test ThreadLocal_test.cpp)
target_link_libraries(ThreadLocal_test zlreactor)
