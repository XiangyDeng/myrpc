#include "rpcprovider.h"
#include <cstdint>
#include <cstdio>
#include <google/protobuf/message.h>
#include <google/protobuf/stubs/callback.h>
#include <iostream>
#include <stdlib.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpServer.h>
#include <functional>
#include <zookeeper/zookeeper.h>

#include "mprcplogger.h"
#include "mprpcapplication.h"
#include "rpcheader.pb.h"
#include "zookeeperutil.h"

// 这里是提供给外部使用的，发布rpc方法的函数接口
void RpcProvider::NotifyService(google::protobuf::Service *service) {
  ServiceInfo service_info;

  // step 1. 获取服务对象的总信息
  const google::protobuf::ServiceDescriptor *pserverDesc = service->GetDescriptor();
  // 获取服务的名字
  const std::string service_name = pserverDesc->name();
  XLOG_INFO("[service_name]: {} -> notifyservice : ", service_name);

  // 获取服务的方法数量
  int method_cnt=pserverDesc->method_count();

  // step 2. 存储服务对象总信息（对象＋方法）到hashmap中: 使得框架可以自己找到请求service的方法
  // ex：UserService-> service   -> ServiceDescriptor
  //      Login函数 -> method[i] -> MethodDescriptor
  for (int i = 0; i < method_cnt; ++i) {
    // 取出每个方法的描述信息
    const google::protobuf::MethodDescriptor *pmethodDesc = pserverDesc->method(i);
    service_info.m_methodMap[pmethodDesc->name()] = pmethodDesc;
    XLOG_INFO("Notified method_name: {} ", pmethodDesc->name());
  }
  service_info.m_service = service;

  // step 3. 存入存储注册成功的服务对象与服务对象方法的所有信息
  m_serviceMap[service_name] = service_info;
}

// 启动rpc节点，开始提供rpc远程网络调用服务
// ? 为什么要使用muduo网络库
// * 很好的分离了 网络框架代码 与 业务代码
void RpcProvider::run() {
  std::string ip =
      MprpcApplication::getInstance().GetMprpcConfig().Load("rpcserver_ip");
  std::string port =
      MprpcApplication::getInstance().GetMprpcConfig().Load("rpcserver_port");

  // step 1. 设置TcpServer的地址：ip port，从配置文件中读取
  // * 注意此处atoi的使用：使用const char*类型
  muduo::net::InetAddress addr(ip, atoi(port.c_str()),  false);

  // step 2. 设置TcpServer对象
  muduo::net::TcpServer server(&m_eventLoop, addr, "RpcServer", muduo::net::TcpServer::Option::kNoReusePort);

  // step 3. 设置回调函数
  server.setConnectionCallback(
      std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
  server.setMessageCallback(
      std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
  
  
  // step 4. 设置muduo的线程数量：此处为subLoop，并非mainLoop
  server.setThreadNum(4);

  // ! step 5. 把当前服务端rpc节点上的所有服务注册到zookeeper上，使得zkClient可以从zk上 
  // ! 发现服务, timeout : 30s zkclient 网络IO线程以1/3*timeout时间发送ping消息（心跳）
  // 自身为rpc服务端，并以zkclient的身份连接zkserver
  ZkClient zkclient;
  zkclient.Start(); // 初始化zk服务,连接zkServer
  // 设置service_name为永久节点，method_name为临时节点
  for (auto &serviceInfo : m_serviceMap) {
    // 1. 添加service_name永久节点
    std::string service_path = "/" + serviceInfo.first;
    zkclient.Create(service_path.c_str(), nullptr, 0);  // 默认为永久节点

    // 2. 为service中的每个method_name设置临时节点
    for (auto &methodInfo : serviceInfo.second.m_methodMap) {
      std::string method_path = service_path + "/" + methodInfo.first;
      char method_path_data[128];
      sprintf(method_path_data, "%s:%s", ip.c_str(), port.c_str());
      zkclient.Create(method_path.c_str(), method_path_data, sizeof method_path,
                      ZOO_EPHEMERAL); // ! method 皆注册为临时性节点，自动删除，避免错误
    }
  }
  
  // 打印提示信息
  XLOG_INFO("\nRpcServer started -> ip: {}  port: {} \n", ip.c_str(), port.c_str());
  
  // step 6. 开启网络服务
  server.start();     // * 开启Acceptor的listen -> accept connfd -> 建立新连接 TcpConnection -> 线程池轮询分发subReactor监听处理
  m_eventLoop.loop(); // 开启epoll_wait监听mainLoop可连接事件
}

// 新连接（socket）回调方法
void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr &conn) {
  if (!conn->connected()) {
    // 与rpc client的连接断开了
    conn->shutdown();
  }
}

// * 在框架内部，RpcProvider和RpcConsumer协商好通信的protobuf的数据类型
// Service_name Service_method args:        定义proto的message类型，进行数据的序列化与反序列化（以对象为单位）
// ex：16UserServiceLoginZhang San123456    service_name methon_name args_size(解决通信粘包问题)
// header_size(4 byte 二进制存储 并非文本字符串, 数据头，服务与方法的名字) -> header_str() -> args_str()
// 已建立连接接收数据完毕回调方法，如果远程有一个rpc服务的调用请求，则该方法就会相应
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr &conn,
                            muduo::net::Buffer *buffer,
                            muduo::Timestamp timestamp) {
  // 二进制寸尺格式： header_size -> header_str -> args_str
  // step 1. 从muduo库接收的buffer中读取header_size
  std::string recv_buf = buffer->retrieveAllAsString();
  
  uint32_t header_size = 0;
  recv_buf.copy((char*)&header_size, 4, 0); // ! header_size在rpc通信时为二进制存储，直接copy即可（效率更高）

  // step 2. 根据header_size取得数据头的信息，反序列化后获得rpc请求的详细信息
  std::string header_str = recv_buf.substr(4, header_size);

  mprpc::RpcHeader rpcheader;
  std::string service_name;
  std::string method_name;
  uint32_t args_size;

  if (rpcheader.ParseFromString(header_str) == true) {
    // 数据头反序列化成功
    service_name = rpcheader.service_name();
    method_name = rpcheader.method_name();
    args_size = rpcheader.args_size();
  } else {
    XLOG_ERROR( "rpcheader str : {} parse failed! ", header_str);

    return;
  }

  // step 3. 获取参数内容：args_str
  std::string args_str = recv_buf.substr(4 + header_size, args_size);

  // 打印调试信息
  XLOG_DEBUG("=================================");
  XLOG_DEBUG("header_size: {}" ,  header_size);
  XLOG_DEBUG("header_str: {}"  ,  header_str );
  XLOG_DEBUG("service_name: {}", service_name);
  XLOG_DEBUG("method_name: {}" ,  method_name);
  XLOG_DEBUG("args_str: {}"    ,     args_str);
  XLOG_DEBUG("==================================");

  // step 4. 获取service对象与method对象：通过上述步骤拿到的service_name 与 method_name
  // 在 NotifyService 方法中拿到的m_serviceMap与m_methodMap中获取service与method
  auto it = m_serviceMap.find(service_name);
  if (it == m_serviceMap.end()) {
    XLOG_ERROR("Service: {} is not exit!", service_name);
    return;
  }

  auto mit = it->second.m_methodMap.find(method_name);
  if (mit == it->second.m_methodMap.end()) {
    XLOG_ERROR("Method: {} is not exit!", method_name);
    return;
  }

  // step 6. 定义服务与方法描述,通过反序列化拿到的name获取service与method
  google::protobuf::Service *service = it->second.m_service;
  const google::protobuf::MethodDescriptor *method = mit->second;

  // step 7. 生成rpc方法调用的请求request调用 与 响应response调用
  google::protobuf::Message *request = service->GetRequestPrototype(method).New();
  if (!request->ParseFromString(args_str)) {
    XLOG_WARN("Request: {} parsed failed!", method_name);
  }

  google::protobuf::Message *response = service->GetResponsePrototype(method).New();

  // 给下面的method方法调用绑定一个Closure回调函数
  google::protobuf::Closure *done =
      google::protobuf::NewCallback<RpcProvider, const muduo::net::TcpConnectionPtr&, google::protobuf::Message *>(
          this, &RpcProvider::SendRpcResponse, conn, response);
  
  // step 8. rpc框架根据远端rpc请求，调用当前rpc节点上发布的方法
  service->CallMethod(method, nullptr, request, response, done);

}

// Closure的回调操作，用于序列化rpc的响应与muduo网络发送
void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr &conn,
                     google::protobuf::Message *response) {
  // 序列化response
  std::string response_str;
  if (response->SerializeToString(&response_str)) {
    conn->send(response_str);
  } else {
    XLOG_WARN("Response_str SerializeToString failed.");
  }
  conn->shutdown(); // 关闭写端（半关闭），模拟http的短链接服务，由rpcprovider主动断开链接
}



