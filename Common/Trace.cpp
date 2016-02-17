/******************************************************************************
 * Copyright (c) 2014-2016-2015 Leandro T. C. Melo (ltcmelo@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 *****************************************************************************/

/*--------------------------*/
/*--- The UaiSo! Project ---*/
/*--------------------------*/

#include "Common/Trace__.h"
#include <iostream>
#include <cstdarg>
#include <cstdio>

using namespace uaiso;

Trace::Trace(const std::string& name)
    : name_(name)
{}

bool Trace::isActive() const
{
    // TODO: Check trace name through a register.
    return false;
}

void Trace::debug(const char* msg, ...)
{
    if (isActive()) {
        std::cout << "[uaiso:" << name_ << "] " << std::flush;
        va_list args;
        va_start(args, msg);
        vfprintf(stderr, msg, args);
        va_end(args);
    }
}

void Trace::print(const char* msg, ...)
{
    std::cout << "[uaiso:" << name_ << "] " << std::flush;
    va_list args;
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
}
