#    Copyright 2026 François Aucoin
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.

set(MY_NAMESPACE_PREFIX LC3Tools)                                   # This is the project name, but also used as prefix at several places.
set(MY_INSTALLATION_TARGETS_NAME "${MY_NAMESPACE_PREFIX}Targets")   # This is used by the targets for the installation-related configurations.
set(MY_INCLUDE_PREFIX_DIR_NAME lc3tools)                            # Used as prefix directory (for the installation)

set(MY_LIB_NAME_BACKEND backend)                                    # The name of the "backend" library
set(MY_LIB_NAME_COMMON common)                                      # The name of the "common" library
set(MY_LIB_NAME_TESTING_FRAMEWORK_COMMON testing_framework_common)  # The name of the "testing framework common" library
set(MY_LIB_NAME_TESTING_FRAMEWORK_V1 testing_framework_v1)          # The name of the "testing framework (version 1)" library
set(MY_LIB_NAME_TESTING_FRAMEWORK_V2 testing_framework_v2)          # The name of the "testing framework (version 2)" library

set(MY_BIN_NAME_ASSEMBLER assembler)
set(MY_BIN_NAME_SIMULATOR simulator)

cmake_minimum_required(VERSION 3.31.4)                              # This version requirement is somewhat arbitrary, but it's what I was using on my development machine when I put together this build system.
project(
    ${MY_NAMESPACE_PREFIX}
    VERSION 1.0.0                                                   # Project version 1.0.0 here corresponds to LC3Tools (the C++ code) [release 2.1.0](https://github.com/chiragsakhuja/lc3tools/releases/tag/v2.1.0)
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

add_compile_options(
    -Wall
    -Wextra
    -Werror
    -Wpedantic
    
    # Given the use of `-Werror`, the following flags are needed
    # for the un-modified project to compile on Apple's Clang 21
    # (and likely as well when using other modern compilers).
    -Wno-format-security 
)
add_link_options($<$<CONFIG:Debug>:-fsanitize=address,undefined>)

if (EMSCRIPTEN)
    if (LC3_TOOLS_ENABLE_WASM_64_BIT)
        add_link_options("--target=wasm64")
        add_compile_options("--target=wasm64")
    else()
        add_link_options("--target=wasm32")
        add_compile_options("--target=wasm32")    
    endif()
endif()

add_subdirectory(src)

install(
    EXPORT "${MY_INSTALLATION_TARGETS_NAME}"
    NAMESPACE "${MY_NAMESPACE_PREFIX}::"
    FILE "${MY_INSTALLATION_TARGETS_NAME}.cmake"
    DESTINATION "lib/cmake/${MY_NAMESPACE_PREFIX}"
)

file(
    WRITE
    "${CMAKE_CURRENT_BINARY_DIR}/${MY_NAMESPACE_PREFIX}Config.cmake"
    "include(\"\${CMAKE_CURRENT_LIST_DIR}/${MY_INSTALLATION_TARGETS_NAME}.cmake\")"
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/${MY_NAMESPACE_PREFIX}Config.cmake"
    DESTINATION "lib/cmake/${MY_NAMESPACE_PREFIX}"
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${MY_NAMESPACE_PREFIX}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY ExactVersion
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/${MY_NAMESPACE_PREFIX}ConfigVersion.cmake"
    DESTINATION "lib/cmake/${MY_NAMESPACE_PREFIX}"
)

# message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")