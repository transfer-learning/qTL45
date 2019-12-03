cmake_minimum_required(VERSION 3.5)

project(tl45_emu)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

include_directories({CMAKE_BINARY_DIR}/tl45_emu_autogen/include)

add_executable(tl45_emu
        main.cpp
        TL45RegisterListModel.cpp
        tl45emu.cpp
        tl45emu.h
        tl45emu.ui
        )

target_link_libraries(tl45_emu PRIVATE Qt5::Widgets)
