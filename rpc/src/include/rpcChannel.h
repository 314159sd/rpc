#pragma once
#include <google/protobuf/service.h>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>




class RpcChannel:public google::protobuf::RpcChannel{
public:
    RpcChannel()=default;
    ~RpcChannel()=default;

    void CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                          google::protobuf::Message* response, google::protobuf::Closure* done);
};
        