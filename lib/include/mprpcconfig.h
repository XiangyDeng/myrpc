#pragma once

#include <unordered_map>
#include <string>

/**
 * @brief  rpc框架读取配置文件 ：rpcserver_ip rpcserver_port zookeeper_ip zookeeper_port
 * 
 */
class MprpcConfig {
public:
  // 解析加载配置文件
  void LoadConfigFile(const char *config_file);
  // 查询配置项信息
  std::string Load(std::string key);
private:
  std::unordered_map<std::string, std::string> m_configMap;
  
};