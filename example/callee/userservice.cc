#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>

#include "../user.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"

/**
 * @brief UserService 原来是一个本地业务，提供了两个进程内的本地方法，Login 与 GetFriendLists
 *        改为rpc远程调用服务 
 */
class UserService : public fixbug::UserServiceRpc { // ! 继承后为 使用rpc服务器发布端（rpc服务提供者）
public:
  // 在其他机器到其他进程也可调用: 服务端本地服务
  bool Login(std::string name, std::string pwd) {
    std::cout << "doing local service: Login" << std::endl;

    if (m_userInfo.count(name)) {
      if (m_userInfo[name] == pwd) {
        std::cout << "[Login account successful] -- \nname: " << name << "\npassword: " << pwd << std::endl;
      } else {
        std::cout << "user password is wrong! " << std::endl;
        return false;
      }
      
    } else {
      std::cout << "username is not exit! " << std::endl;
      return false;
    }

    return true;
  }

  bool Register(uint32_t id, std::string name, std::string pwd) {
    std::cout << "doing local service: Register" << std::endl;
    std::cout << "[Register account] -- \nid:" << id << "\nname: " << name << "\npassword: " << pwd
              << std::endl;

    m_userInfo[name] = pwd;
    m_userID[id] = name;

    return true;
  }

  /**
   * @brief （业务代码，非框架，框架自动调用）重写fixbug::UserServiceRpc基类基函数，下面方法都是框架直接调用
   *        ! 1. caller => Login(Request) => muduo =>callee
   *        ! 2. callee => Login(Request) => 交到下面重写的Login方法上 -> response -> run()
   *    
   * @param controller 
   * @param request     ：传入请求信息
   * @param response    ：返回业务处理结果
   * @param done        ：执行回调
   */
  void Login(::google::protobuf::RpcController *controller,
             const ::fixbug::LoginRequest *request,
             ::fixbug::LoginResponse *response,
             ::google::protobuf::Closure *done) {
    // step 1. 框架给业务上报了LoginRequest， 应用取出对应的事件做 本地业务
    std::string name = request->name();
    std::string passwd = request->pwd();

    // step 2. 处理本地业务
    bool login_result = Login(name, passwd);

    // step 3. response：包括错误码，错误信息，返回值
    fixbug::ResultCode *code = response->mutable_result();
    code->set_errcode(0); // 假定不存在错误
    code->set_errmsg("");
    response->set_success(login_result);

    // step 4. 执行回调函数(基类中为纯虚函数)：执行响应数据的序列化和网络发送（都是由框架完成的）
    done->Run();
  }

  void Register(::google::protobuf::RpcController *controller,
                const ::fixbug::RegisterRequest *request,
                ::fixbug::RegisterResponse *response,
                ::google::protobuf::Closure *done) {

    uint32_t id = request->id();
    std::string name = request->name();
    std::string pwd = request->pwd();

    bool register_result = Register(id, name, pwd);

    fixbug::ResultCode *code = response->mutable_result();
    code->set_errcode(0);
    code->set_errmsg("");
    response->set_success(register_result);

    done->Run();
  }

private:
  std::unordered_map<std::string, std::string> m_userInfo;
  std::unordered_map<uint32_t, std::string> m_userID;
  
};

// rpc服务器端的业务代码
int main(int argc, char **argv) {
  // 本地调用
  // UserService us;
  // us.Login("melody", "123");

  // 远端调用
  // step 1. 调用框架初始化操作 provider -i config.conf, 只用初始化一次, 获得配置变量
  MprpcApplication::Init(argc, argv);

  // step 2. 将UserService对象（业务代码）发布到rpc节点上：由框架调用对应的应用程序
  RpcProvider provider;
  provider.NotifyService(new UserService());

  // step 3. 启动一个rpc服务发布节点, Run以后，进程进入阻塞状态，等待远程的rpc调用请求
  provider.run();

  return 0;
}