// ***********************************************************************
// Filename         : DomainResolver.h
// Author           : LIZHENG
// Created          : 2015-04-09
// Description      : 
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_DOMAINRESOLVER_H
#define ZL_DOMAINRESOLVER_H
#include "Define.h"
#include "net/InetAddress.h"
struct hostent;
struct addrinfo;
NAMESPACE_ZL_NET_START

class DomainResolver;

/// 存储主机名、别名、ip地址
class HostEntry
{
public:
    typedef std::vector<std::string>   AliasList;
    typedef std::vector<InetAddress>   AddressList;

public:
    void initialize(const struct hostent* entry);

    void initialize(const struct addrinfo* info);

    const std::string& name() const      { return hostName_; }

    const AliasList aliases() const      { return aliases_; }

    const AddressList& addresses() const { return addresses_; }

    void clear()
    {
        hostName_.clear();
        aliases_.clear();
        addresses_.clear();
    }

    void swap(HostEntry& other)
    {
        std::swap(hostName_, other.hostName_);
        std::swap(aliases_, other.aliases_);
        std::swap(addresses_, other.addresses_);
    }

private:
    void add(const InetAddress& address);

private:
    std::string hostName_;
    AliasList   aliases_;      /// alias names
    AddressList addresses_;
};

class DomainResolver
{
public:
    static bool query(const std::string& hostname, HostEntry* host, int *error = NULL);

    static bool resolveInetAddress(const std::string& hostname, std::vector<InetAddress>* ips, int *error = NULL);

    static bool isError(int error);

    static std::string errorString(int error);
};

#endif  /* ZL_DOMAINRESOLVER_H */
NAMESPACE_ZL_NET_END
