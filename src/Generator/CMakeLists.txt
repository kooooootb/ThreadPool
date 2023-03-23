project(Generator)

add_library(generator STATIC Generator.cpp)

# add common functions and structs
target_include_directories(generator PUBLIC "${Project_root}/Common")
target_link_libraries(generator common)

# add threads libs
find_package(Threads)
target_link_libraries(threadPool ${CMAKE_THREAD_LIBS_INIT})
