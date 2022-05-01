#include <cstdint>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

#include "../friend.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"

// * rpc框架自动调用本地方法:传入对象在rpc节点服务器中调用本地方法进行处理，在序列化反序列化将结果response回调用方
class FriendService : public fixbug::FriendsServiceRpc {
public:
  std::vector<std::string> GetFriendsList(uint32_t userid) {
    std::cout << "do GetFriendsList service: " << "GetFriendsList" << std::endl;

    m_vec.emplace_back("zhang yi");
    m_vec.emplace_back("zhang er");
    m_vec.emplace_back("zhang san");
    m_vec.emplace_back("zhang si");
    
    return m_vec;
  }

  void GetFriendsList(::google::protobuf::RpcController *controller,
                      const ::fixbug::GetFriendsListRequest *request,
                      ::fixbug::GetFriendsListResponse *response,
                      ::google::protobuf::Closure *done) {
    // step 1. rpc服务器框架给服务器业务上报对端的Request，应用业务取出对应的数据在本地方法做业务
    uint32_t userid = request->userid();

    // step 2. 进行本地业务
    auto getfriendslist_vec = GetFriendsList(userid);

    // step 3. response：包括错误码，错误信息，返回值
    fixbug::ResultCode *code = response->mutable_result();
    code->set_errcode(0);
    code->set_errmsg("");

    for (const auto str:getfriendslist_vec){
      std::string *p = response->add_friends();
      *p = str;
    }

    // step 4. 执行响应数据的序列化和网络发送（都是由rpc框架完成的）,序列化后发送回服务调用方
    done->Run();
  }

private:
  std::vector<std::string> m_vec;
};

// rpc节点服务端主程序
int main(int argc, char **argv) {

  // step 1. 读取命令行中的配置文件
  MprpcApplication::Init(argc, argv);

  // step 2. 在rpc服务器上发布方法（FriendService）
  RpcProvider provider;
  provider.NotifyService(new FriendService());

  // step 3. 启动rpc服务发布节点，等待远端rpc调用请求
  provider.run();

  return 0;
}