#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <algorithm>


class RpcConfig{
public:
    RpcConfig();
    ~RpcConfig();
    std::string Get(std::string key);
    void LoadConfigFile(std::string config_file);
    static RpcConfig& GetInstance();
private:
    std::unordered_map<std::string, std::string> config_map_;
};
        
