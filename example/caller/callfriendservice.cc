#include <iostream> // 调用方所使用的头文件
#include "mprpcapplication.h"
#include "mprpcchannel.h"
#include "friend.pb.h"
#include "mprpccontroller.h"

int main(int argc, char **argv) {

  MprpcApplication::Init(argc, argv);

  fixbug::FriendsServiceRpc_Stub stub(new MpRpcChannel());

  fixbug::GetFriendsListRequest request;
  request.set_userid(1);

  fixbug::GetFriendsListResponse response;
  MprpcController controller;
  stub.GetFriendsList(&controller, &request, &response, nullptr);

  if (0 == response.mutable_result()->errcode()) {
    std::cout << "rpc register response success, friends list is:" << std::endl;
    int size = response.friends_size();
    for (int i = 0; i < size; ++i) {
      std::cout << "index: " << i + 1 << ", name: " << response.friends(i)
                << std::endl;
      
    }
  } else {
    std::cout << "rpc login response error:" << response.result().errmsg()
              << "\n\n" << std::endl;
  }

  return 0;
}