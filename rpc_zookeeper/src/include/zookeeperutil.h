#pragma once

#include <zookeeper/zookeeper.h>
#include <string>

class ZookeeperUtil
{
public:
    ZookeeperUtil();
    ~ZookeeperUtil();

    void Init();
    void Create(const char* path, const char* data, int len, int state);
    void GetData(const char* path);

private:
    zhandle_t* zh_;
};