// ***********************************************************************
// Filename         : Signalfd.h
// Author           : LIZHENG
// Created          : 2015-01-07
// Description      : signalfd need linux kernel > 2.6.25
//
// Last Modified By : LIZHENG
// Last Modified On : 2015-05-05
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_SIGNALFD_H
#define ZL_SIGNALFD_H
#include "Define.h"
#include "net/CallBacks.h"
#include <sys/signalfd.h>
#include <unordered_map>
NAMESPACE_ZL_NET_START

typedef int Signalfd;

class SignalfdHandler
{
public:
    SignalfdHandler();
    ~SignalfdHandler();

public:
    Signalfd fd() { return signalFd_; }

    void addSigHandler(int sig, const SignalCallback& handler);

    void removeSig(int sig);

    bool haveSignal(int sig);

    /// must call registerAll after addSigHandler
    void registerAll(int flags = SFD_NONBLOCK | SFD_CLOEXEC);

    /// read signalno and return
    int readSig();

    /// sync loop-read 
    void wait();

    void stop() { isReady_ = false; }

private:
    Signalfd createSignalfd(int flags);

private:
    typedef std::unordered_map<int, SignalCallback>  SigHandlerMap;

    bool             isReady_;
    Signalfd         signalFd_;
    sigset_t         mask_; 
    SigHandlerMap    sigHanhlers_;
};

NAMESPACE_ZL_NET_END
#endif  /* ZL_SIGNALFD_H */
