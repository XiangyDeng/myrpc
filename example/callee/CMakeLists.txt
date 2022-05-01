set(USER_SRC_LIST userservice.cc ../user.pb.cc)
set(FRIEND_SRC_LIST friendservice.cc ../friend.pb.cc)

# 在bin目录下, 两个rpc服务
add_executable(userservice_provider ${USER_SRC_LIST})
target_link_libraries(userservice_provider 
${PROJECT_NAME}               # rpc框架库文件
/usr/local/lib/libprotobuf.so # protobuf库文件
)

add_executable(friendservice_provider ${FRIEND_SRC_LIST})
target_link_libraries(friendservice_provider 
${PROJECT_NAME}               # rpc框架库文件
/usr/local/lib/libprotobuf.so # protobuf库文件
)