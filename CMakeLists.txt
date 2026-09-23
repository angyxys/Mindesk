cmake_minimum_required(VERSION 3.16)

project(mindesk
    VERSION 0.1.0
    DESCRIPTION "Compositor Wayland con tiling desplazable estilo Niri"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(
    -Wall
    -Wextra
    -Wno-unused-parameter
)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

find_package(PkgConfig REQUIRED)

pkg_check_modules(WLROOTS IMPORTED_TARGET wlroots-0.19)
if(NOT WLROOTS_FOUND)
    pkg_check_modules(WLROOTS IMPORTED_TARGET wlroots)
endif()
if(NOT WLROOTS_FOUND)
    message(FATAL_ERROR "wlroots no encontrado.")
endif()

pkg_check_modules(WAYLAND_SERVER    REQUIRED IMPORTED_TARGET wayland-server)
pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED IMPORTED_TARGET wayland-protocols)
pkg_check_modules(WAYLAND_SCANNER   REQUIRED IMPORTED_TARGET wayland-scanner)

execute_process(
    COMMAND pkg-config --variable=pkgdatadir wayland-protocols
    OUTPUT_VARIABLE WAYLAND_PROTOCOLS_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND pkg-config --variable=wayland_scanner wayland-scanner
    OUTPUT_VARIABLE WAYLAND_SCANNER_EXEC
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(PROTOCOL_DIR ${CMAKE_CURRENT_BINARY_DIR}/protocols)
file(MAKE_DIRECTORY ${PROTOCOL_DIR})

set(XDG_SHELL_XML    ${WAYLAND_PROTOCOLS_DIR}/stable/xdg-shell/xdg-shell.xml)
set(XDG_SHELL_HEADER ${PROTOCOL_DIR}/xdg-shell-protocol.h)
set(XDG_SHELL_CODE   ${PROTOCOL_DIR}/xdg-shell-protocol.c)

add_custom_command(
    OUTPUT ${XDG_SHELL_HEADER}
    COMMAND ${WAYLAND_SCANNER_EXEC} server-header
            ${XDG_SHELL_XML} ${XDG_SHELL_HEADER}
    DEPENDS ${XDG_SHELL_XML}
    COMMENT "Generando xdg-shell-protocol.h"
)

add_custom_command(
    OUTPUT ${XDG_SHELL_CODE}
    COMMAND ${WAYLAND_SCANNER_EXEC} private-code
            ${XDG_SHELL_XML} ${XDG_SHELL_CODE}
    DEPENDS ${XDG_SHELL_XML}
    COMMENT "Generando xdg-shell-protocol.c"
)

add_executable(mindesk
    src/main.cpp
    src/server.cpp
    ${XDG_SHELL_CODE}
)

add_custom_target(mindesk_protocols ALL
    DEPENDS ${XDG_SHELL_HEADER} ${XDG_SHELL_CODE}
)
add_dependencies(mindesk mindesk_protocols)

target_include_directories(mindesk PRIVATE
    src
    ${PROTOCOL_DIR}
)

target_compile_definitions(mindesk PRIVATE
    WLR_USE_UNSTABLE
)

target_link_libraries(mindesk PRIVATE
    PkgConfig::WLROOTS
    PkgConfig::WAYLAND_SERVER
)

install(TARGETS mindesk RUNTIME DESTINATION bin)
