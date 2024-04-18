#pragma once
#include"noncopyable.h"
#include"Timestamp.h"
#include<vector>
#include<unordered_map>

class Channel;
class EventLoop;

//mudou库中多路事件分发器的核心IO复用模块

class Poller : noncopyable{
public:
    using ChannelList = std::vector<Channel*>;

    Poller(EventLoop* Loop);
    virtual ~Poller() = default;

    //给所有IO复用保留统一接口
    virtual Timestamp poll(int timeoutMs,ChannelList* activeChanneLs) = 0;
    virtual void updateChannel(Channel* channeL) = 0;
    virtual void removeChannel(Channel* channeL) = 0;

    //判断参数channel是否在当前Poller当中
    bool hasChannel(Channel* channeL) const;

    //EventLoop 可以通过该接口获取默认的IO复用的具体实现
    static Poller* newDefaultPoller(EventLoop* loop);
protected:
    //map 的 key ： sockfd    value : sockfd 所需的Channel通道类型
    using ChannelMap = std::unordered_map<int,Channel*>;
    ChannelMap channels_;
private:
    EventLoop* ownerLoop_;
};