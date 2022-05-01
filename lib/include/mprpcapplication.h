#pragma once

#include "mprpcconfig.h"

/**
 * @brief mprpc框架的基础类,负责框架的初始化操作
 * 
 */
class MprpcApplication {
public:
  static void Init(int argc, char **argv);
  // 单例构造
  static MprpcApplication &getInstance() {
    static MprpcApplication instance;
    return instance;
  }

  // 访问数据成员接口
  MprpcConfig &GetMprpcConfig() { return m_config; }
  
private:
  MprpcApplication();
  MprpcApplication(const MprpcApplication &) = delete;
  MprpcApplication(MprpcApplication &&) = delete;

  static MprpcConfig m_config;
};

