#include "mprpcconfig.h"
#include "mprcplogger.h"

#include <cstdlib>
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// 解析加载配置文件
void MprpcConfig::LoadConfigFile(const char *config_file) {
  std::ifstream infile(config_file, std::ios::in);
  if (!infile.is_open()) {
    XLOG_ERROR("Open configfile failed: {}", config_file);
    infile.close();
    exit(EXIT_FAILURE);
  }

  std::string s;
  while (std::getline(infile, s)) {
    // * 健壮性处理：注释，空格，正确的配置格式
    // step 1. 去除注释
    if (s[0] == '#') {
      continue;
    }
    auto it = s.find('=');
    if (it == s.npos) {
      continue;
    }

    std::string key = std::string(s, 0, it);
    std::string value = std::string(s, it + 1, s.size() - 1 - it);

    // step 2. 去除多余空格，用字符串流操作
    std::stringstream ss(key);
    ss >> key;
    ss.clear();   // ! 此处清空数据很重要
    ss << value;
    ss >> value;

    // 存入hashmap
    m_configMap[key] = value;
  }

  infile.close();
}

// 查询配置项信息
std::string MprpcConfig::Load(std::string key) {
  if (m_configMap.count(key)) {
    return m_configMap[key];
  } else {
    return "";
  }
}