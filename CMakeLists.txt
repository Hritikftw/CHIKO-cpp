cmake_minimum_required(VERSION 3.14)
project(CHIKO CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Server ----
add_executable(ChatServer
    server/ChatServer.cpp
)
target_include_directories(ChatServer PRIVATE server)

if(WIN32)
    target_link_libraries(ChatServer ws2_32)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(ChatServer Threads::Threads)
endif()

# ---- Client ----
add_executable(ChatClient
    client/ChatClient.cpp
)

if(WIN32)
    target_link_libraries(ChatClient ws2_32)
endif()
