#include "net/Poller.h"
//#include "Channel.h"
NAMESPACE_ZL_NET_START

Channel     *Poller::getChannel(ZL_SOCKET sock)
{
    ChannelMap::iterator itr = channelMap.find(sock);
    if(itr == channelMap.end())
        return NULL;
    return itr->second;
}

NAMESPACE_ZL_NET_END

