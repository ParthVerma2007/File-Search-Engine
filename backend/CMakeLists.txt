cmake_minimum_required(VERSION 3.20)
project(MiniSearchEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(SearchEngine ${SOURCES})

target_include_directories(SearchEngine PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

if(WIN32)
    target_link_libraries(SearchEngine PRIVATE ws2_32)
endif()
