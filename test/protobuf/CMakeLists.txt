cmake_minimum_required(VERSION 3.0)
project(test)

# 头文件
include_directories(./test.pb.h)

# 源文件
add_library(${PROJECT_NAME} SHARED 
./test.pb.cc)

# 可执行文件
add_executable(mytest ./main.cpp)

# 链接动态库
target_link_libraries(mytest ${PROJECT_NAME}
/usr/local/lib/libprotobuf.so
/usr/lib/x86_64-linux-gnu/libprotobuf.so)


