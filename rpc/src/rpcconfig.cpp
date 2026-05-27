#include "rpcconfig.h"

RpcConfig::RpcConfig() = default;
RpcConfig::~RpcConfig() = default;

std::string RpcConfig::Get(std::string key){
    return config_map_[key];
}

void RpcConfig::LoadConfigFile(std::string config_file){
    // load config file
    std::fstream ifs(config_file, std::ios::out);
    if(ifs.fail()){
        std::cout << "open config file failed: " << config_file << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string line;
    while(std::getline(ifs, line)){
        if(line.empty()){
            continue;
        }
        if(line[0] == '#'){
            continue;
        }
        
        if(line.find("=") == std::string::npos){
            std::cout << "config file format error: " << line << std::endl;
            continue;
        }

        replace(line.begin(), line.end(), '=', ' ');
        std::string key, value;
        std::istringstream iss(line);
        iss >> key >> value;
        config_map_[key] = value;
    }
}

RpcConfig& RpcConfig::GetInstance(){
    static RpcConfig instance;
    return instance;
}