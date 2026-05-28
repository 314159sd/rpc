#include "rpcChannel.h"
#include "rpcheader.pb.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "rpcconfig.h"
#include <iostream>
#include <unistd.h>
#include "rpcapplication.h"
#include <fmt/core.h>

void RpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                          google::protobuf::Message* response, google::protobuf::Closure* done)
{
    const google::protobuf::ServiceDescriptor* service = method->service();
    std::string service_name = service->name();
    std::string method_name = method->name();
    std::cout << "service name: " << service_name << std::endl;
    std::cout << "method name: " << method_name << std::endl;

    uint32_t args_size = 0;
    std::string args_str;
    if(request->SerializeToString(&args_str)){
        args_size = args_str.size();
    }
    else{
        controller->SetFailed("serialize args failed");
        return;
    }

    fixbug::RpcHeader header;
    header.set_service_name(service_name);
    header.set_method_name(method_name);
    header.set_arg_size(args_size);

    uint32_t header_size;
    std::string header_str;
    if(header.SerializeToString(&header_str)){
        header_size = header_str.size();
        std::cout << "header: " << header_str.size() << std::endl;
    }
    else{
        controller->SetFailed("serialize header failed");
        return;
    }

    std::string request_str;
    // request_str.insert(0, sizeof(uint32_t), (char)header_size);
    uint32_t header_size_big = htonl(header_size);
    request_str.append((char*)&header_size_big, sizeof(uint32_t));
    request_str += header_str;
    request_str += args_str;
    std::cout << "request: " << request_str.size() << std::endl;
    fixbug::RpcHeader response_header;
    

    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if(clientfd < 0){
        std::string error_text = fmt::format("socket failed! {}", errno);
        controller->SetFailed(error_text);
        exit(EXIT_FAILURE);
    }
    std::string ip = RpcApplication::config_.Get("rpcserverip");
    // std::cout << "rpc server ip: " << ip << std::endl; 
    uint16_t port = atoi(RpcApplication::config_.Get("rpcserverport").c_str());
    // std::cout << "rpc server port: " << port << std::endl;

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    server_addr.sin_port = htons(port);

    if(connect(clientfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        std::string error_text = fmt::format("connect failed! {}", errno);
        controller->SetFailed(error_text);
        close(clientfd);
        exit(EXIT_FAILURE);
    }

    if(send(clientfd, request_str.c_str(), request_str.size(), 0) < 0){
        std::string error_text = fmt::format("send failed! {}", errno);
        controller->SetFailed(error_text);
        close(clientfd);
        exit(EXIT_FAILURE);
    }

    char response_buf[1024] {0};
    int response_size = recv(clientfd, response_buf, sizeof(response_buf), 0);
    if(response_size < 0){
        std::string error_text = fmt::format("recv failed! {}", errno);
        controller->SetFailed(error_text);
        close(clientfd);
        return;
    }

    std::string response_str(response_buf, 0, response_size);
    if(response_header.ParseFromString(response_str)){
        std::cout << "response header!" << std::endl;
    }
    else{
        std::string error_text = fmt::format("response header parse failed! {}", errno);
        controller->SetFailed(error_text);
    }
    close(clientfd);
   
}