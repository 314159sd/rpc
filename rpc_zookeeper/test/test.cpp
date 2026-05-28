#include <zookeeper/zookeeper.h>
#include <iostream>
#include <cstring>
#include <unistd.h>

static bool connected = false;

void watcher(zhandle_t* zh, int type, int state,
             const char* path, void* context)
{
    std::cout << "[zk event] type=" << type << " state=" << state;
    if (path) std::cout << " path=" << path;
    std::cout << std::endl;

    if (state == ZOO_CONNECTED_STATE)
        connected = true;
}

int main()
{
    zhandle_t* zh = zookeeper_init(
        "127.0.0.1:2181",
        watcher,
        10000,
        0, nullptr, 0
    );
    if (!zh) {
        std::cerr << "zookeeper_init failed\n";
        return 1;
    }

    // 等连接就绪
    for (int i = 0; i < 50 && !connected; ++i)
        usleep(100000);

    if (!connected)
        std::cout << "warning: not yet connected\n";

    std::cout << "zookeeper session OK\n";
    zookeeper_close(zh);
}