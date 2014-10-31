#SUBDIRS= thread utility task_service log net statistic

CXXFLAGS = -Wall -g -O2 -fPIC

lib_LIBRARIES = libzlreactor.a

## 安装根目录

## 安装库目录

## 安装头文件目录

libzlreactor_a_LIBFLAGS =
libzlreactor_a_LIBADD =

INCLUDES                   = -I../
libzlreactor_a_CPPFLAGS = -std=c++0x
#libzlreactor_a_LDFLAGS = -lpthread	

libzlreactor_a_SOURCES = \
					 base/Base64.cpp \
					 base/ByteArray.cpp \
					 base/Date.cpp \
					 base/DateTimeUtil.cpp \
					 base/FileUtil.cpp \
					 base/MD5.cpp \
					 base/StringHash.cpp \
					 base/Timestamp.cpp \
					 base/ZLog.cpp \
					 thread/Thread.cpp \
					 thread/ThreadGroup.cpp \
					 thread/ThreadPool.cpp \
					 thread/Timer.cpp \
					 thread/TimerQueue.cpp \
					 net/NetUtil.cpp \
					 net/SocketUtil.cpp \
					 net/InetAddress.cpp \
					 net/Socket.cpp \
					 net/ActiveSocket.cpp \
					 net/PassiveSocket.cpp \
					 net/Poller.cpp \
					 net/EpollPoller.cpp \
					 net/EventLoop.cpp \
					 net/Channel.cpp \
					 net/Acceptor.cpp 
