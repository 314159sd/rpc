file(GLOB_RECURSE SRC_FILES "*.cpp" "../user.pb.cc")

add_executable(userserver ${SRC_FILES})
target_link_libraries(userserver protobuf rpc)
