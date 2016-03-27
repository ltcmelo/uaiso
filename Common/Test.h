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

#ifndef UAISO_TEST_H__
#define UAISO_TEST_H__

#include "Common/Config.h"
#include "StringUtils/trim.hpp"
#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

namespace uaiso {

class UAISO_API Test
{
public:
    virtual ~Test() {}

    virtual void run() = 0;

    virtual void reset() {}

    static void printStats();

protected:
    std::string currentTest_;
    static size_t passed_;
    static size_t skipped_;
    static size_t failed_;
    static std::vector<std::string> failedNames_;
};

struct UAISO_API TestFailed {};
struct UAISO_API TestSkipped {};

} // namespace uaiso


#define TEST_RUN(CLASS, ...) \
    std::vector<std::function<void (CLASS&)>> funcs_; \
    std::string funcNames_; \
    \
    void run() override \
    { \
        funcs_ = { __VA_ARGS__ }; \
        funcNames_ = { #__VA_ARGS__ }; \
        size_t pos1 = 0, pos2; \
        for (int i = 0; i < funcs_.size(); ++i) { \
            reset(); \
            pos2 = funcNames_.find(',', pos1); \
            currentTest_ = funcNames_.substr(pos1, pos2 - pos1); \
            str::trim(currentTest_); \
            pos1 = pos2 + 1; \
            try { \
                funcs_[i](*this); \
                std::cout << "[uaiso] Test " << currentTest_ << " -> OK\n"; \
                ++passed_; \
            } catch (const TestFailed&) { \
                ++failed_; \
                failedNames_.push_back(currentTest_); \
            } catch (const TestSkipped&) { \
                ++skipped_; \
            } \
        } \
    }

#define DECL_CLASS_TEST(CLASS) \
    friend void test_##CLASS(); \
    class CLASS##Test; \
    static std::unique_ptr<Test> createSelfTest(); \

#define MAKE_CLASS_TEST(CLASS) \
    namespace uaiso { \
        std::unique_ptr<Test> CLASS::createSelfTest() \
        { \
            return std::unique_ptr<Test>(new CLASS##Test); \
        } \
    }

#define CALL_CLASS_TEST(CLASS) \
    namespace uaiso { \
        void test_##CLASS() \
        { \
            std::unique_ptr<Test> test = CLASS::createSelfTest(); \
            test->run(); \
        } \
    }

#define UAISO_SKIP_TEST \
    do { \
        std::cout << "[uaiso] Skipping " << currentTest_ << std::endl; \
        throw TestSkipped(); \
    } while (0)

#define UAISO_FAIL_TEST(MSG) \
    do { \
        std::cout << "[uaiso] Test " << currentTest_ << " -> FAILED\n" \
                  << "        Reason: " << MSG << "\n" \
                  << "        " << __FILE__ << ":" << __LINE__ << std::endl; \
        throw TestFailed(); \
    } while (0)

#define UAISO_EXPECT_EQ(EXPECTED, ACTUAL, EQ) \
    do { \
        if (!(EQ(EXPECTED, ACTUAL))) { \
            std::cout << "[uaiso] Test " << currentTest_ << " -> FAILED\n" \
                      << "        Expected: " << EXPECTED << "\n" \
                      << "        Actual  : " << ACTUAL << "\n" \
                      << "        " << __FILE__ << ":" << __LINE__ << std::endl; \
            throw TestFailed(); \
        } \
    } while (0)

#define UAISO_EQ_OPR(A, B) A == B
#define UAISO_STD_EQ(A, B) std::equal(A.begin(), A.end(), B.begin())

#define UAISO_EXPECT_PTR_EQ(EXPECTED, ACTUAL) \
    UAISO_EXPECT_EQ(EXPECTED, ACTUAL, UAISO_EQ_OPR)
#define UAISO_EXPECT_STR_EQ(EXPECTED, ACTUAL) \
    UAISO_EXPECT_EQ(EXPECTED, ACTUAL, UAISO_EQ_OPR)
#define UAISO_EXPECT_INT_EQ(EXPECTED, ACTUAL) \
    UAISO_EXPECT_EQ(EXPECTED, ACTUAL, UAISO_EQ_OPR)
#define UAISO_EXPECT_CONTAINER_EQ(EXPECTED, ACTUAL) \
    UAISO_EXPECT_EQ(EXPECTED, ACTUAL, UAISO_STD_EQ)

#define UAISO_EXPECT(EXPR, BOOLEAN) \
    do { \
        if (bool(EXPR) != BOOLEAN) { \
            std::cout << "[uaiso] Test " << currentTest_ << " -> FAILED\n" \
                      << "        Expression is NOT " << #BOOLEAN << "\n" \
                      << "        " << __FILE__ << ":" << __LINE__ << std::endl; \
            throw TestFailed(); \
        } \
    } while (0)
#define UAISO_EXPECT_TRUE(EXPR) \
   UAISO_EXPECT(EXPR, true)
#define UAISO_EXPECT_FALSE(EXPR) \
   UAISO_EXPECT(EXPR, false)

#define UAISO_EXPECT_ASSERT \
    do { \
        std::cout << "[uaiso] Test assert -> "; \
    } while (0)

#endif
