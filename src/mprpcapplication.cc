#include "mprpcapplication.h"

#include <cstdlib>
#include <unistd.h>

#include "mprcplogger.h"

#include <string>
#include <iostream>

MprpcConfig MprpcApplication::m_config;

void ShowArgsHelp() {
  XLOG_ERROR("format: command -i <configfile> ");
}

/**
 * @brief 负责框架的初始化操作
 * 
 */
void MprpcApplication::Init(int argc, char **argv) {
  if (argc < 2) {
    ShowArgsHelp();
    exit(EXIT_FAILURE);
  }

  // step 1. 读取配置文件名称
  int c = 0;
  std::string config_file;
  // 使用getopt 截取命令行的指令
  while ((c = getopt(argc, argv, "i:")) != -1) {
    switch (c) {
    case 'i':
      config_file = std::string(optarg);
      XLOG_INFO("Open file: {}",config_file);
      break;

    case '?':
      ShowArgsHelp();
      exit(EXIT_FAILURE);
      break;

    case ':':
      ShowArgsHelp();
      exit(EXIT_FAILURE);
      break;

    default:
      break;
    }
}

  // step 2. 开始加载配置文件：rpcserver_ip rpcserver_port zookeeper_ip zookeeper_port
  m_config.LoadConfigFile(config_file.c_str());

  XLOG_INFO("Load configfile successful: ");
  XLOG_INFO("\nrpcserver_ip: {} ", m_config.Load("rpcserver_ip").c_str());
  XLOG_INFO("rpcserver_port: {} ", m_config.Load("rpcserver_port").c_str());
  XLOG_INFO("zookeeper_ip: {} ", m_config.Load("zookeeper_ip").c_str());
  XLOG_INFO("zookeeper_port: {} \n", m_config.Load("zookeeper_port").c_str());
}

MprpcApplication::MprpcApplication() {}
