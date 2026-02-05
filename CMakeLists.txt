cmake_minimum_required(VERSION 3.16)

project(canvaz VERSION 0.2.0 LANGUAGES CXX)

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

# Installation
install(TARGETS canvaz DESTINATION bin)
install(FILES resources/canvaz.desktop DESTINATION share/applications)
install(FILES docs/images/canvaz.svg DESTINATION share/icons/hicolor/scalable/apps)

# CPack Packaging
set(CPACK_PACKAGE_NAME "canvaz")
set(CPACK_PACKAGE_VENDOR "MaskedSyntax")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A modern wallpaper companion for Linux desktops")
set(CPACK_PACKAGE_VERSION "0.2.0")
set(CPACK_PACKAGE_CONTACT "MaskedSyntax")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_STRIP_FILES TRUE)

# DEB
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "MaskedSyntax")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6widgets6, libqt6gui6, libqt6core6, libqt6network6, libx11-6")

# RPM
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_GROUP "Utilities/System")
set(CPACK_RPM_PACKAGE_AUTOREQ ON)

set(CPACK_GENERATOR "DEB;RPM")

include(CPack)
target_include_directories(canvaz PRIVATE ${X11_INCLUDE_DIR})
