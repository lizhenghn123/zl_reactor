// ***********************************************************************
// Filename         : ScopeExitGuard.h
// Author           : LIZHENG
// Created          : 2014-10-01
// Description      : RAII类，用于资源释放、清理
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_SCOPEEXITGUARD_H
#define ZL_SCOPEEXITGUARD_H
#include <functional>

#define SCOPEGUARD_LINENAME_CAT(name, line) name##line
#define SCOPEGUARD_LINENAME(name, line)     SCOPEGUARD_LINENAME_CAT(name, line)
#define ON_SCOPE_EXIT(callback) zl::base::ScopeExitGuard SCOPEGUARD_LINENAME(EXIT, __LINE__)(callback)

namespace zl
{
namespace base
{

class ScopeExitGuard
{
public:
    explicit ScopeExitGuard(std::function<void ()> onExitCallback)
        : onExitCb_(onExitCallback), dismissed_(false)
    {
    }

    ScopeExitGuard(ScopeExitGuard&& rhs)
        : onExitCb_(std::move(rhs.onExitCb_))
        , dismissed_(rhs.dismissed_)
    {
    }

    ~ScopeExitGuard()
    {
        if(!dismissed_)
        {
            onExitCb_();
        }
    }

    void dismiss()
    {
        dismissed_ = true;
    }

private:
    std::function<void()> onExitCb_;
    bool dismissed_;

private:
    ScopeExitGuard(ScopeExitGuard const&);
    ScopeExitGuard& operator=(ScopeExitGuard const&);
};

template <typename F>
ScopeExitGuard makeScopeExitGuard(F&& f)
{
    return ScopeExitGuard(std::forward<F>(f));
}

}
}
#endif  /* ZL_SCOPEEXITGUARD_H */