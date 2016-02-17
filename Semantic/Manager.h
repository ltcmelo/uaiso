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

#ifndef UAISO_MANAGER_H__
#define UAISO_MANAGER_H__

#include "Common/Flag.h"
#include "Common/LineCol.h"
#include "Common/Pimpl.h"
#include <cstdio>
#include <string>
#include <vector>

namespace uaiso {

class Factory;
class LexemeMap;
class Snapshot;
class TokenMap;
class Unit;

/*!
 * \brief The Manager class
 */
class UAISO_API Manager final
{
public:
    Manager();
    ~Manager();

    void config(Factory* factory,
                TokenMap* tokens,
                LexemeMap* lexs,
                Snapshot snapshot);

    void addSearchPath(const std::string& searchPath);

    /*!
     * \brief The BehaviourFlag enum
     */
    enum class BehaviourFlag : uint8_t
    {
        None                   = 0,
        IgnoreBuiltins         = 0x1,
        IgnoreAutomaticModules = 0x1 << 1,
    };
    UAISO_FLAGGED_ENUM(BehaviourFlag);

    /*!
     * \brief setBehavior
     * \param flags
     */
    void setBehaviour(BehaviourFlags flags);

    /*!
     * \brief behavior
     * \return
     */
    BehaviourFlags behaviour() const;

    /*!
     * \brief process
     * \param code
     * \param fullFileName
     * \return
     *
     * Parse and bind the code associated with the given name, insert it
     * into the Snapshot, and process its dependencies.
     */
    std::unique_ptr<Unit> process(const std::string& code,
                                  const std::string& fullFileName);

    /*!
     * \brief process
     * \param file
     * \param fullFileName
     * \return
     *
     * Parse and bind the file associated with the given name, insert it
     * into the Snapshot, and process its dependencies.
     */
    std::unique_ptr<Unit> process(FILE* file,
                                  const std::string& fullFileName);

    /*!
     * \brief process
     * \param code
     * \param fullFileName
     * \param lineCol
     * \return
     *
     * Parse and bind the code associated with the given name, up to the
     * specified line and colum, insert it into the Snapshot, and process
     * its dependencies.
     */
    std::unique_ptr<Unit> process(const std::string& code,
                                  const std::string& fullFileName,
                                  const LineCol& lineCol);

    /*!
     * \brief resolveDeps
     * \param fullFileName
     *
     * Expects that a Program associated with the given name is present
     * in the snapshot and process its dependencies.
     *
     * \note This is usefull is the AST has already been gone through the Binder.
     */
    void processDeps(const std::string& fullFileName) const;

private:
    DECL_PIMPL(Manager)

    void processCore(Unit* unit);
};

} // namespace uaiso

#endif
