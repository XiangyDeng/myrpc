#pragma once

#include <google/protobuf/service.h>

class MpRpcChannel : public google::protobuf::RpcChannel {
public:
  // * 所有通过stub（caller）对象调用的rpc方法，都走到了这个函数，统一做rpc方法调用的数据序列化与网络发送
  // 然后进入阻塞状态等待request
  void CallMethod(const google::protobuf::MethodDescriptor *method,
                  google::protobuf::RpcController *controller,
                  const google::protobuf::Message *request,
                  google::protobuf::Message *response,
                  google::protobuf::Closure *done); 

};