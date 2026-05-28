file(GLOB_RECURSE SRC_FILES "*.cpp" "rpcheader.pb.cc" include/*.h "include/rpcheader.pb.h")
add_library(rpc ${SRC_FILES})
target_link_libraries(rpc muduo_net muduo_base pthread protobuf zookeeper_mt)