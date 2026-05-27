#include <iostream>
#include "user.pb.h"
#include <string>
#include "rpcapplication.h"
#include "rpcprovider.h"



class UserService: public fixbug::UserService
{
private:
    /* data */
public:
    bool Login(std::string name, std::string pwd){
        std::cout << "login!" << std::endl;
        std::cout << "name" << name << "passward" << pwd << std::endl;
    }

    void Login(::google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,
                       ::fixbug::LoginResponse* response,
                       ::google::protobuf::Closure* done)
                       {
                            std::string name = request->name();
                            std::string pwd = request->pwd();
                            bool success = Login(name, pwd);
                            fixbug::ResultCode *result = response->mutable_result();
                            result->set_errcode(0);
                            result->set_errmsg("success");
                            response->set_success(success);

                            done->Run();
                       }

};


int main(int argc, char** argv){

    RpcApplication::Init(argc, argv);
    
    RpcProvider provider;
    provider.NotifyService(new UserService());
    

    return 0;
}