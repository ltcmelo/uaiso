/******************************************************************************
 * Copyright (c) 2014-2015 Leandro T. C. Melo (ltcmelo@gmail.com)
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

#include "Parsing/Unit__.h"

using namespace uaiso;

Unit::Unit()
    : P(new UnitImpl)
{}

Unit::~Unit()
{}

void Unit::assignInput(const std::string& source)
{
    P->source_ = &source;
    P->bit_.readFromFile_ = false;
}

void Unit::assignInput(FILE* file)
{
    P->file_ = file;
    P->bit_.readFromFile_ = true;
}

void Unit::setFileName(const std::string& fullFileName)
{
    P->fullFileName_ = fullFileName;
}

const std::string& Unit::fileName() const
{
    return P->fullFileName_;
}

Ast* Unit::ast() const
{
    return P->ast_.get();
}

DiagnosticReports* Unit::releaseReports()
{
    auto reports = P->reports_.release();
    P->reports_.reset(new DiagnosticReports);
    return reports;
}
