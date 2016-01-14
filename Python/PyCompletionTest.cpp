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

#include "Semantic/CompletionTest.h"
#include "Parsing/Factory.h"
#include "Parsing/Unit.h"

using namespace uaiso;

void CompletionProposer::CompletionProposerTest::PyTestCase1()
{
    std::string code = R"raw(
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y
    def show(self):
        print 'x' % x
        print 'y' % y
                                                 # line 8
p =
#  ^
#  |
#  complete at up-arrow
)raw";

    lineCol_ = { 9, 3 };
    auto expected = { "p", "Point" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase2()
{
    std::string code = R"raw(
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y
    def show(self):
        print 'x' % x
        print 'y' % y
                                                 # line 8
p = Point()
p.
# ^
# |
# complete at up-arrow
)raw";

    lineCol_ = { 10, 2 };
    auto expected = { "x", "y", "show", "__init__" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase3()
{
    std::string code = R"raw(
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y
                                                 # line 5

if a:
    p = Point()

p.
# ^
# |
# complete at up-arrow
)raw";

    lineCol_ = { 10, 2 };
    auto expected = { "x", "y", "__init__" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase4()
{
    std::string code = R"raw(
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y
                                                 # line 5

def foo():
    p = Point()
    p.
#     ^
#     |
#     complete at up-arrow
)raw";

    lineCol_ = { 9, 6 };
    auto expected = { "x", "y", "__init__" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase5()
{
    std::string code = R"raw(
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y
                                                 # line 5

p = Point()

def foo():
    p.
#     ^
#     |
#     complete at up-arrow
)raw";

    lineCol_ = { 10, 6 };
    auto expected = { "x", "y", "__init__" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase6()
{
    std::string code = R"raw(
class A:
    a = 1

class B:
    b = "b"
                                                 # line 6
p = A()

def foo():
    p = B()
    p.
#     ^
#     |
#     complete at up-arrow
)raw";

    lineCol_ = { 11, 6 };
    auto expected = { "b" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase7()
{
    std::string code = R"raw(
class A:
    a = 1

class B:
    b = "b"
                                                 # line 6

if x:
    p = A()
else:
    p = A()

p.
# ^
# |
# complete at up-arrow
)raw";

    lineCol_ = { 13, 2 };
    auto expected = { "a" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase8()
{
    UAISO_SKIP_TEST;

    std::string code = R"raw(
class A:
    a = 1

class B:
    b = "b"
                                                 # line 6

if x:
    p = A()
else:
    p = B()

p.
# ^
# |
# complete at up-arrow
)raw";

    lineCol_ = { 13, 2 };
    // Be conservative and show completion for both types.
    auto expected = { "a", "b" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase9()
{
    std::string code = R"raw(
import fibo

def doit():
    fibo.
#        ^
#        |
#        complete at up-arrow
)raw";

    lineCol_ = { 4, 9 };
    auto expected = { "fib", "fib2" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase10()
{
    std::string code = R"raw(
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y
                                                 # line 5

name = "foo"

# complete above at line start
)raw";

    lineCol_ = { 8, 0 };
    auto expected = { "Point", "name" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase11()
{
    std::string code = R"raw(
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y
                                                 # line 5

name = "foo"

def f():
#       ^
#       |
#       complete at up-arrow
)raw";

    lineCol_ = { 9, 8 };
    auto expected = { "Point", "name", "f" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase12()
{
    std::string code = R"raw(
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y
                                                 # line 5

name = "foo"

if __name__ == "__main__":
#                         ^
#                         |
#                         complete at up-arrow
)raw";

    lineCol_ = { 9, 26 };
    auto expected = { "Point", "name" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase13()
{
    std::string code = R"raw(
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y
                                                 # line 5

name = "foo"

if __name__ == "__main__":
    p = point(1, 2)
    
#   ^
#   |
#   complete at up-arrow (notice the identation spaces above)
)raw";

    lineCol_ = { 11, 4 };
    auto expected = { "Point", "name", "p" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase14()
{
    std::string code = R"raw(
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y
                                                 # line 5

name = "foo"

def f():
    local = 10
    
#   ^
#   |
#   complete at up-arrow (notice the identation spaces above)
)raw";

    lineCol_ = { 11, 4 };
    auto expected = { "Point", "name", "f", "local" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase15()
{
    std::string code = R"raw(
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y
                                                 # line 5

name = "foo"

def f():
    local = 10
      
#     ^
#     |
#     complete at up-arrow (notice indentation slightly misaligned ahead)
)raw";

    lineCol_ = { 11, 6 };
    auto expected = { "Point", "name", "f", "local" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase16()
{
    std::string code = R"raw(
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y
                                                 # line 5

name = "foo"

def f():
    local = 10
  
# ^
# |
# complete at up-arrow (notice indentation slightly misaligned behind)
)raw";

    lineCol_ = { 11, 2 };
    auto expected = { "Point", "name", "f" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase17()
{
    std::string code = R"raw(
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y
                                                 # line 5

name = "foo"

def f():
    local = 10

# complete above at line start
)raw";

    lineCol_ = { 11, 0 };
    auto expected = { "Point", "name", "f" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase18()
{
    std::string code = R"raw(
from fibo import *

def f():
    local = 10
    
#   ^
#   |
#   complete at up-arrow
)raw";

    lineCol_ = { 5, 4 };
    auto expected = { "f", "local", "fib", "fib2" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase19()
{
    std::string code = R"raw(
from fibo import *

def f():
    
#   ^
#   |
#   complete at up-arrow (notice indentation spaces above)
)raw";

    lineCol_ = { 4, 4 };
    auto expected = { "f", "fib", "fib2" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase20()
{
    std::string code = R"raw(
from fibo import *

def f():
  
# ^
# |
# complete at up-arrow (notice indentation spaces above)
)raw";

    lineCol_ = { 4, 2 };
    auto expected = { "f", "fib", "fib2" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase21()
{
    std::string code = R"raw(
class A:
    a = 1

class B(A):
    b = 2

b = B()
b.
# ^
# |
# complete at up-arrow
)raw";

    lineCol_ = { 8, 2 };
    auto expected = { "a", "b" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase22()
{
    std::string code = R"raw(
class A:
    a = 1

class B(A):
    b = 2

class C(B):
    c = 3

                                                 # line 10

c = C()
c.
# ^
# |
# complete at up-arrow
)raw";

    lineCol_ = { 13, 2 };
    auto expected = { "a", "b", "c" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase23()
{
    std::string code = R"raw(
class X:
    x = 1

class Y:
    y = 2

class Z(X,Y):
    z = 3

                                                 # line 10

z = Z()
z.
# ^
# |
# complete at up-arrow
)raw";

    lineCol_ = { 13, 2 };
    auto expected = { "x", "y", "z" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

namespace {

std::vector<const char*>& addRootObjectNames(std::vector<const char*>& v)
{
    auto names { "__gt__", "__new__", "__getattribute__", "__str__",
                 "__format__", "__ge__", "__lt__", "__repr__", "__setattr__",
                 "__delattr__", "__ne__", "__getattr__", "__init__",
                 "__del__", "__le__", "__bool__", "__eq__", "__hash__" };
    v.insert(v.begin(), names.begin(), names.end());
    return v;
}

std::vector<const char*>& addConstructorNames(std::vector<const char*>& v)
{
    auto names { "float", "bool", "int", "complex" };
    v.insert(v.begin(), names.begin(), names.end());
    return v;
}

std::vector<const char*>& addGlobalFuncNames(std::vector<const char*>& v)
{
    auto names {
        "abs",
        "all",
        "any",
        "ascii",
        "bin",
        "bytearray",
        "bytes",
        "callable",
        "chr",
        "classmethod",
        "compile",
        "delattr",
        "dir",
        "divmod",
        "enumerate",
        "eval",
        "exec",
        "filter",
        "format",
        "getattr",
        "globals",
        "hasattr",
        "hash",
        "help",
        "id",
        "input",
        "isinstance",
        "issubclass",
        "iter",
        "len",
        "locals",
        "map",
        "max",
        "min",
        "next",
        "oct",
        "open",
        "ord",
        "pow",
        "property",
        "range",
        "repr",
        "reversed",
        "round",
        "setattr",
        "slice",
        "sorted",
        "staticmethod",
        "sum",
        "super",
        "type",
        "vars",
        "zip",
        "__import__"
    };
    v.insert(v.begin(), names.begin(), names.end());
    return v;
}

std::vector<const char*>& addIntObjectNames(std::vector<const char*>& v)
{
    auto names {
        // Unary
        "__pos__",
        "__neg__",
        "__abs__",
        "__invert__",
        "__round__",
        "__floor_",
        "__ceil__",
        "__trunc__",

        // Binary
        "__add__",
        "__sub__",
        "__mul__",
        "__floordiv__",
        "__div__",
        "__truediv__",
        "__mod__",
        "__divmod__",
        "__pow__",
        "__lshift__",
        "__rshift__",
        "__rand__",
        "__ror__",
        "__rxor__",
        "__radd__",
        "__rsub__",
        "__rmul__",
        "__rfloordiv__",
        "__rdiv__",
        "__rtruediv__",
        "__rmod__",
        "__rdivmod__",
        "__rpow__",
        "__rlshift__",
        "__rrshift__",
        "__rand__",
        "__ror__",
        "__rxor__",

        // Augmented assignment
        "__iadd__",
        "__isub__",
        "__imul__",
        "__ifloordiv__",
        "__idiv__",
        "__itruediv__",
        "__imod__",
        "__idivmod__",
        "__ipow__",
        "__ilshift__",
        "__irshift__",
        "__iand__",
        "__ior__",
        "__ixor__",

        // Extra
        "bit_length",
        "to_bytes",
        "from_bytes"
    };
    v.insert(v.begin(), names.begin(), names.end());
    return v;
}

} // anonymous

void CompletionProposer::CompletionProposerTest::PyTestCase24()
{
    std::string code = R"raw(
i = 10
i.
# ^
# |
# complete at up-arrow
)raw";

    lineCol_ = { 2, 2 };
    disableBuiltins_ = false;
    std::vector<const char*> expected;
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py",
            addRootObjectNames(addIntObjectNames(expected)));
}

void CompletionProposer::CompletionProposerTest::PyTestCase25()
{
    std::string code = R"raw(
i = int(10)
i.
# ^
# |
# complete at up-arrow
)raw";

    lineCol_ = { 2, 2 };
    disableBuiltins_ = false;
    std::vector<const char*> expected;
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py",
            addRootObjectNames(addIntObjectNames(expected)));
}

void CompletionProposer::CompletionProposerTest::PyTestCase26()
{
    std::string code = R"raw(
i =
#  ^
#  |
#  complete at up-arrow
)raw";

    lineCol_ = { 1, 3 };
    disableBuiltins_ = false;
    std::vector<const char*> expected { "i", "object" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py",
            addGlobalFuncNames(addConstructorNames(expected)));
}

void CompletionProposer::CompletionProposerTest::PyTestCase27()
{
    std::string code = R"raw(
o = object()
o.
# ^
# |
# complete at up-arrow
)raw";

    lineCol_ = { 2, 2 };
    disableBuiltins_ = false;
    std::vector<const char*> expected;
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py",
            addRootObjectNames(expected));
}

void CompletionProposer::CompletionProposerTest::PyTestCase28()
{
    std::string code = R"raw(
class Z:
    z = 3

                                                 # line 4

z = Z()
z.
# ^
# |
# complete at up-arrow
)raw";

    lineCol_ = { 7, 2 };
    disableBuiltins_ = false;
    std::vector<const char*> expected = { "z" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py",
            addRootObjectNames(expected));
}

void CompletionProposer::CompletionProposerTest::PyTestCase29()
{
    std::string code = R"raw(
class X:
    x = 1

class Y:
    y = 2

class Z(X,Y):
    z = 3

                                                 # line 10

z = Z()
z.
# ^
# |
# complete at up-arrow
)raw";

    lineCol_ = { 13, 2 };
    disableBuiltins_ = false;
    std::vector<const char*> expected = { "x", "y", "z" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py",
            addRootObjectNames(expected));
}

void CompletionProposer::CompletionProposerTest::PyTestCase30()
{
    std::string code = R"raw(
from fibo import fib, fib2

# complete at first column above
)raw";

    lineCol_ = { 2, 0 };
    auto expected = { "fib", "fib2" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase31()
{
    std::string code = R"raw(
from fibo import fib

# complete at first column above
)raw";

    lineCol_ = { 2, 0 };
    auto expected = { "fib" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase32()
{
    std::string code = R"raw(
import sound.effects.echo

# complete at first column above
)raw";

    lineCol_ = { 2, 0 };
    auto expected = { "sound.effects.echo" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase33()
{
    std::string code = R"raw(
from sound.effects import echo

# complete at first column above
)raw";

    lineCol_ = { 2, 0 };
    auto expected = { "echo" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase34()
{
    std::string code = R"raw(
from sound.effects import echo, other

# complete at first column above
)raw";

    lineCol_ = { 2, 0 };
    auto expected = { "echo", "other" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase35()
{
    UAISO_SKIP_TEST;

    std::string code = R"raw(
from sound.effects.echo import echofilter

# complete at first column above
)raw";

    lineCol_ = { 2, 0 };
    auto expected = { "echofilter" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase36()
{
    std::string code = R"raw(
import fibo

# complete at first column above
)raw";

    lineCol_ = { 2, 0 };
    auto expected = { "fibo" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase37()
{
    std::string code = R"raw(
import fibo as m

# complete at first column above
)raw";

    lineCol_ = { 2, 0 };
    auto expected = { "m" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase38()
{
    std::string code = R"raw(
from fibo import fib as f

# complete at first column above
)raw";

    lineCol_ = { 2, 0 };
    auto expected = { "f" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase39()
{
    std::string code = R"raw(
from sound.effects import echo
echo.
#    ^
#    |
#    complete at up-arrow
)raw";

    lineCol_ = { 2, 5 };
    auto expected = { "echofilter" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase40()
{
    std::string code = R"raw(
from sound.effects import echo, other
echo.
#    ^
#    |
#    complete at up-arrow
)raw";

    lineCol_ = { 2, 5 };
    auto expected = { "echofilter" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase41()
{
    std::string code = R"raw(
from sound.effects import echo, other
other.
#     ^
#     |
#     complete at up-arrow
)raw";

    lineCol_ = { 2, 6 };
    auto expected = { "otherfilter" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase42()
{
    std::string code = R"raw(
import fibo
import dingo

# complete at first column above
)raw";

    lineCol_ = { 3, 0 };
    auto expected = { "fibo", "dingo" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase43()
{
    std::string code = R"raw(
import fibo

import dingo

import hypo


# complete at first column above
)raw";

    lineCol_ = { 7, 0 };
    auto expected = { "fibo", "dingo", "hypo" };
    runCore(FactoryCreator::create(LangName::Py), code, "/test.py", expected);
}

void CompletionProposer::CompletionProposerTest::PyTestCase44()
{
}

void CompletionProposer::CompletionProposerTest::PyTestCase45()
{
}

void CompletionProposer::CompletionProposerTest::PyTestCase46()
{
}

void CompletionProposer::CompletionProposerTest::PyTestCase47()
{
}

void CompletionProposer::CompletionProposerTest::PyTestCase48()
{
}

void CompletionProposer::CompletionProposerTest::PyTestCase49()
{
}
