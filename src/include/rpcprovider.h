#pragma once
#include "google/protobuf/service.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>

#include <string>
#include <unordered_map>

#include <memory>

/**
 * @brief rpc框架提供的专门发布rpc服务的网络对象,负责数据的序列化与反序列和网络数据收发
 *        主要使用muduo网络库代码编写TcpServer 
 *
 */
class RpcProvider {
public:
  // 这里是提供给外部使用的，发布rpc方法的函数接口
  void NotifyService(google::protobuf::Service *service);

  // 启动rpc节点，开始提供rpc远程网络调用服务
  void run();

private:
  muduo::net::EventLoop m_eventLoop; // main loop

  // 用户自定义回调函数:入参为TcpConnectionPtr（因为一个连接对应一个TcpConnectionPtr）
  void OnConnection(const muduo::net::TcpConnectionPtr&); // 新连接（socket）回调方法
  void OnMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp);    // 已建立连接接收数据回调方法
  void SendRpcResponse(const muduo::net::TcpConnectionPtr&, google::protobuf::Message*);
  
  // service服务类型信息
  struct ServiceInfo {
    google::protobuf::Service *m_service; // 保存服务对象
    // 保存服务方法名字 与 服务方法
    std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> m_methodMap; 
  };

  // 存储注册成功的服务对象名字 与 服务对象方法的所有信息
  std::unordered_map<std::string, ServiceInfo> m_serviceMap;

};