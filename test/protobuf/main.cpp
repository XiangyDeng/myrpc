#include "test.pb.h"

#include <iostream> 
#include <string>

using namespace fixbug;

int main() {

  // 序列化：对象 -> 二进制
  // 反序列化：二进制 -> 对象
  GetFriendListsResponse rsp;
  ResultCode *rc = rsp.mutable_result();  // 获取 对象类型，不能直接获取
  rc->set_errcode(0);
  

  User *user1 = rsp.add_friend_list();  // 获取 列表类型s
  user1->set_name("Shang san");
  user1->set_age(20);
  user1->set_sex(User::MAN);

  User *user2 = rsp.add_friend_list();
  user2->set_name("li si");
  user2->set_age(22);
  user2->set_sex(User::MAN);

  std::cout << user1->name() << std::endl;

  return 0;  
}