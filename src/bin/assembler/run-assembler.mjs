/*
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
*/

// -------------------------------------------------------------------------------------------------------
//                                          ABOUT THIS PROGRAM
// -------------------------------------------------------------------------------------------------------
// This is a very simple Node.js program that allows using the produced Emscripten
// Wasm32 (or Wasm64) Node.js `LC-3 Tools Assembler` module (i.e., the `assembler.mjs`
// Emscripten module and the corresponding `assembler.wasm` Wasm module). This program assumes
// that the assembly (or "binary") files use the `.asm` (or `.bin`) extension. Here are two simple
// usage examples:
// 
// > # This prints the usage info
// > node /path/to/run-assembler.mjs /path/to/assembler.mjs -h
// >
// > # This assembles an LC-3 ISA assembly file and creates `/path/to/lc3-asm-program.obj`
// > node /path/to/run-assembler.mjs /path/to/assembler.mjs /path/to/lc3-asm-program.asm
// 
// So basically we run `node` with the first argument being this very program, followed by the
// full path to the Emscripten module, beside which is expected to be found the corresponding
// `.wasm` module, followed by the actual arguments to be passed to the assembler executable itself; 
// i.e., the optional flags to be used and the files to be assembled.
// -------------------------------------------------------------------------------------------------------

if (process.argv.length < 3) {
    throw new Error(`expecting module path as first argument`);
}

import { readFileSync, writeFileSync } from "node:fs";

const FILE_EXTENSION_REGULAR_EXPRESSION = /(\.asm|\.bin)$/i;

const isAsmOrBinFile = (/** @type {string} */arg) => {
    return FILE_EXTENSION_REGULAR_EXPRESSION.test(arg);
}

const Module = (await import(process.argv[2]));

const myArguments = process.argv.slice(3);

try {
    await Module.default({
        arguments: myArguments.map((arg, index) => {
            if (isAsmOrBinFile(arg)) {
                return `/file_${index}.asm`;
            } else {
                return arg;
            }
        }),
        preRun: [
            (mod) => {
                myArguments.forEach((arg, index) => {
                    if (isAsmOrBinFile(arg)) {
                        const data = readFileSync(arg, "utf-8");
                        mod.FS.writeFile(`/file_${index}.asm`, data);
                    }
                });
            }
        ],
        postRun: [
            (mod) => {
                myArguments.forEach((arg, index) => {
                    if (isAsmOrBinFile(arg)) {
                        const data = mod.FS.readFile(`/file_${index}.obj`);
                        writeFileSync(arg.replace(FILE_EXTENSION_REGULAR_EXPRESSION, ".obj"), data);
                    }
                });
            }
        ],
        print: text => {
            console.log(text);
        },
        printErr: text => {
            // I don't think the assembler uses STDERR anywhere, so we don't
            // expect this function to ever get called.
            console.log(`INTERCEPTED STDERR:\n${text}`);
        }
    });
} catch (error) {
    if (error instanceof WebAssembly.Exception) {
        console.log(`WebAssembly.Exception occurred`);
        process.exit(0); // NOTE: For some reason, the WebAssembly exception is accompanied by a non-zero error code getting set, even if we catch. I need to research this more to understand why that is.
    } else {
        throw error;
    }
}
