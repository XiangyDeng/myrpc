#include "mprpcchannel.h"
#include "mprcplogger.h"
#include "rpcheader.pb.h"
#include <cstdint>
#include <cstdlib>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "mprpcapplication.h"
#include "mprpcconfig.h"
#include "zookeeperutil.h"

// protobuf数据格式：header_size + service_name + method_name args_size + args
// * 所有通过stub（caller）对象调用的rpc方法，都走到了这个函数，统一做rpc方法调用的数据序列化与网络发送
// 然后进入阻塞状态等待request
void MpRpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                google::protobuf::RpcController *controller,
                const google::protobuf::Message *request,
                google::protobuf::Message *response,
                google::protobuf::Closure *done) { // 调用方 回调函数没有用处

  // ! step 0. 定义了controller对象
  
  // step 1. 获取method对应的service， 取得service_name、method_name、
  const google::protobuf::ServiceDescriptor *sd = method->service();
  std::string service_name = sd->name();
  std::string method_name = method->name();

  // step 2. 获取序列化长度args_size
  if (controller->Failed()) {
    XLOG_WARN("{}", controller->ErrorText());
  } else {
    uint32_t args_size = 0;
    std::string args_str;
    if (request->SerializeToString(&args_str)) {
      args_size = args_str.size();
    } else {
      controller->SetFailed("request SerializeToString error!");  // 改用controller
      return;
    }

    // step 3. 定义rpc的header: 
    mprpc::RpcHeader rpcheader;
    rpcheader.set_service_name(service_name);
    rpcheader.set_method_name(method_name);
    rpcheader.set_args_size(args_size);

    uint32_t header_size = 0;
    std::string rpc_header_str;
    if (rpcheader.SerializeToString(&rpc_header_str)) {
      header_size = rpc_header_str.size();
    } else {
      controller->SetFailed("rpcheader SerializeToString error!");
      return;
    }

    // step 4. 组织待发送的rpc请求字符串
    std::string send_rpc_str;
    send_rpc_str.insert(0, std::string((char*)&header_size, 4)); // header_size
    send_rpc_str += rpc_header_str;
    send_rpc_str += args_str;

      // 打印调试信息
    XLOG_DEBUG("=================================");
    XLOG_DEBUG("header_size: {}" ,     header_size);
    XLOG_DEBUG("header_str: {}"  , rpc_header_str );
    XLOG_DEBUG("service_name: {}",    service_name);
    XLOG_DEBUG("method_name: {}" ,     method_name);
    XLOG_DEBUG("args_str: {}"    ,        args_str);
    XLOG_DEBUG("=================================");

    // step 5. 使用tcp原生api编程，完成rpc方法的远程调用
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1) {
      std::string context = "create sock failed! errno!" + std::to_string(errno);
      controller->SetFailed(context);
      return;
    }

    // * 不再需要从配置文件中读取 固定 的 服务端方法的ip地址与port端口号
    // // 读取配置文件信息（rpcserver）: rpcserver_ip 与 rpcserver_port
    // //std::string rpcserver_ip = MprpcApplication::getInstance().GetMprpcConfig().Load("rpcserver_ip");
    // //uint32_t rpcserver_port = atoi(MprpcApplication::getInstance().GetMprpcConfig().Load("rpcserver_port").c_str());
    // ! 若rpc调用方想要调用service_name的method_name，则需要
    // ! 使用zookeeper查询对应服务所在的host信息
    ZkClient zkclient;
    zkclient.Start(); // zkclient初始化，连接zkServer（端口为2181）

    // 从path ——> host
    std::string method_path = "/" + service_name + "/" + method_name;
    std::string host_data = zkclient.GetData(method_path.c_str());

    if (host_data == "") {
      controller->SetFailed(method_path + "is not exist!");
      exit(EXIT_FAILURE);
    }

    int index = host_data.find(':');
    if (index == host_data.npos) {
      controller->SetFailed(method_path + " address is invalid!");
      exit(EXIT_FAILURE);
    }
    std::string rpcserver_ip = host_data.substr(0, index);
    // TODO index 有没有错
    uint32_t rpcserver_port =
        atoi(host_data.substr(index + 1, host_data.size() - index - 1).c_str());
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(rpcserver_port);
    inet_pton(AF_INET, rpcserver_ip.c_str(), &server_addr.sin_addr);

    // 连接rpc服务节点
    XLOG_INFO("Start connecting server -> ip:port {}:{}", rpcserver_ip, rpcserver_port);
    
    if (connect(clientfd, (struct sockaddr *)&server_addr, sizeof server_addr) == -1) {
      std::string context = "clientfd connected error! errno: " + std::to_string(errno);
      controller->SetFailed(context);
      close(clientfd);
      return;
    }

    XLOG_INFO("Connected to server -> ip:port {}:{}", rpcserver_ip, rpcserver_port);

    // 发送rpc请求
    if (send(clientfd, send_rpc_str.c_str(), send_rpc_str.size(), 0) == -1) {
      std::string context = "clientfd send error! errno: " + std::to_string(errno);
      controller->SetFailed(context);
      close(clientfd);
      return;
    }

    // 接收rpc请求的响应值:阻塞等待
    char recv_buf[1024] = {0};
    uint32_t recv_size = 0;
    if ((recv_size = recv(clientfd, recv_buf, 1024, 0)) == -1) {
      std::string context = "clientfd recv error! errno: " + std::to_string(errno);
      controller->SetFailed(context);
      close(clientfd);
      return;
    }

    // step 6. 通知caller应用程序:填入response
    // 反序列化rpc调用的响应数据(对端发来)
    // ! string 用char 初始化，遇到\0就结束，导致反序列化失败
    // // std::string response_str(recv_buf + 0, recv_size);
    if (!response->ParseFromArray(recv_buf,recv_size)) {
      controller->SetFailed("response ParseFromArray error!");
      close(clientfd);
      return;
    }

    close(clientfd);
  }

}