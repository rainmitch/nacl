

cmake_minimum_required (VERSION 3.1)

project (nacl)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

include_directories(src/headers)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++14")

file(GLOB SRC
    "src/*.cpp"
)

add_executable(nacl ${SRC})

target_link_libraries (nacl)
