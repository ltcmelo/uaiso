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

#ifndef UAISO_TYPECHECKERTEST_H__
#define UAISO_TYPECHECKERTEST_H__

#include "Common/LineCol.h"
#include "Semantic/TypeChecker.h"

namespace uaiso {

class Factory;
class Unit;

class TypeChecker::TypeCheckerTest final : public Test
{
public:
    TEST_RUN(TypeCheckerTest
             // Go
             , &TypeCheckerTest::GoTestCase1
             , &TypeCheckerTest::GoTestCase2
             , &TypeCheckerTest::GoTestCase3
             , &TypeCheckerTest::GoTestCase4
             , &TypeCheckerTest::GoTestCase5
             , &TypeCheckerTest::GoTestCase6
             , &TypeCheckerTest::GoTestCase7
             , &TypeCheckerTest::GoTestCase8
             )

    //--- Go ---//

    void GoTestCase1();
    void GoTestCase2();
    void GoTestCase3();
    void GoTestCase4();
    void GoTestCase5();
    void GoTestCase6();
    void GoTestCase7();
    void GoTestCase8();


    std::unique_ptr<Unit> runCore(std::unique_ptr<Factory> factory,
                                  const std::string& code,
                                  const std::string& fullFileName,
                                  const std::vector<Diagnostic::Code>& expectedReports,
                                  const std::pair<std::string, Type::Kind>& expectedBindings);
};

} // namespace uaiso

#endif
