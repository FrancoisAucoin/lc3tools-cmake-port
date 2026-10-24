# LC3Tools CMake Port

This is an experimental CMake port of the official [LC3Tools project](https://github.com/chiragsakhuja/lc3tools). This port repackages a subset of the original source code, using a somewhat modern CMake approach that enables
installation of the `assembler` and `simulator` executables, as well as the "testing framework" as a library. Moreover, the port implements the minimal required plumbing to allow targeting Emscripten's Wasm for a Node.js host.

## Why I created this project

I created this project to enable two personal use cases:

### 1. Emscripten support

I wanted to target Emscripten's Wasm for a Node.js host. I needed
this because I was writing a simple [VS Code extension](https://github.com/FrancoisAucoin/lc3tools-vscode) with support for the LC-3 ISA Assembly language, inside of which I needed both the assembler (for validating and
assembling) and the simulator (for running programs directly inside a VS Code integrated terminal).
While the extension could simply have relied on those executables having been installed on the system path,
I felt that it would be nice if the extension was self-contained and didn't require the user
to manually install them. Wasm modules being cross-platform, they can simply be shipped
as part of the extension without much concern regarding the user's system specifications.

### 2. Exported CMake targets

I also wanted to be able to build and install the project as a set
of libraries (and executables) in an effort to make the process of
using the LC3Tools testing framework more "standalone". More
precisely, the current recommended way of using the testing framework
is to clone the official repository and to add user-created tests directly 
inside the repository's `src/test/tests/samples` directory. With this CMake
port, the user can clone, build (for their host), and install (anywhere on their
system), and then proceed to create standalone CMake-managed C/C++ projects in
which they simply do something like this:

```CMakeLists.txt
find_package(LC3Tools 1.0.0 CONFIG REQUIRED)
add_executable(my_test_1 my_test_1.cc)
target_link_libraries(my_test_1 PRIVATE LC3Tools::testing_framework_v2)
```

## How to use

So far I have tested the build system generator with two compilers/toolchains, namely, Apple's
Clang 21 and Emscripten (the SDK) version 6.0.9. However, as far as native builds are concerned, I expect
the port to also work with other modern UNIX/Linux toolchains, as well as with Windows Subsystem
for Linux (WSL).

Also note that, somewhat arbitrarily, as well as out of personal preference and convenience, the
minimum required CMake version for the project was set to `3.31.4`, and the minimum C++ standard
was set to `23`. You can have a look at [./CMakeLists.txt](./CMakeLists.txt) if you are curious 
about the details.

That being said, assuming that you have `git`, `cmake`, and `clang/clang++` (or `gcc/g++`) 
installed on your UNIX-like system, here is a simple example that illustrates how to get
started with a native build:

```shell
git clone https://github.com/FrancoisAucoin/lc3tools-cmake-port
cd lc3tools-cmake-port
mkdir build
cmake -S . -B ./build -DCMAKE_BUILD_TYPE="Release"
cmake --build ./build --parallel
cmake --install ./build --prefix=/your/custom/installation/path
# Or `sudo cmake --install ./build` for a system-wide installation.
```

Those commands will install a set of libraries as well as the `assembler` and
`simulator` executables (note that the binaries are named `lc3tools-assembler` and `lc3tools-simulator`
to avoid name collisions; the libraries are prefixed with `lc3tools_` for the same reason). For the purpose of using the testing framework,
the library of interest is named `testing_framework_v2`. When using
CMake to manage the "consumer project" (i.e., the project using `find_package(LC3Tools 1.0.0 CONFIG REQUIRED)`),
one only needs to link against that specific library (i.e., `target_link_libraries(my_test_1 PRIVATE LC3Tools::testing_framework_v2)`), as CMake will take care of linking against the other required
ones, namely `backend`, `common`, and `testing_framework_common`.

A simple C++ source file representing a dummy test implementation would look something like this:

```C++
#include <lc3tools/testing_framework_v2/framework.h>

using Tester = framework2::Tester;

static void DummyTest(lc3::sim &sim, Tester &tester, double total_points)
{
    (void)sim;
    tester.verify("Dummy", true, total_points);
}

void testBringup(lc3::sim &sim)
{
    (void)sim;
}

void testTeardown(lc3::sim &sim)
{
    (void)sim;
}

void setup(Tester &tester)
{
    tester.registerTest("Dummy", DummyTest, 20, false);
}

void shutdown() {}
```

Again, the nice thing about this approach is that the project containing the
test(s) is completely independent of the LC3Tools project, and _vice versa_.

For a comprehensive and official treatment of the "testing framework" usage, consult the following links:
* [lc3tools/docs/TEST.md](https://github.com/chiragsakhuja/lc3tools/blob/master/docs/TEST.md)
* [lc3tools/docs/API.md](https://github.com/chiragsakhuja/lc3tools/blob/master/docs/API.md)

Finally, at this point I should mention that I haven't included, in the current project, any of the source code pertaining to the test samples, as those weren't relevant to my objectives. But the interested
user can find them [here](https://github.com/chiragsakhuja/lc3tools/blob/7006c2b8c68e33fa86237e1395308c73a3445fb2/src/test/tests/samples), in the official repository.

### A few extra words about targeting Emscripten

If your use case demands that you compile using the Emscripten SDK/toolchain, you should know about
a few more things:

#### Custom CMake variables 

* **`LC3_TOOLS_ENABLE_WASM_64_BIT`** – This instructs CMake to generate a build targeting Wasm 64-bit instead of the default Wasm 32-bit.

#### Provide CMake with `CMAKE_TOOLCHAIN_FILE`

You also need to tell CMake where to find the Emscripten toolchain file. To do that,
you can either pass `-DCMAKE_TOOLCHAIN_FILE=...` or simply use the (slightly) more
convenient `--toolchain` flag, as shown in this example:

```shell
mkdir ./build
# NOTE 1: If you want to target Wasm64, add `-DLC3_TOOLS_ENABLE_WASM_64_BIT=true` to
# the configuration step.
# NOTE 2: The Emscripten SDK version 6.0.9 requires Python 3.10 or above. You can use `EMSDK_PYTHON`
# to specify which Python installation to use (e.g., `export EMSDK_PYTHON=/path/to/custom/python`)
cmake \
    -S /path/to/cloned/lc3tools-cmake-port/on/your/system \
    -B ./build \
    --toolchain=/path/to/emsdk-6.0.9/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake \
    -DCMAKE_BUILD_TYPE="Release"
cmake --build ./build --parallel
```

In most cases, as far as targeting Emscripten is concerned, you'll likely be interested
in the produced executables (the Emscripten Wasm module and the associated Node.js glue
code). Those will be under `./build/src/bin/assembler` and `./build/src/bin/simulator`.
More on those right below.

#### Using the produced executables

After having built the project, the following files of interest will be available:

* Assembler
  * **`./build/src/bin/assembler/lc3tools-assembler.mjs`** – This is the Emscripten JavaScript
  glue code for bridging the Node.js host and the Wasm guest (module).
  * **`./build/src/bin/assembler/lc3tools-assembler.wasm`** – This is the Emscripten Wasm module.
  In most cases, it should remain right beside its `lc3tools-assembler.mjs` counterpart for things
  to function smoothly without the need for more advanced configurations.
  * **`./build/src/bin/assembler/lc3tools-run-assembler.mjs`** – This is a simple JavaScript program
  I wrote to be able to run the executable inside a Node.js host. That file is
  simply copied from [src/bin/assembler/run-assembler.mjs](./src/bin/assembler/run-assembler.mjs)
  during the build process, for convenience. To run the program, you would do something 
  like this: `node /path/to/lc3tools-run-assembler.mjs /path/to/lc3tools-assembler.mjs /path/to/test.asm`.
  I added a discussion at the top of [src/bin/assembler/run-assembler.mjs](./src/bin/assembler/run-assembler.mjs).
    * **IMPORTANT:** The program expects the full path for `lc3tools-assembler.mjs`.
* Simulator
  * **`./build/src/bin/simulator/lc3tools-simulator.mjs`** – This is the Emscripten JavaScript
  glue code for bridging the Node.js host and the Wasm guest (module).
  * **`./build/src/bin/simulator/lc3tools-simulator.wasm`** – This is the Emscripten Wasm module.
  In most cases, it should remain right beside its `lc3tools-simulator.mjs` counterpart for things
  to function smoothly without the need for more advanced configurations.
  * **`./build/src/bin/simulator/lc3tools-run-simulator.mjs`** – This is a simple JavaScript program
  I wrote to be able to run the executable inside a Node.js host. That file is
  simply copied from [src/bin/simulator/run-simulator.mjs](./src/bin/simulator/run-simulator.mjs)
  during the build process, for convenience. To run the program, you would do something 
  like this: `node /path/to/lc3tools-run-simulator.mjs /path/to/lc3tools-simulator.mjs /path/to/test.obj`.
  I added a discussion at the top of [src/bin/simulator/run-simulator.mjs](./src/bin/simulator/run-simulator.mjs). I encourage you to have a look at it, because there are a few constraints worth knowing
  about when it comes to running the Wasm version of the simulator inside a Node.js host.
    * **IMPORTANT:** The program expects the full path for `lc3tools-simulator.mjs`.
  
It is worth noting that the `run-assembler.mjs` and `run-simulator.mjs` JavaScript programs
are just blueprints to get started quickly, and that some (most) applications will require their
own custom entrypoints. For instance, inside my VS Code extension, the entrypoint script
for my assembler is a Node.js worker, which avoids blocking VS Code's main thread while assembling.

## Modifications to the original source code

The [original project](https://github.com/chiragsakhuja/lc3tools) uses a relatively
flat folder structure. This is one of the major differences with the current project,
in which I opted for something more structured (not necessarily better, but structured with deeper nesting).
I divided my project's `src` directory into `bin` and `lib`, for the executables (assembler and simulator) and the libraries,
respectively. I named the libraries `backend`, `common`, and `testing_framework_{suffix}`
based on the original project's nomenclature. Everything (i.e., testing frameworks and executables)
is based on the `backend`. The `common` part, as I understand it, contains code that is not
relevant to the `backend`, but which does get used by both executables, as well as by the
testing frameworks. The testing frameworks consist of the old, now-deprecated testing framework (i.e., version 1),
and the new, recommended testing framework (i.e., version 2). Parts of the source code (for the testing frameworks)
were used by both versions, which is why it is referred to as "common". In the original project,
the testing framework gets selected based on a pre-processor macro named `API_VER`, and the frameworks'
`main` functions are both declared from inside a single header file. I didn't retain that approach here.
Instead, I deleted the [lc3tools/src/test/framework.h](https://github.com/chiragsakhuja/lc3tools/blob/master/src/test/framework.h) header file entirely, and created two separate `main.cpp` files: one for each framework. I also moved the user-provided functions inside their respective framework header file, as opposed to declaring
them at the top of the implementation files. See [src/lib/testing_framework/v2](./src/lib/testing_framework/v2) as an example.

With this new structure, I had to modify some "cross-library" `include` directives (e.g., when the assembler implementation file needs to refer to backend symbols, we must now do `#include <lc3tools/backend/interface.h>`). But internally, inside each library, I kept the flat structure for include directives.

While one of my goals was to keep code modifications to a minimum, one thing
I absolutely had to do was to add preprocessor conditional checks for `__EMSCRIPTEN__`
at a few key places throughout the codebase, to provide alternative implementations for
Emscripten targets. The affected source files are:
* [src/bin/simulator/main.cpp](./src/bin/simulator/main.cpp)
* [src/lib/common/include/lc3tools/common/console_inputter.h](./src/lib/common/include/lc3tools/common/console_inputter.h)
* [src/lib/common/src/console_inputter.cpp](./src/lib/common/src/console_inputter.cpp)
If you are curious to see exactly what implementation details were added, simply do
a project-level text search for `__EMSCRIPTEN__` in your favorite editor/IDE.

I did find a few hidden bugs during the process of adding support for Emscripten. Interestingly, 
I probably wouldn't have found those had I not tried to target Wasm 32-bit (which is Emscripten's default) for the simulator. The simulator was starting but was crashing for most instructions. The reported cause for those crashes
was a segmentation fault; i.e., the program was performing out-of-bounds memory access operations at runtime.
Initially, I couldn't figure out just why, but a light bulb went on when I tried Wasm-64 and it worked, as it gave me the idea to start looking for type mismatches inside formatting strings, which was indeed the problem. While those mismatches don't appear to affect a 64-bit system (although I think they are still undefined behavior), they are a problem for a 32-bit system like Wasm32. As an example, at [src/backend/simulator.cpp:122 – official repo](https://github.com/chiragsakhuja/lc3tools/blob/d8409060241d35bb31fce0ced534e63e150b54f1/src/backend/simulator.cpp#L122), `%d ... %s` is used to format a value of type `uint64_t` followed by a null-terminated string. The reason we don't get a crash on modern 64-bit systems is likely just a lucky artifact of how the compilers implement `va_list/va_arg` (which, as I understand, is what gets used by the `fprintf` family of functions to enable variadic arguments). But what likely happens for Emscripten's Wasm32 implementation is that `va_arg(ap, int)` probably first gets called for `%d`, which gets the first 32 bits (because `int` is 32-bit on Emscripten's Wasm32), followed by `va_arg(ap, char*)` for `%s`, which is 32-bit on Wasm32 (it's a pointer), but those 32 bits are not the ones that represent the memory address of our string, but instead the upper (likely padding) bits of our first `uint64_t` value, causing the segmentation fault (at runtime). Note that I haven't officially confirmed how Emscripten's `va_list/va_arg` is implemented, but fixing the formatters did solve the problem, so it's likely something along those lines.

So here is the list of the small modifications I made to the source code in an attempt to 
remove the possibility of those runtime memory access bugs that become apparent when trying to compile for
a 32-bit system:

* Replaced `%d` with `%zu` in the format string, as the (implicit/promoted) type is `size_t`.
  * [src/backend/simulator.cpp:122 – official repo](https://github.com/chiragsakhuja/lc3tools/blob/d8409060241d35bb31fce0ced534e63e150b54f1/src/backend/simulator.cpp#L122)
* Replaced `%d` with `PRIu64` (from the `cinttypes` header)
  * [src/backend/encoder.cpp:168 – official repo](https://github.com/chiragsakhuja/lc3tools/blob/7006c2b8c68e33fa86237e1395308c73a3445fb2/src/backend/encoder.cpp#L168)
  * [src/backend/simulator.cpp:115 – official repo](https://github.com/chiragsakhuja/lc3tools/blob/7006c2b8c68e33fa86237e1395308c73a3445fb2/src/backend/simulator.cpp#L115)
  * [src/backend/simulator.cpp:122 – official repo](https://github.com/chiragsakhuja/lc3tools/blob/7006c2b8c68e33fa86237e1395308c73a3445fb2/src/backend/simulator.cpp#L122)
  * [src/backend/simulator.cpp:127 – official repo](https://github.com/chiragsakhuja/lc3tools/blob/7006c2b8c68e33fa86237e1395308c73a3445fb2/src/backend/simulator.cpp#L127)
* Modified the for loop such that it uses `size_t` instead of `int64_t`.
  * [src/backend/simulator.cpp:204 – official repo](https://github.com/chiragsakhuja/lc3tools/blob/7006c2b8c68e33fa86237e1395308c73a3445fb2/src/backend/simulator.cpp#L204)
  * [src/backend/simulator.cpp:212 – official repo](https://github.com/chiragsakhuja/lc3tools/blob/7006c2b8c68e33fa86237e1395308c73a3445fb2/src/backend/simulator.cpp#L212)

Also, although not a bug, I did modify [src/backend/decoder.cpp:28 – official repo](https://github.com/chiragsakhuja/lc3tools/blob/7006c2b8c68e33fa86237e1395308c73a3445fb2/src/backend/decoder.cpp#L28) to get rid of a `range-loop-construct` Clang warning, because we were creating a constant copy instead of using a constant reference; i.e., I changed `for(PIOperand const op : inst->getOperands())` to `for(PIOperand const & op : inst->getOperands())`.

## Miscellaneous

* As I like using `-Werror` for my personal C and C++ projects, I had to add `-Wno-format-security` as
a compile option for the project to build on Apple's Clang. The `format string is not a string literal (potentially insecure)` warnings happen because a template inside the codebase (see [src/backend/utils.h:118](https://github.com/chiragsakhuja/lc3tools/blob/7006c2b8c68e33fa86237e1395308c73a3445fb2/src/backend/utils.h#L118)) uses `ssprintf` with a non-literal formatting string, which is, per the warning name, insecure. Interestingly, when using `-Werror`, the formatting string type mismatches that cause the bug discussed earlier is not allowed to happen without `-Wno-format-security`. Ideally, the codebase would be refactored to not use `std::string const &format` in `template<typename ... Args> std::string ssprintf(std::string const & format, Args ... args)`. One safer alternative would be to do something like `template<typename ... Args> std::string ssprintf(const char* format, Args ... args) __attribute__((format(printf, 1, 2)))`. Another, more modern approach could rely on `format`; e.g.,`template<typename ... Args> std::string ssprintf(std::format_string<Args...> format, Args ... args)`. But either approach would require substantial refactoring, so I decided to simply keep `-Wno-format-security` for now and keep in mind that doing so comes with non-negligible risks.

## Experimental status

This is an experimental project, so treat it as such. As stated in the in the [Why I created this project](#why-i-created-this-project) section above, my two main goals here were to (1) make testing using the framework more portable and to (2) add Emscripten support for the assembler and the simulator so I could use them as Wasm modules in my [VS Code extension](https://github.com/FrancoisAucoin/lc3tools-vscode) project. As soon as I felt that those two goals had been achieved (i.e., that things were working well enough), I stopped, although I still did put some non-negligible efforts into decently documenting everything so that others could make sense of my work, and also so that I could more easily come back at a later time and continue improving the project and extending its scope.

## Copyright

Copyright (c) 2026 François Aucoin. All rights reserved.