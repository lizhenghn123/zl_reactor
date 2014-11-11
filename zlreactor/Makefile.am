#SUBDIRS= thread utility task_service log net statistic

CXXFLAGS = -Wall -g -O2 -fPIC

#plugindir = ../../lib
plugindir = ${exec_prefix}/lib

plugin_LTLIBRARIES = libzlreactor.la

## 安装根目录

## 安装库目录

## 安装头文件目录

libzlreactor_la_LIBFLAGS =
libzlreactor_la_LIBADD =

INCLUDES                   = -I../
libzlreactor_la_CPPFLAGS = -std=c++0x -fPIC -shared
#libzlreactor_a_LDFLAGS = -lpthread	

libzlreactor_la_SOURCES = \
					 base/Base64.cpp \
					 base/ByteArray.cpp \
					 base/Date.cpp \
					 base/DateTimeUtil.cpp \
					 base/FileUtil.cpp \
					 base/MD5.cpp \
					 base/StringHash.cpp \
					 base/Timestamp.cpp \
					 base/ZLog.cpp \
					 base/Demangle.cpp \
					 base/Exception.cpp \
					 thread/Thread.cpp \
					 thread/ThreadGroup.cpp \
					 thread/ThreadPool.cpp \
					 thread/Timer.cpp \
					 thread/TimerQueue.cpp \
					 net/NetUtil.cpp \
					 net/NetBuffer.cpp \
					 net/SocketUtil.cpp \
					 net/InetAddress.cpp \
					 net/Socket.cpp \
					 net/ActiveSocket.cpp \
					 net/PassiveSocket.cpp \
					 net/Poller.cpp \
					 net/EpollPoller.cpp \
					 net/EventLoop.cpp \
					 net/EventLoopThreadPool.cpp \
					 net/Channel.cpp \
					 net/Acceptor.cpp \
					 net/TcpConnection.cpp \
					 net/TcpServer.cpp \
					 net/TcpClient.cpp \
					 net/Connector.cpp 
