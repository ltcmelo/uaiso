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

#include "Common/FileInfo.h"

using namespace uaiso;

class FileInfo::FileInfoTest final : public Test
{
    public:
    TEST_RUN(FileInfoTest
             , &FileInfoTest::testCase1
             , &FileInfoTest::testCase2
             , &FileInfoTest::testCase3
             )

    void testCase1()
    {
        FileInfo fileInfo("/home/user/project/code.cpp");

        UAISO_EXPECT_STR_EQ("/home/user/project/code.cpp", fileInfo.fullFileName());
        UAISO_EXPECT_STR_EQ("code.cpp", fileInfo.fileName());
        UAISO_EXPECT_STR_EQ("code", fileInfo.fileBaseName());
        UAISO_EXPECT_STR_EQ("project", fileInfo.dir());
        UAISO_EXPECT_STR_EQ("/home/user/project/", fileInfo.fullDir());
    }

    void testCase2()
    {
        FileInfo fileInfo("/code.cpp");

        UAISO_EXPECT_STR_EQ("/code.cpp", fileInfo.fullFileName());
        UAISO_EXPECT_STR_EQ("code.cpp", fileInfo.fileName());
        UAISO_EXPECT_STR_EQ("code", fileInfo.fileBaseName());
        UAISO_EXPECT_STR_EQ("", fileInfo.dir());
        UAISO_EXPECT_STR_EQ("/", fileInfo.fullDir());
    }

    void testCase3()
    {
        // Test with invalid file name (path not complete), which should
        // be cleared in the assert.

        UAISO_EXPECT_ASSERT;
        FileInfo fileInfo("code.cpp");

        UAISO_EXPECT_ASSERT;
        UAISO_EXPECT_STR_EQ("", fileInfo.fullFileName());
    }
};

MAKE_CLASS_TEST(FileInfo)
