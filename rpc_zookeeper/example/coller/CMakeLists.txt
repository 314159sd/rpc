file(GLOB_RECURSE SRC_FILES "*.cpp" "../user.pb.cc")

find_package(fmt REQUIRED)

add_executable(coller ${SRC_FILES})
target_link_libraries(coller protobuf rpc fmt::fmt)
