cmake_minimum_required(VERSION 3.5)
project(spdft C)

set(C_STANDARD 99)
set(C_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Create an option to switch between a system sdl library and a vendored SDL library
option(MYGAME_VENDORED "Use vendored libraries" ON)

if(MYGAME_VENDORED)
    # This assumes you have added SDL as a submodule in vendored/SDL
    add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
else()
    # 1. Look for a SDL3 package,
    # 2. look for the SDL3-shared component, and
    # 3. fail if the shared component cannot be found.
    find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
endif()


add_subdirectory(spike)

# Create your game executable target as usual
add_executable(spdft main.c)


# We want to include the current directory for headers like a.h and utils.h
target_include_directories(${PROJECT_NAME}
    PUBLIC spike
)

target_link_directories(${PROJECT_NAME} PRIVATE spike)

# Link to the actual SDL3 library.
target_link_libraries(spdft PRIVATE SDL3::SDL3 spike)
