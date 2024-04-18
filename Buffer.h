#pragma once

#include<vector>
#include<string>

#include<stddef.h>

//网络库底层的缓冲器类型定义
class Buffer{
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t initiaLSize = kInitialSize) 
          : buffer_(kCheapPrepend + kInitialSize)
          ,readerIndex_(kCheapPrepend)
          ,writerIndex_(kCheapPrepend){}

    size_t readableBytes() const{    //可读数据大小
        return writerIndex_ - readerIndex_;
    }

    size_t writableBytes() const{
        return buffer_.size() - writerIndex_;
    }

    size_t prependableBytes() const{
        return readerIndex_;
    }

    //return buffer 可读数据的起始地址
    const char* peek() const{
        return begin() + readerIndex_;
    }

    //onMessage string <- Buffer
    void retrieve(size_t len){
        if(len < readableBytes()){
            readerIndex_ += len; //应用只读取了刻度缓冲区的一部分,就是len,还剩下readerIndex_ += len -> writerIndex_
        }else{  // len == readableBytes()
            retrieveAll();
        }
    }

    void retrieveAll(){
        readerIndex_ = writerIndex_ = kCheapPrepend;
    }
    
    //把onMessage函数上报的buffer数据，转成string类型的数据返回
    std::string retrieveAllAsString(){
        return retrieveAsString(readableBytes()); //应用可读取数据的长度
    }

    std::string retrieveAsString(size_t len){
        std::string result(peek(),len);
        retrieve(len);
        return result;
    }

    void ensureWriteableBytes(size_t len){
        if(writableBytes() < len){
            makeSpace(len); // 扩容
        }
    }

    //把[data,data+len]内存上的数据，添加到writable缓冲区当中
    void append(const char* data,size_t len){
        ensureWriteableBytes(len);
        std::copy(data,data+len,beginWrite());
        writerIndex_ += len;
    }

    char* beginWrite(){
        return begin() + writerIndex_;
    }

    const char* beginWrite() const{
        return begin() + writerIndex_;
    }

    //从fd上读取数据
    size_t readFd(int fd,int* saveErrno);
    size_t writeFd(int fd,int* saveErrno);
private:
    char* begin(){
        return &*buffer_.begin();
    }

    const char* begin() const{
        return &*buffer_.begin();
    }

    void makeSpace(size_t len){
        if(writableBytes() + prependableBytes() < len + kCheapPrepend){
            buffer_.resize(writerIndex_ + len);
        }else{
            size_t readalbe = readableBytes();
            std::copy(begin() + readerIndex_,
                    begin() + writerIndex_,
                    begin() + kCheapPrepend);
            readerIndex_ = kCheapPrepend;
            writerIndex_ = readerIndex_ + readalbe;
        }
    }
    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;
};