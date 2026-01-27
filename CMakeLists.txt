cmake_minimum_required(VERSION 3.16)

project(canvaz VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core Network)
find_package(X11 REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find all source files in src/
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")

add_executable(canvaz ${SOURCES} resources/resources.qrc)

target_link_libraries(canvaz PRIVATE Qt6::Widgets Qt6::Gui Qt6::Core Qt6::Network ${X11_LIBRARIES})
target_include_directories(canvaz PRIVATE ${X11_INCLUDE_DIR})
