cmake_minimum_required(VERSION 3.10)
project(ClickWise)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(X11 REQUIRED x11)
pkg_check_modules(XTEST REQUIRED xtst)

include_directories(${GTK3_INCLUDE_DIRS} ${X11_INCLUDE_DIRS} ${XTEST_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS} ${X11_LIBRARY_DIRS} ${XTEST_LIBRARY_DIRS})

add_executable(clickwise
    src/main.cpp
    src/AutoClicker.cpp
    src/ConfigManager.cpp
    src/InputHandler.cpp
    src/ApplicationFilter.cpp
    src/UI.cpp
)

target_link_libraries(clickwise
    ${GTK3_LIBRARIES}
    ${X11_LIBRARIES}
    ${XTEST_LIBRARIES}
    pthread
)

target_compile_options(clickwise PRIVATE ${GTK3_CFLAGS_OTHER})