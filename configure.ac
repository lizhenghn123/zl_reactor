#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([zlreactor], [1.0], [lizhenghn@gmail.com])
AC_CONFIG_SRCDIR([zlreactor/Define.h])
AC_CONFIG_HEADERS([Config.h])
AM_INIT_AUTOMAKE(zlreactor,1.0)

AC_LINK_IFELSE([AC_LANG_SOURCE([int main() { return 0; }])], [some=thing], [some=other])
AC_PROG_LIBTOOL

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([pthread], [main])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h netinet/in.h stdint.h stdlib.h string.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_CHECK_FUNCS([bzero gethostbyname gettimeofday memset mkdir select socket strchr strerror])

dnl Check if we're a little-endian or a big-endian system, needed by hash code
AC_DEFUN([AC_C_ENDIAN],
[AC_CACHE_CHECK(for endianness, ac_cv_c_endian,
[
  AC_RUN_IFELSE(
    [AC_LANG_PROGRAM([], [dnl
        long val = 1;
        char *c = (char *) &val;
        exit(*c == 1);
    ])
  ],[
    ac_cv_c_endian=big
  ],[
    ac_cv_c_endian=little
  ])
])
if test $ac_cv_c_endian = big; then
  AC_DEFINE(ENDIAN_BIG, 1, [machine is bigendian])
fi
if test $ac_cv_c_endian = little; then
  AC_DEFINE(ENDIAN_LITTLE, 1, [machine is littleendian])
fi
])

AC_C_ENDIAN


AC_CONFIG_FILES([Makefile
                zlreactor/Makefile
                zlreactor/base/Makefile
                zlreactor/base/tests/Makefile
                zlreactor/thread/Makefile
                zlreactor/thread/tests/Makefile
                zlreactor/net/Makefile
                zlreactor/net/tests/Makefile
                examples/Makefile
                examples/reader_writer/Makefile
                examples/simpleServers/Makefile
                examples/maxconnection/Makefile
                examples/idleconnection/Makefile
                examples/roundtriptime/Makefile
                examples/twisted_finger/Makefile
                examples/test_httpserver/Makefile
                examples/echo/Makefile
                examples/thread_bench/Makefile
                tests/Makefile
                tests/stress_client/Makefile
                tests/universal_timer/Makefile
                tests/timerqueue/Makefile
                tests/epollclient/Makefile
                tests/epollserver/Makefile
                tests/signal_handler/Makefile
                tests/tcpselfconnect1/Makefile
                tests/tcpselfconnect2/Makefile
                tests/PipePairFactory/Makefile
                tests/master_worker_process/Makefile
                tests/socketAcceptThunderingHerd/Makefile
                tests/bind_tcp_udp/Makefile
                tests/backtrace_test/Makefile
               ])

AC_OUTPUT

cp Config.h zlreactor

AC_MSG_NOTICE
AC_MSG_NOTICE([Report:])
AC_MSG_NOTICE([ ______     ____  _____    _    ____ _____ ___  ____    ])
AC_MSG_NOTICE([|__  / |   |  _ \| ____|  / \  / ___|_   _/ _ \|  _ \   ])
AC_MSG_NOTICE([  / /| |   | |_) |  _|   / _ \| |     | || | | | |_) |  ])
AC_MSG_NOTICE([ / /_| |___|  _ <| |___ / ___ \ |___  | || |_| |  _ <   ])
AC_MSG_NOTICE([/____|_____|_| \_\_____/_/   \_\____| |_| \___/|_| \_\  ])
AC_MSG_NOTICE([  zlreactor : 1.0 ])
AC_MSG_NOTICE([  repository: https://github.com/lizhenghn123/zl_reactor.git ])
AC_MSG_NOTICE([  author    : lizhenghn@gmail.com ])

