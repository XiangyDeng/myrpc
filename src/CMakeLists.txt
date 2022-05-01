# 查找dir路径下的所有源文件，保存到variable(SRC_LIST)变量中，在lib目录下
# aux_source_directory(. SRC_LIST)
set(SRC_LIST 
../protobuf/rpcheader.pb.cc 
mprpcapplication.cc 
mprpcconfig.cc
rpcprovider.cc
mprpcchannel.cc
mprpccontroller.cc
zookeeperutil.cc)
include_directories(${PROJECT_SOURCE_DIR}/src/include)

# 将指定的源文件生成链接文件，然后添加到工程中，在lib目录下（写框架的时候使用）
# * 此处链接动态库 改为 链接静态库：muduo库编译为静态库.a 而静态库不可链接在动态库上，因此不使用SHARED
add_library(mprpc ${SRC_LIST})

# * 可在add的cmake 语句下链接库文件:此处为：静态库 链接 静态库
# 使用 target_link_libraries 应注意链接顺序（存在包含关系）
target_link_libraries(mprpc muduo_net muduo_base pthread zookeeper_mt)  # zk多线程版本