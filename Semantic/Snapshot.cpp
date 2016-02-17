/******************************************************************************
 * Copyright (c) 2014-2016 Leandro T. C. Melo (ltcmelo@gmail.com)
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

#include "Semantic/Snapshot.h"
#include "Semantic/Program.h"
#include "Semantic/Symbol.h"
#include "Ast/Ast.h"
#include "Common/Assert.h"
#include "Parsing/LexemeMap.h"
#include "Parsing/TokenMap.h"
#include <unordered_map>
#include <vector>

using namespace uaiso;

struct uaiso::Snapshot::SnapshotImpl
{
    std::unordered_map<std::string, std::unique_ptr<Program> > programs_;
};

Snapshot::Snapshot()
    : impl_(new SnapshotImpl)
{}

void Snapshot::insertOrReplace(const std::string& fullFileName,
                               std::unique_ptr<Program> program)
{
    impl_->programs_[fullFileName] = std::move(program);
}

Program* Snapshot::find(const std::string& fullFileName) const
{
    auto it = impl_->programs_.find(fullFileName);
    if (it != impl_->programs_.end())
        return (it->second).get();
    return nullptr;
}
