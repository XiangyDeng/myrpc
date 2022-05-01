#pragma once

#include <semaphore.h>
#include <zookeeper/zookeeper.h>
#include <string>

class ZkClient {
public:
  ZkClient();
  ~ZkClient();
  // ZkClient 启动连接 zkserver
  void Start();
  // 在 zkserver 上指定的path创建znode节点
  void Create(const char *path, const char *data, int datalen, int state = 0);
  // 根据参数指定的znode节点路径，获得znode节点的值
  std::string GetData(const char *path);
private:
  // zk的客户端句柄
  zhandle_t *m_zhandle;
};