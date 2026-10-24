/*
 * Copyright 2020 McGraw-Hill Education. All rights reserved. No reproduction or distribution without the prior written consent of McGraw-Hill Education.
 *
 * ----------------------------------------------------------------
 * Modifications Copyright (c) 2026 François Aucoin.
 * 
 * Notice: This file has been modified from its original version.
 * Description of changes: This file was created by François Aucoin
 * (as part of this CMake port of the LC3Tools project) based on
 * the original `framework.h` (which no longer exists in the current
 * project, as it was broken down in two).
 * ---------------------------------------------------------------- 
 */

#include "framework.h"

int main(int argc, char * argv[])
{
    std::cout << "\033[1;31m";
    std::cout << "********************************************************************************\n";
    std::cout << "*                             Deprecation Notice                               *\n";
    std::cout << "*                                                                              *\n";
    std::cout << "* Version 1 of the API is deprecated and no longer supported.  Please use the  *\n";
    std::cout << "* guide at the following URL to update this unit test:                         *\n";
    std::cout << "*   https://github.com/chiragsakhuja/lc3tools/blob/master/docs/UPGRADE1.md     *\n";
    std::cout << "*                                                                              *\n";
    std::cout << "********************************************************************************\n";
    std::cout << "\033[0m";

    framework1::setup = setup;
    framework1::shutdown = shutdown;
    framework1::testBringup = testBringup;
    framework1::testTeardown = testTeardown;

    framework1::main(argc, argv);
}