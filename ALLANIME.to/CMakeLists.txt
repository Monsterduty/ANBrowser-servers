cmake_minimum_required(VERSION 3.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

project("allanime")

find_package(nlohmann_json CONFIG REQUIRED)

add_executable(a.out main.cpp)

target_link_libraries( a.out Qt6Widgets Qt6Gui Qt6Core Qt6Network nlohmann_json )
