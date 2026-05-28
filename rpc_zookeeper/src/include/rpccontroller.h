#pragma once
#include <string>
#include <google/protobuf/service.h>

class RpcController:public google::protobuf::RpcController{
public:
    RpcController();
    ~RpcController() = default;

    void Reset();
    bool Failed() const;
    std::string ErrorText() const;
    void SetFailed(const std::string& reason);

    void StartCancel();
    bool IsCanceled() const;
    void NotifyOnCancel(google::protobuf::Closure* callback);

private:
    bool failed_;
    std::string error_text_;
};