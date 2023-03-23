project(ThreadPool)

# create library
add_library(threadPool STATIC ThreadPool.cpp)

# add thread specific libs
find_package(Threads)
target_link_libraries(threadPool ${CMAKE_THREAD_LIBS_INIT})

# add common funcs
target_include_directories(threadPool PUBLIC "${Project_root}/Common")
target_link_libraries(threadPool common)
