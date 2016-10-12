include_directories(${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "-std=c++0x -Wall -pthread")

link_directories(${CMAKE_INSTALL_PREFIX}/lib/
        ${PROJECT_SOURCE_DIR}/lib/
        ${PROJECT_BINARY_DIR}/lib/
        )

add_executable(ConfigFile_test ConfigFile_test.cpp)
target_link_libraries(ConfigFile_test zlreactor)

add_executable(Daemon_test Daemon_test.cpp)
target_link_libraries(Daemon_test zlreactor)

add_executable(Exception_test Exception_test.cpp)
target_link_libraries(Exception_test zlreactor)

add_executable(AsyncLogger_test AsyncLogger_test.cpp)
target_link_libraries(AsyncLogger_test zlreactor)

add_executable(Logger_test Logger_test.cpp)
target_link_libraries(Logger_test zlreactor)

add_executable(MasterWorkerProcess_test MasterWorkerProcess_test.cpp)
target_link_libraries(MasterWorkerProcess_test zlreactor)

add_executable(ProcessUtil_test ProcessUtil_test.cpp)
target_link_libraries(ProcessUtil_test zlreactor)

add_executable(Singleton_test Singleton_test.cpp)
target_link_libraries(Singleton_test zlreactor)

add_executable(SmartAssert_test SmartAssert_test.cpp)
target_link_libraries(SmartAssert_test zlreactor)

add_executable(StringUtil_test StringUtil_test.cpp)
target_link_libraries(StringUtil_test zlreactor)

add_executable(Timestamp_test Timestamp_test.cpp)
target_link_libraries(Timestamp_test zlreactor)

