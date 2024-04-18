#include"Poller.h"
#include"Channel.h"

Poller::Poller(EventLoop* loop) 
    : ownerLoop_(loop){

}

bool Poller::hasChannel(Channel* channeL) const{
    auto it = channels_.find(channeL->fd());
    return it != channels_.end() && it->second == channeL;
}

