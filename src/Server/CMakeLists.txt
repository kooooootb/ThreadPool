project(Server)

add_library(server STATIC Server.cpp)

# add threadpool class
add_subdirectory(ThreadPool)
target_include_directories(server PUBLIC ThreadPool)
target_link_libraries(server threadPool)

# add common functions and structs
target_include_directories(server PUBLIC "${Project_root}/Common")
target_link_libraries(server common)
