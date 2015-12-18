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

#include "Semantic/ImportResolver.h"
#include "Semantic/Symbol.h"
#include "Common/FileInfo.h"
#include "Common/Util__.h"
#include "Common/Trace__.h"
#include "Tinydir/Tinydir.h"
#include "Parsing/Factory.h"
#include "Parsing/Syntax.h"
#include "StringUtils/predicate.hpp"
#include <fstream>
#include <iostream>

#define TRACE_NAME "ImportResolver"

using namespace uaiso;
using namespace str;

struct ImportResolver::ImportResolverImpl
{
    ImportResolverImpl(Factory* factory)
        : syntax_(factory->makeSyntax())
    {}

    std::vector<std::string> searchFile(std::string relPath,
                                        const std::string& basePath)
    {
        auto pos = relPath.find(syntax_->packageSeparator());
        while (pos != std::string::npos) {
            relPath.replace(pos, 1, std::string(1, FileInfo::dirSeparator()));
            pos = relPath.find(syntax_->packageSeparator(), pos + 1);
        }

        std::vector<std::string> result;

        if (syntax_->importMechanism() == Syntax::PerModule) {
            auto moduleFile = basePath + relPath + syntax_->sourceFileSuffix();
            DEBUG_TRACE("search module import %s\n", moduleFile.c_str());
            std::ifstream ifs(moduleFile);
            if (ifs.is_open()) {
                ifs.close();
                result.emplace_back(moduleFile);
                DEBUG_TRACE("module file %s found\n", moduleFile.c_str());
            }
            return result;
        }

        auto dirPath = basePath + relPath;
        DEBUG_TRACE("search package import %s\n", dirPath.c_str());
        tinydir_dir dir;
        tinydir_open(&dir, dirPath.c_str());
        while (dir.has_next) {
            tinydir_file fileInDir;
            tinydir_readfile(&dir, &fileInDir);

            std::string fileInDirName(fileInDir.name);
            if (!fileInDir.is_dir
                    && iends_with(fileInDirName, syntax_->sourceFileSuffix())) {
                result.emplace_back(dirPath + "/" + fileInDirName);
                DEBUG_TRACE("package file %s found\n",
                            (dirPath + "/" + fileInDirName).c_str());
            }
            tinydir_next(&dir);
        }
        tinydir_close(&dir);

        return result;
    }

    std::unique_ptr<Syntax> syntax_;
};

ImportResolver::ImportResolver(Factory *factory)
    : P(new ImportResolverImpl(factory))
{}

ImportResolver::~ImportResolver()
{}

std::vector<std::string> ImportResolver::
resolve(const Import* import) const
{
    return P->searchFile(import->moduleName(), import->originDir());
}

std::vector<std::string> ImportResolver::
resolve(const Import* import,
        const std::vector<std::string>& searchPaths) const
{
    auto filesData = resolve(import);
    if (!filesData.empty())
        return filesData;

    for (const auto& path : searchPaths) {
        filesData = P->searchFile(import->moduleName(), path);
        if (!filesData.empty())
            break;
    }

    return filesData;
}
