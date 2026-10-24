/*
 * Copyright 2020 McGraw-Hill Education. All rights reserved. No reproduction or distribution without the prior written consent of McGraw-Hill Education.
 *
 * -----------------------------------------------------------
 * Modifications Copyright (c) 2026 François Aucoin.
 * 
 * Notice: This file has been modified from its original version.
 * Description of changes: Modified inclusion directive(s) from
 * flat header referencing to library-based referencing.
 * ----------------------------------------------------------- 
 */
#ifndef CONSOLE_PRINTER_H
#define CONSOLE_PRINTER_H

#include <lc3tools/backend/printer.h>

namespace lc3
{
    class ConsolePrinter : public utils::IPrinter
    {
    public:
        virtual void setColor(utils::PrintColor color) override;
        virtual void print(std::string const & string) override;
        virtual void newline(void) override;
    };
};

#endif
