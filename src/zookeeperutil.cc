#include "zookeeperutil.h"
#include <cstddef>
#include <cstdlib>
#include <semaphore.h>
#include <iostream>
#include <zookeeper/zookeeper.h>

#include "mprpcapplication.h"
#include "./include/mprcplogger.h"

// * 全局的watcher观察器回调:保证zk 异步init的情况下，初始化完毕, 此时zkserver给zkclient下发通知
// 独立线程运行
void global_watcher(zhandle_t *zh, int type, int state, const char *path,
                    void *watherCtx) {
  if (type == ZOO_SESSION_EVENT) {  // 回调的消息 为 与会话相关的信号量
    if (state == ZOO_CONNECTED_STATE) { // ZkClient 与 zkserver 为连接状态
      sem_t *sem = (sem_t *)zoo_get_context(zh);  // 向zk客户端句柄中取出信号量的信息
      sem_post(sem);                              // * 并通知zkclient初始化已经完成
    }
  }
}

ZkClient::ZkClient() : m_zhandle(nullptr) {}

ZkClient::~ZkClient() {
  if (m_zhandle != nullptr) { 
    zookeeper_close(m_zhandle); //  关闭句柄，释放资源
  }
}

// ZkClient 启动连接 zkserver
void ZkClient::Start() {
  // step 1. 获取zk ip 与zk port，初始化zk,返回客户端句柄
  std::string host = MprpcApplication::getInstance().GetMprpcConfig().Load("zookeeper_ip");
  std::string port = MprpcApplication::getInstance().GetMprpcConfig().Load("zookeeper_port");

  std::string conn_str = host + ":" + port;

  /*
  zookeeper_mt 为多线程版本
  zookeeper的客户端API版本提供了三个线程 :
  1. API调用线程
  2. 网络I/O线程 ： pthread_create poll
  3. wather回调线程
  */
  // 异步调用初始化，需要使用sem同步操作，等待初始化完成
  m_zhandle = zookeeper_init(conn_str.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
  if (m_zhandle == nullptr) {
    XLOG_ERROR("Zookeeper init failed!");
    exit(EXIT_FAILURE);
  }

  // step 2. 设置信号量同步等待异步初始化完成
  sem_t sem;
  sem_init(&sem, 0, 0);
  // 为客户端句柄设置上下文，添加信号量的额外信息，等待全局watcher观察zk客户端句柄是否初始化完成;
  // 1. 回调的消息 为 与会话相关的信号量
  // 2. ZkClient 与 zkserver 为连接状态
  zoo_set_context(m_zhandle, &sem); 

  sem_wait(&sem);
  XLOG_INFO("Zookeeper init successful!");
}

// 在 zkserver 上指定的path创建znode节点
void ZkClient::Create(const char *path, const char *data, int datalen, int state) {
  char path_buffer[128];
  int path_buffer_size = sizeof path_buffer;
  // 先判断znode节点是否存在，不用设置异步watcher
  int flag = zoo_exists(m_zhandle, path, 0, nullptr);
  if (flag == ZNONODE) {
    // 创建znode节点，并返回是否创建成功的标志位
    flag = zoo_create(m_zhandle, path, data, datalen, &ZOO_OPEN_ACL_UNSAFE,
                      state, path_buffer, path_buffer_size);
    
    if (flag == ZOK) {
      XLOG_INFO("znode created successful, path: {}", path);

    } else {
      XLOG_ERROR("Flag : {}", flag);
      XLOG_ERROR("znode created failed, path: {}", path);
      exit(EXIT_FAILURE);
    }

  }
}
// 根据参数指定的znode节点路径，获得znode节点的值 （zoo_get）
std::string ZkClient::GetData(const char *path) {
  char buffer[128];
  int buffer_len = sizeof buffer;
  // 不用设置异步watcher
  int flag = zoo_get(m_zhandle, path, 0, buffer, &buffer_len, nullptr);
  if (flag == ZOK) {
    XLOG_INFO("ZkClient getData successful!, content is: {}", buffer);
  } else {
    XLOG_ERROR("Flag : {}", flag);
    XLOG_ERROR("ZkClient getData failed!, ERROR flag is: {}", flag);
    exit(EXIT_FAILURE);
  }
  return buffer;
}