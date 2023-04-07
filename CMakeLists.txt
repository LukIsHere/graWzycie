cmake_minimum_required(VERSION 3.14)
project(graWzycie)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(graWzycie src/main.cpp)

include_directories(graWzycie include)



if (WIN32)

endif (WIN32)

if (UNIX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")#mniejsza binarka
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto")#mniejsza binarka

target_link_libraries(graWzycie X11 pthread)
endif (UNIX)



