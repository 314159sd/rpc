#include "rpcapplication.h"

RpcConfig RpcApplication::config_;
void ShowHelp(){
    std::cout << "format: command -i <config_file>" << std::endl;
}

void RpcApplication::Init(int argc, char** argv){
    if(argc < 2){
        ShowHelp();
        return;
    }

    int c =0;
    std::string config_file;

    while((c = getopt(argc, argv, "i:")) != -1){
        switch(c){
            case 'i':
                config_file = optarg;
                std::cout << "config file: " << config_file << std::endl;
                break;
            case '?':
                std::cout << "unknown option: " << optopt << std::endl;
                ShowHelp();
                exit(EXIT_FAILURE);
            case ':':
                ShowHelp();
                exit(EXIT_FAILURE);
            default:
                ShowHelp();
                return;
        }
    }

    if(config_file.empty()){
        std::cout << "config file is empty" << std::endl;
        return;
    }
    
    config_.LoadConfigFile(config_file);

    std::cout << "config: " << config_.Get("rpcserverip") << std::endl;
    std::cout << "config: " << config_.Get("rpcserverport") << std::endl;
}


RpcApplication& RpcApplication::GetInstance(){
    static RpcApplication instance;
    return instance;
}
