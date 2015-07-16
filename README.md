==========

学习C++，学习windows/linux编程，学习网络编程，学习reactor模型，自己实现一遍练手；

包含(以下实现是Windows/Linux平台通用的，美名其曰**跨平台**)

* **基本工具库**：
    * 日志工具（日志优先级、同步日志、异步日志、带色彩的屏幕输出）
    * 模板单例类
    * 时间戳类
    * 计时器类
    * 日期类
    * 智能断言（静态断言、动态断言）
    * ScopeGuard RAII类
    * STL辅助工具
    * 字符串辅助工具
    * File类（本地文件读取、内存数据映像文件操作）
    * 文件及目录操作工具
    * 异常类及命名重整还原实现
    * 进程属性工具（Linux特有）
    * 多进程master-worker实现（Linux特有）
* **线程库**：
    * 信号量 
    * 锁(互斥、读写、独占、旋转)
    * 原子计数器
    * 同步waiter
    * (固定大小、不限大小）同步队列（先进先出、先进后出、按优先级出入）
    * TLS
    * 线程池（组）
* **网络库**：
    * 网络相关工具 
    * socket api的跨平台包装
    * socket RAII类
    * socket address类
    * EventLoop、EventLoopThreadPool
    * IO MultiPlexing（select\poll\epoll\iocp）
    * TimerQueue
    * TcpAcceptor、TcpConnector、TcpConnection、TcpServer
    * eventfd、signalfd、timerfd（Linux特有）
    * simple httpserver
* **通用工具**
    * base64 编解码
    * md5
    * UUID
    * 字符串多种hash实现
 

感谢

* [c++11](http://www.cplusplus.com/reference/)

* [TinyThread++](http://tinythreadpp.bitsnbites.eu/)

* [Muduo](https://github.com/chenshuo/muduo)

* [libevent](http://libevent.org/)  

* [lighttpd](https://github.com/lizhenghn123/lighttpd1.4)

* [redis](https://github.com/antirez/redis)

* [Schmidt的三篇reactor文献]
    * [Reactor ： An Object Behavioral Pattern for Demultiplexing and Dispatching Handles for Synchronous Events](http://www.cs.wustl.edu/~schmidt/PDF/reactor-siemens.pdf)
    * [The Reactor : An Object-Oriented Wrapper for Event-Driven Port Monitoring and Service Demultiplexing](https://www.dre.vanderbilt.edu/~schmidt/PDF/Reactor1-93.pdf)
    * [The Reactor 2 : The Design and Implementation of the Reactor An Object-Oriented Framework for Event Demultiplexing](https://www.dre.vanderbilt.edu/~schmidt/PDF/Reactor2-93.pdf)
