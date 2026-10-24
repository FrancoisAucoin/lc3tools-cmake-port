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
 * project, as it was broken down in two). The `#define API_VER 2` 
 * macro was added because it is needed by `{console}/interface.h`
 * to define the proper API for testing framework version 2.
 * ----------------------------------------------------------------  
 */

#define API_VER 2 

#include "framework.h"

int main(int argc, char * argv[])
{
    framework2::setup = setup;
    framework2::shutdown = shutdown;
    framework2::testBringup = testBringup;
    framework2::testTeardown = testTeardown;

    framework2::main(argc, argv);
}