#include "zlreactor/net/DomainResolver.h"
NAMESPACE_ZL_NET_START


void HostEntry::initialize(const struct hostent* entry)
{
    hostName_ = entry->h_name;

    char** alias = entry->h_aliases;
    if (alias)
    {
        while (*alias)
        {
            aliases_.push_back(std::string(*alias));
            ++alias;
        }
    }

    char** address = entry->h_addr_list;
    if (address && entry->h_addrtype == AF_INET)
    {
        while (*address)
        {
            in_addr *i = reinterpret_cast<in_addr*>(*address);
            sockaddr_in d  = *((sockaddr_in*)(i));;
            add(InetAddress(d));
            ++address;
        }
    }
}

void HostEntry::initialize(const struct addrinfo* ainfo)
{
    for (const struct addrinfo* ai = ainfo; ai; ai = ai->ai_next)
    {
        if (ai->ai_canonname)
        {
            hostName_.assign(ai->ai_canonname);
        }
        else if (ai->ai_addrlen && ai->ai_addr && ai->ai_family == AF_INET)
        {
            sockaddr_in *d = reinterpret_cast<sockaddr_in*>(ai->ai_addr);
            add(InetAddress(*d));
        }
    }
}

void HostEntry::add(const InetAddress& address)
{
    if (std::find(addresses_.begin(), addresses_.end(), address) == addresses_.end())
        addresses_.push_back(address);
}

/////////////////////////////////////////////////////////////////////////////
// DomainResolver class implementation

bool DomainResolver::query(const std::string& hostname, HostEntry* host, int *error)
{
    struct hostent* he = NULL;
    int err = 0;
#ifdef _GNU_SOURCE
    char buf[4096];
    struct hostent he_buf;
    gethostbyname_r(hostname.c_str(), &he_buf, buf, sizeof(buf), &he, &err);
#else
    he = gethostbyname(hostname.c_str());
    if (!he)
        err = h_errno;
#endif
    if (!he)
    {
        if(error)    *error = err;
        return false;
    }
    host->initialize(he);

    if(error)    *error = err;
    return true;
}

bool DomainResolver::resolveInetAddress(const std::string& hostname, std::vector<InetAddress>* ips, int *error)
{
    HostEntry host;
    if (query(hostname, &host, error))
    {
        *ips = host.addresses();
        return true;
    }
    return false;
}

bool DomainResolver::isError(int error)
{
    return error != 0 && error != TRY_AGAIN;
}

std::string DomainResolver::errorString(int error)
{
#ifndef OS_WINDOWS
    return hstrerror(error);
#else
    return strerror(error);
#endif
}

NAMESPACE_ZL_NET_END