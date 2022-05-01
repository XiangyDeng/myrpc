set(USER_SRC_LIST calluserservice.cc ../user.pb.cc)
set(FRIEND_SRC_LIST callfriendservice.cc ../friend.pb.cc)

add_executable(userservice_consumer ${USER_SRC_LIST})
target_link_libraries(userservice_consumer 
${PROJECT_NAME}               # rpc框架库文件
/usr/local/lib/libprotobuf.so # protobuf库文件
)

add_executable(friendservice_consumer ${FRIEND_SRC_LIST})
target_link_libraries(friendservice_consumer 
${PROJECT_NAME}               # rpc框架库文件
/usr/local/lib/libprotobuf.so # protobuf库文件
)