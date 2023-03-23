cmake_minimum_required(VERSION 3.18)
project(main)

set(CMAKE_CXX_STANDARD 17)

# explicitly set project root
set(Project_root ${CMAKE_CURRENT_LIST_DIR})

# runs server app
add_executable(runServer server.cpp)
# runs generators
add_executable(runGenerator generator.cpp)

add_subdirectory(Common)

# get server class from folder
add_subdirectory(Server)
target_include_directories(runServer PUBLIC "${Project_root}/Server")
target_link_libraries(runServer server)

# get generator class from folder
add_subdirectory(Generator)
target_include_directories(runGenerator PUBLIC "${Project_root}/Generator")
target_link_libraries(runGenerator generator)
