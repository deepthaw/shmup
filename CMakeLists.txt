cmake_minimum_required(VERSION 3.28.2)

project(shmup C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB REQUIRED glib-2.0)
pkg_search_module(LIBMIKMOD REQUIRED libmikmod)
find_package(tmx REQUIRED)
find_package(SDL2 REQUIRED)
find_package(LIBXML2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS} ${tmx_INCLUDE_DIRS} ${LIBMIKMOD_INCLUDE_DIRS})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
add_executable(shmup main.c sprites.c tmx_render.c fire.c sound.c actors.c graphics.c camera.c player.c)
add_custom_command(TARGET shmup POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink $<TARGET_FILE:shmup> ${CMAKE_SOURCE_DIR}/shmup)
target_link_libraries(shmup ${SDL2_LIBRARIES} ${LIBXML2_LIBRARIES} SDL2_image ${GLIB_LIBRARIES} ${LIBMIKMOD_LIBRARIES} tmx )
#add_compile_options(-std=gnu11)

