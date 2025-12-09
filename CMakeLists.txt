cmake_minimum_required(VERSION 3.20)
project(pingtrace LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include_directories(include)

file(GLOB_RECURSE SOURCES "src/*.c")

# add winsock library for Windows
if (WIN32)
    set(WINSOCK_LIB ws2_32)
    list(APPEND EXTRA_LIBS ${WINSOCK_LIB})
endif()

# Create the executable
add_executable(pingtrace ${SOURCES})

# only used for pingtrace executable
target_include_directories(pingtrace PRIVATE include)

if (MSVC)
    # MSVC (Visual Studio) flags
    target_compile_options(pingtrace PRIVATE 
        /W4          # Enable warning level 4 (strong warnings)
        # /WX          # Treat warnings as errors
    )
else()
    # GCC/Clang flags
    target_compile_options(pingtrace PRIVATE 
        -Wall 
        -Wextra 
        -Wpedantic 
        -Werror
    )
endif()

set_target_properties(pingtrace PROPERTIES OUTPUT_NAME "pingtrace")