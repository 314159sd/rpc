#include <rpcapplication.h>
#include "user.pb.h"
#include "rpcChannel.h"

int main(int argc, char** argv){
    RpcApplication::Init(argc, argv);

    fixbug::UserService_Stub stub(new RpcChannel());
    fixbug::LoginRequest request;
    request.set_name("yoyo");
    request.set_pwd("123456");
    fixbug::LoginResponse response;
    stub.Login(nullptr, &request, &response, nullptr);
    
    if(response.result().errcode() == 0){
        std::cout << "login success" << std::endl;
    }else{
        std::cout << "login failed" << std::endl;
    }


    return 0;
}
