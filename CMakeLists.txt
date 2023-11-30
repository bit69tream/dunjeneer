cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+
project(dunjeneer)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 23)
set(C_STANDARD_REQUIRED ON)

# Dependencies
set(RAYLIB_VERSION 5.0)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
  endif()
endif()

add_executable(${PROJECT_NAME}
  source/game.c
  source/level_generator.c
  source/rand.c
  source/font.c
  source/player.c
  source/ui.c
  source/audio.c
  source/config.c)
set(raylib_VERBOSE 1)
target_link_libraries(${PROJECT_NAME} raylib)

if(CMAKE_BUILD_TYPE STREQUAL Release)
  target_compile_definitions(${PROJECT_NAME} PRIVATE NO_RAYLIB_INFO)
  target_compile_options(${PROJECT_NAME} PRIVATE -O2)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -O0 -ggdb)
endif()

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror -Wconversion -ggdb)
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
