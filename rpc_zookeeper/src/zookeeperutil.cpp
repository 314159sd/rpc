#include "zookeeperutil.h"
#include "rpcapplication.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <semaphore.h>

// 全局的watcher观察器
void global_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx) {
    (void)zh;
    (void)type;
    (void)state;
    (void)path;
    (void)watcherCtx;
}

// 回调函数中使用的信号量（静态变量保证生命周期）
static sem_t create_sem;
static int create_rc;

// 创建完成回调
void create_completion(int rc, const char *path, const void *data) {
    (void)path;
    (void)data;
    create_rc = rc;
    sem_post(&create_sem);
}

ZookeeperUtil::ZookeeperUtil() : zh_(nullptr) {
    sem_init(&create_sem, 0, 0);
}

ZookeeperUtil::~ZookeeperUtil() {
    sem_destroy(&create_sem);
    if (zh_ != nullptr) {
        zookeeper_close(zh_);
        zh_ = nullptr;
    }
}

void ZookeeperUtil::Init() {
    std::string host = RpcApplication::config_.Get("rpczookeeperhost");
    std::string port = RpcApplication::config_.Get("rpczookeeperport");
    std::string connstr = host + ":" + port;

    std::cout << "Connecting to ZooKeeper: " << connstr << std::endl;

    zh_ = zookeeper_init(connstr.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
    if (zh_ == nullptr) {
        std::cerr << "zookeeper_init error!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // 轮询等待连接
    int timeout = 300;
    while (timeout > 0) {
        int state = zoo_state(zh_);
        if (state == ZOO_CONNECTED_STATE) {
            std::cout << "zookeeper_init success!" << std::endl;
            return;
        }
        usleep(100000);
        timeout--;
    }

    std::cerr << "zookeeper connection timeout!" << std::endl;
    exit(EXIT_FAILURE);
}

void ZookeeperUtil::Create(const char *path, const char *data, int datalen, int state) {
    std::cout << "=== Create called ===" << std::endl;
    // std::cout << "zh_ pointer: " << (void*)zh_ << std::endl;
    
    if (zh_ == nullptr) {
        std::cerr << "ERROR: ZookeeperUtil not initialized!" << std::endl;
        return;
    }

    if (path == nullptr) {
        std::cerr << "ERROR: path is null!" << std::endl;
        return;
    }
    
    std::cout << "path: " << path << std::endl;
    std::cout << "data: " << (data ? data : "null") << std::endl;
    std::cout << "datalen: " << datalen << std::endl;
    std::cout << "state: " << state << std::endl;

    const char* node_data = data ? data : "";
    int node_datalen = data ? datalen : 0;

    std::cout << "Calling zoo_acreate (sync mode)..." << std::endl;
    
    // 使用异步API但通过信号量实现同步等待
    zoo_acreate(zh_, path, node_data, node_datalen, &ZOO_OPEN_ACL_UNSAFE, state, create_completion, nullptr);
    
    // 等待回调完成（最多等待5秒）
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 5;
    int sem_rc = sem_timedwait(&create_sem, &ts);
    
    if (sem_rc == -1) {
        std::cerr << "ERROR: sem_timedwait timeout!" << std::endl;
        return;
    }
    
    if (create_rc == ZOK) {
        std::cout << "znode create success!" << std::endl;
    } else if (create_rc == ZNODEEXISTS) {
        std::cout << "znode already exists!" << std::endl;
    } else {
        std::cerr << "zoo_create error, rc: " << create_rc << std::endl;
    }
    
    std::cout << "Create completed!" << std::endl;
}

void ZookeeperUtil::GetData(const char *path) {
    if (zh_ == nullptr) {
        std::cerr << "ZookeeperUtil not initialized!" << std::endl;
        return;
    }

    zoo_aget(zh_, path, 0, nullptr, nullptr);
}