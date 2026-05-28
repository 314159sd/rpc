#pragma once
#include <iostream>
#include <string>
#include <getopt.h>
#include "rpcconfig.h"
#include "rpccontroller.h"



class RpcApplication
{

    RpcApplication()=default;
    // RpcApplication(const RpcApplication&) = delete;
    RpcApplication& operator=(const RpcApplication&) = delete;  
    ~RpcApplication()=default;

public:
    static RpcConfig config_;
    static void Init(int argc, char** argv);
    static RpcApplication& GetInstance();
};


