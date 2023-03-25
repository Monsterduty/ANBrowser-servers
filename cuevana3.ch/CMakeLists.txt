cmake_minimum_required( VERSION 3.0 )
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project( CUEVANA3.CH )

find_package( Qt6 )
link_libraries( Qt6Core Qt6Widgets Qt6Network )

add_executable( a.out main.cpp networking.hpp networking.cpp search.hpp search.cpp play.hpp play.cpp fetch.hpp fetch.cpp)

target_link_libraries( a.out Qt6Core Qt6Network Qt6Widgets )
