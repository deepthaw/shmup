cmake_minimum_required(VERSION 3.28.2)

project(shmup C)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB REQUIRED glib-2.0)
find_package(SDL2 REQUIRED)
find_package(LIBXML2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS})

add_executable(shmup main.c tiles.c sprites.c map.c strdup.c)
target_link_libraries(shmup ${SDL2_LIBRARIES} ${LIBXML2_LIBRARIES} SDL2_image ${GLIB_LIBRARIES})
