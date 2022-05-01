#include <iostream>
#include "mprpcapplication.h"
#include "mprpcchannel.h"
#include "user.pb.h"

int main(int argc, char **argv) {

  // step 1. 调用rpc基本框架 解析命令行指令(-i) -> 读取配置文件（四元数）
  MprpcApplication::Init(argc, argv);

  // step 2. 演示调用远程发布的rpc方法：Register() -> Login() -> GetFriendsList()
  fixbug::UserServiceRpc_Stub stub(new MpRpcChannel());

  // 1. Register account
  fixbug::RegisterRequest register_request;
  register_request.set_id(1);
  register_request.set_name("zhang san");
  register_request.set_pwd("123456");

  fixbug::RegisterResponse register_response;
  // 发起rpc方法调用（同步的rpc调用过程）: MprpcChannel::CallMethod() -> 执行Register
  stub.Register(nullptr, &register_request, &register_response, nullptr);

  if (0 == register_response.result().errcode()) {
     std::cout << "rpc register response success:" << register_response.success()
              << "\n\n" << std::endl;
  } else {
    std::cout << "rpc login response error:" << register_response.result().errmsg()
              << "\n\n" << std::endl;
  }

  // 2.  Login account
  fixbug::LoginRequest login_request;
  login_request.set_name("zhang san");
  login_request.set_pwd("123456");

  fixbug::LoginResponse login_response;
  // 发起rpc方法调用（同步的rpc调用过程）: MprpcChannel::CallMethod()
  stub.Login(nullptr,&login_request,&login_response,nullptr); // RpcChannel -> RpcChannel::callMessage() : 集中处理rpc方法调用的参数序列化与网络传输

  // 判断rpc调用结果
  if (0 == login_response.result().errcode()) {
    std::cout << "rpc login response success:" << login_response.success()
              << std::endl;
  } else {
    std::cout << "rpc login response error:" << login_response.result().errmsg()
              << std::endl;
  }
  
  return 0;
}
