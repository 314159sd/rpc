#pragma once

#include <google/protobuf/service.h>
#include "rpcconfig.h"
#include <memory>
#include <muduo/net/TcpServer.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/Buffer.h>
#include <muduo/base/Timestamp.h>
#include <unordered_map>



class RpcProvider
{
public:
    RpcProvider()=default;
    ~RpcProvider()=default;


    void NotifyService(google::protobuf::Service* service);

    void Run();

    void OnConnection(const muduo::net::TcpConnectionPtr& conn);

    void OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp time);

    void SendResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response);
private:
    // std::unique_ptr<muduo::net::TcpServer> server_;
    muduo::net::EventLoop loop_;
    // muduo::net::InetAddress addr_;

    struct ServiceInfo{
        google::protobuf::Service* service;
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> method_map;
    };

    std::unordered_map<std::string, ServiceInfo> service_map_;
};