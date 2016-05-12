# -----------------------------------------------------------------------------
# Copyright (c) 2014-2016 Leandro T. C. Melo (ltcmelo@gmail.com)
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
# USA
# -----------------------------------------------------------------------------

# -------------------------- #
# --- The UaiSo! Project --- #
# -------------------------- #

import os
import platform
import GenTokens
from subprocess import call


def gen_lexer(lang):
    if platform.system() == 'Windows':
        print "Calling win_flex on %s" % lang
        call(["win_flex", "--wincompat", "%s/%s.l" % (lang, lang)])
    else:
        print "Calling flex on %s" % lang
        call(["flex", "%s/%s.l" % (lang, lang)])
    os.rename("%sLexer.h" % lang, "%s/%sLexer.h" % (lang, lang))
    os.rename("%sLexer.cpp" % lang, "%s/%sLexer.cpp" % (lang, lang))


def gen_parser(lang):
    if platform.system() == 'Windows':
        print "Calling win_bison on %s" % lang
        call(["win_bison", "-d", "-v", "%s/%s.y" % (lang, lang)])
    else:
        print "Calling bison on %s" % lang
        call(["bison", "-d", "-v", "%s/%s.y" % (lang, lang)])
    os.rename("%sParser.h" % lang, "%s/%sParser.h" % (lang, lang))
    os.rename("%sParser.cpp" % lang, "%s/%sParser.cpp" % (lang, lang))
    os.rename("%sParser.output" % lang, "%s/%sParser.output" % (lang, lang))


def run():
    # First of all, we must update the tokens.
    GenTokens.run()
    # D
    gen_parser("D")
    gen_lexer("D")
    # Go
    gen_parser("Go")
    gen_lexer("Go")


if __name__ == "__main__":
    run()
