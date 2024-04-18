#include<mymuduo/TcpServer.h>
#include<mymuduo/logger.h>
#include<mymuduo/Buffer.h>
#include<mymuduo/TcpConnection.h>

#include<string>
#include<functional>

class EchoServer{
public:
    EchoServer(EventLoop* loop,
            const InetAddress& addr,
            const std::string& name)
        : server_(loop,addr,name),loop_(loop){
            //注册回调函数
            server_.setConnectionCallback(
                std::bind(&EchoServer::onConnection,this,std::placeholders::_1)
            );

            server_.setMessageCallback(
                std::bind(&EchoServer::onMessage,this,
                    std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)
            );
            //设置合适的loop thread 数量
            server_.setThreadNum(3);
    }

    void start(){
        server_.start();
    }

private:
    //连接建立或断开的回调
    void onConnection(const TcpConnectionPtr& conn){
        if(conn->connecteed()){
            LOG_INFO("Connection UP : %s",conn->peerAddress().toIpPort().c_str());
        }else{
            LOG_INFO("Connection Down : %s",conn->peerAddress().toIpPort().c_str());
        }
    }

    //可读写事件回调
    void onMessage(const TcpConnectionPtr& conn,Buffer* buf,Timestamp time){
        std::string msg = buf->retrieveAllAsString();
        conn->send(msg);
        conn->shutdown(); // 写端 EPOLLUP => closeCallback_
    }
    EventLoop* loop_;
    TcpServer server_;
};

int main(){
    EventLoop loop;
    InetAddress addr(8000);
    EchoServer server(&loop,addr,"EchoServer-01");
    server.start();
    loop.loop();

    return 0;
}