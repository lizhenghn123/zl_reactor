// ***********************************************************************
// Filename         : SignalHandler.h
// Author           : LIZHENG
// Created          : 2015-01-07
// Description      : 
//
// Last Modified By : LIZHENG
// Last Modified On : 2015-01-07
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_SIGNALHANDLER_H
#define ZL_SIGNALHANDLER_H
#include "Define.h"
#include "net/CallBacks.h"
#include <unordered_map>
#include <memory>
NAMESPACE_ZL_NET_START

class Channel;
class EventLoop;

class SignalHandler
{
public:
    explicit SignalHandler(EventLoop *loop);
    ~SignalHandler();

public:
	void setSigHandler(int sig, const SignalCallback& handler);
    int  registerAll();
	void removeSig(int sig);
	bool haveSignal(int sig);

private:
	void sigOnRead();

private:
	typedef std::unordered_map<int, SignalCallback>  SigHandlerMap;
	
	EventLoop        *loop_;
	int              signalFd_;
	Channel          *sigFdChannel_;
	SigHandlerMap    sigHanhlers_;
};

NAMESPACE_ZL_NET_END
#endif  /* ZL_SIGNALHANDLER_H */
