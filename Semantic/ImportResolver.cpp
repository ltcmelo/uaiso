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
#include "Semantic/Import.h"
#include "Common/FileInfo.h"
#include "Common/Util__.h"
#include "Common/Trace__.h"
#include "Tinydir/Tinydir.h"
#include "Parsing/Factory.h"
#include "Parsing/Lang.h"
#include "Parsing/Lexeme.h"
#include "StringUtils/predicate.hpp"
#include <fstream>
#include <iostream>
#include <unordered_set>

#define TRACE_NAME "ImportResolver"

using namespace uaiso;
using namespace str;

struct ImportResolver::ImportResolverImpl
{
    ImportResolverImpl(Factory* factory)
        : lang_(factory->makeLang())
    {}

    std::pair<std::vector<std::string>, Import::TargetEntity>
    resolve(std::string target,
            const std::string& path,
            const std::unordered_set<std::string>& fileFilter)
    {
        auto pos = target.find(lang_->packageSeparator());
        while (pos != std::string::npos) {
            target.replace(pos, 1, std::string(1, FileInfo::dirSeparator()));
            pos = target.find(lang_->packageSeparator(), pos + 1);
        }

        std::vector<std::string> result;

        if (lang_->importMechanism() == Lang::PerModule
                || lang_->importMechanism() == Lang::PerModuleAndPackage) {
            auto moduleFile = path + target + lang_->sourceFileSuffix();
            DEBUG_TRACE("search module import %s\n", moduleFile.c_str());
            std::ifstream ifs(moduleFile);
            if (ifs.is_open()) {
                ifs.close();
                result.emplace_back(moduleFile);
                DEBUG_TRACE("module file %s found\n", moduleFile.c_str());
                return std::make_pair(result, Import::Module);
            }
            // If the language's import mechanism is per module only, there's
            // nothing to do. Otherwise, let it search packages.
            if (lang_->importMechanism() == Lang::PerModule)
                return std::make_pair(result, Import::Module);
        }

        auto dirPath = path + target;
        DEBUG_TRACE("search package import %s\n", dirPath.c_str());
        tinydir_dir dir;
        tinydir_open(&dir, dirPath.c_str());
        while (dir.has_next) {
            tinydir_file fileInDir;
            tinydir_readfile(&dir, &fileInDir);

            std::string fileInDirName(fileInDir.name);
            if (!fileInDir.is_dir
                    && iends_with(fileInDirName, lang_->sourceFileSuffix())) {
                // Add as a result if either the filter is completely empty
                // or if the file in question is filtered in.
                if (fileFilter.empty() || fileFilter.count(fileInDirName)) {
                    result.emplace_back(dirPath + "/" + fileInDirName);
                    DEBUG_TRACE("package file %s found\n",
                                (dirPath + "/" + fileInDirName).c_str());
                }
            }
            tinydir_next(&dir);
        }
        tinydir_close(&dir);

        return std::make_pair(result, Import::Package);
    }

    std::unique_ptr<Lang> lang_;
};

ImportResolver::ImportResolver(Factory *factory)
    : P(new ImportResolverImpl(factory))
{}

ImportResolver::~ImportResolver()
{}

std::vector<std::string> ImportResolver::
resolve(Import* import) const
{
    return resolve(import, std::vector<std::string>());
}

std::vector<std::string> ImportResolver::
resolve(Import* import,
        const std::vector<std::string>& searchPaths) const
{
    std::unordered_set<std::string> selected;
    std::for_each(import->selectedItems().begin(),
                  import->selectedItems().end(),
                  [&selected, this](const Ident* name) {
        selected.insert(name->str() + P->lang_->sourceFileSuffix());
    });

    auto data = P->resolve(import->target(), import->fromWhere(), selected);
    if (!data.first.empty()) {
        import->setTargetEntity(data.second);
        return data.first;
    }

    for (const auto& searchPath : searchPaths) {
        data = P->resolve(import->target(), searchPath, selected);
        if (!data.first.empty()) {
            import->setTargetEntity(data.second);
            break;
        }
    }

    return data.first;
}
