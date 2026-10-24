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

set(MY_LIB_NAME ${MY_LIB_NAME_TESTING_FRAMEWORK_COMMON})

set(MY_SOURCE_FILES
    "src/framework_common.cpp"
)

add_library(${MY_LIB_NAME} ${MY_SOURCE_FILES})

set_target_properties(${MY_LIB_NAME} PROPERTIES
    OUTPUT_NAME "${MY_INCLUDE_PREFIX_DIR_NAME}_${MY_LIB_NAME}"
)

target_link_libraries(${MY_LIB_NAME} PRIVATE
    ${MY_LIB_NAME_BACKEND}
)

target_include_directories(${MY_LIB_NAME}
    PRIVATE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> # This would be for private headers/interfaces
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/${MY_INCLUDE_PREFIX_DIR_NAME}/${MY_LIB_NAME}> # This is so we are able to use flat resolution inside this library.
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

install(
    TARGETS ${MY_LIB_NAME}
    EXPORT ${MY_INSTALLATION_TARGETS_NAME}
    ARCHIVE DESTINATION lib
)

install(
    DIRECTORY include/
    DESTINATION include
    FILES_MATCHING
    PATTERN "*.h"
)