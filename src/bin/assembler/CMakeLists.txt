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

set(MY_BIN_NAME ${MY_BIN_NAME_ASSEMBLER})
set(MY_RUN_SCRIPT_NAME "run-${MY_BIN_NAME}.mjs")

add_executable(${MY_BIN_NAME} main.cpp)

set_target_properties(${MY_BIN_NAME} PROPERTIES
    OUTPUT_NAME "${MY_INCLUDE_PREFIX_DIR_NAME}-${MY_BIN_NAME}"
)

target_link_libraries(${MY_BIN_NAME} PRIVATE
     ${MY_LIB_NAME_BACKEND}
     ${MY_LIB_NAME_COMMON}
)

if (EMSCRIPTEN)
    target_compile_options(${MY_BIN_NAME} PRIVATE
        "-fwasm-exceptions"
    )

    target_link_options(${MY_BIN_NAME} PRIVATE 
        "-sENVIRONMENT=node"
        "-sMODULARIZE=1"
        "-sEXPORT_ES6=1"
        "-sFORCE_FILESYSTEM=1"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sEXPORTED_RUNTIME_METHODS=FS"
        "-fwasm-exceptions"
    )

    set_target_properties(${MY_BIN_NAME} PROPERTIES
        SUFFIX ".mjs"
    )
endif()

install(
    TARGETS ${MY_BIN_NAME}
    EXPORT ${MY_INSTALLATION_TARGETS_NAME}
    RUNTIME DESTINATION bin
)

if (EMSCRIPTEN)
    add_custom_command(
        TARGET ${MY_BIN_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_SOURCE_DIR}/${MY_RUN_SCRIPT_NAME}"
            "$<TARGET_FILE_DIR:${MY_BIN_NAME}>/${MY_INCLUDE_PREFIX_DIR_NAME}-${MY_RUN_SCRIPT_NAME}"
    )

    install(
        FILES 
            "$<TARGET_FILE_DIR:${MY_BIN_NAME}>/$<TARGET_FILE_BASE_NAME:${MY_BIN_NAME}>.wasm"
        DESTINATION bin
    )
    install(
        FILES 
            "${CMAKE_CURRENT_SOURCE_DIR}/${MY_RUN_SCRIPT_NAME}"
        DESTINATION bin
        RENAME "${MY_INCLUDE_PREFIX_DIR_NAME}-${MY_RUN_SCRIPT_NAME}"
    )
endif()

if (EMSCRIPTEN)
    add_test(
        NAME ${MY_BIN_NAME}_test_print_help
        COMMAND sh -c "node ${CMAKE_CURRENT_SOURCE_DIR}/${MY_RUN_SCRIPT_NAME} $<TARGET_FILE:${MY_BIN_NAME}> -h"
    )
    
    add_test(
        NAME ${MY_BIN_NAME}_test_assemble
        COMMAND sh -c "node ${CMAKE_CURRENT_SOURCE_DIR}/${MY_RUN_SCRIPT_NAME} $<TARGET_FILE:${MY_BIN_NAME}> --print-level=0 ${CMAKE_CURRENT_SOURCE_DIR}/test.asm"
    )
else() 
    add_test(
        NAME ${MY_BIN_NAME}_test_print_help
        COMMAND ${MY_BIN_NAME} -h
    )

    add_test(
        NAME ${MY_BIN_NAME}_test_assemble
        COMMAND ${MY_BIN_NAME} --print-level=0 ${CMAKE_CURRENT_SOURCE_DIR}/test.asm
    )
endif()