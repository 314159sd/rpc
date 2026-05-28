#include "rpcprovider.h"
#include <thread>
#include <google/protobuf/descriptor.h>
#include "rpcheader.pb.h"
#include "rpcapplication.h"
#include "zookeeperutil.h"
// #include <google/protobuf/stubs/callback.h>

RpcProvider::RpcProvider() : loop_(new muduo::net::EventLoop()) {}

void RpcProvider::NotifyService(google::protobuf::Service* service){
    ServiceInfo service_info;
    service_info.service = service;
    const google::protobuf::ServiceDescriptor *service_desc = service->GetDescriptor();

    std::string service_name = service_desc->name();
    int method_count = service_desc->method_count();

    std::cout << "service name: " << service_name << std::endl;
    std::cout << "method count: " << method_count << std::endl;
    
    for (int i = 0; i < method_count; i++) {
        const google::protobuf::MethodDescriptor *method = service_desc->method(i);
        std::cout << "method name: " << method->name() << std::endl;
        std::string method_name = method->name();
        service_info.method_map.insert(std::make_pair(method_name, method));
    }
    service_map_[service_name] = service_info;
}

void RpcProvider::Run(){
    std::string ip = RpcApplication::config_.Get("rpcserverip");
    uint16_t port = atoi(RpcApplication::config_.Get("rpcserverport").c_str());

    if (ip.empty()) {
        std::cerr << "ERROR: rpcserverip is empty!" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (port == 0) {
        std::cerr << "ERROR: rpcserverport is invalid!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // ========== 在单独的作用域中完成 ZooKeeper 操作 ==========
    {
        std::cout << "Initializing ZooKeeper..." << std::endl;
        
        ZookeeperUtil zookeeper_util;
        zookeeper_util.Init();
        
        std::cout << "Creating znode paths..." << std::endl;
        for(auto &sp : service_map_){
        std::string service_path = "/" + sp.first;
        zookeeper_util.Create(service_path.c_str(), nullptr, service_path.size(), 0);
        std::cout << "create service path: " << service_path << std::endl;
        
        // 短暂等待确保节点创建完成
        usleep(100000);  // 100ms
        
        for(auto &mp : sp.second.method_map){
            std::string method_path = service_path + "/" + mp.first;
            char method_path_buffer[128] = {0};
            sprintf(method_path_buffer, "%s:%d", service_path.c_str(), port);
            zookeeper_util.Create(method_path.c_str(), method_path_buffer, strlen(method_path_buffer), ZOO_EPHEMERAL);
            std::cout << "create method path: " << method_path << std::endl;
            
            // 短暂等待确保节点创建完成
            usleep(100000);  // 100ms
        }
    }
    
        
        // zookeeper_util 在此处析构，会调用 zookeeper_close
    }
    std::cout << "start service at ip: " << ip << " port: " << port << std::endl;


    muduo::net::InetAddress addr(ip, port);
    
    muduo::net::TcpServer server(loop_, addr, "RpcProvider");
    
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, 
                                    std::placeholders::_2, std::placeholders::_3));

    server.setThreadNum(4);

    std::cout << "Calling server.start()..." << std::endl;
    server.start();
    
    std::cout << "Calling loop_.loop()..." << std::endl;
    loop_->loop();
}

void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn){
    if (!conn->connected()) {
        std::cout << "connection closed" << std::endl;
        conn->shutdown();
        return;
    }

    std::cout << "connection established" << std::endl;

}

void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp time){
    std::string message = buf->retrieveAllAsString();

    uint32_t header_size = 0;
    message.copy((char*)&header_size, 4, 0);
    header_size = ntohl(header_size); 
    std::cout << "header size: " << header_size << std::endl;
    std::string header_str = message.substr(4, header_size);

    fixbug::RpcHeader header;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;
    std::string args_str;
    
    if(header.ParseFromString(header_str)){
        service_name = header.service_name();
        method_name = header.method_name();
        args_size = header.arg_size();
        
        std::cout << "args size: " << header_size << std::endl;
        args_str = message.substr(4 + header_size, args_size);

        std::cout << "service name: " << service_name << std::endl;
        std::cout << "method name: " << method_name << std::endl;
        std::cout << "args size: " << args_size << std::endl;
        // std::cout << "args: " << args_str.size() << std::endl;
    }
    else{
        std::cout << "invalid header" << std::endl;
        return;
    }

    auto it = service_map_.find(service_name);
    if(it == service_map_.end()){
        std::cout << "service not found" << std::endl;
        return;
    }

    
    auto method = it->second.method_map.find(method_name);
    if(method == it->second.method_map.end()){
        std::cout << "method not found" << std::endl;
        return;
    }

    google::protobuf::Service *service = it->second.service;
    const google::protobuf::MethodDescriptor *method_desc = method->second;

    google::protobuf::Message* request = service->GetRequestPrototype(method_desc).New();
    if(!request->ParseFromString(args_str)){
        std::cout << "invalid request" << std::endl;
        return;
    }
    google::protobuf::Message* response = service->GetResponsePrototype(method_desc).New();

    google::protobuf::Closure* callback = google::protobuf::NewCallback<RpcProvider,
                                                                        const muduo::net::TcpConnectionPtr&, 
                                                                        google::protobuf::Message*>
                                                                        (this, &RpcProvider::SendResponse, conn, response);
    service->CallMethod(method_desc, nullptr, request, response, callback);

}

void RpcProvider::SendResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response){
    std::string response_str;
    if(response->SerializeToString(&response_str)){
        conn->send(response_str);
        
    }
    else{
        std::cout << "serialize response failed" << std::endl;
    }
    conn->shutdown();
}

