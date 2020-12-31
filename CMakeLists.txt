cmake_minimum_required(VERSION 3.6)

project(turbo-button LANGUAGES CXX C)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

include(GNUInstallDirs)
set(LIBEXEC_PATH "${CMAKE_INSTALL_FULL_LIBDIR}/turbo-button/")
add_definitions(-DLIBEXEC_DIR=\"${LIBEXEC_PATH}\")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_executable(turbo-button turbo-button.cpp turbo-button.h icons.qrc)

target_link_libraries(turbo-button PRIVATE
    Qt5::Widgets
    )

add_executable(toggler toggler.c)

install(TARGETS turbo-button DESTINATION bin)
install(TARGETS toggler
    DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/turbo-button/"
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETUID
    )

install(FILES turbo-button.desktop DESTINATION /etc/xdg/autostart/)
