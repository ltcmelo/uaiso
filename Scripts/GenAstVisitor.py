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

import sys
import os.path


_file_header = (
    "/******************************************************************************\n"
    " * Copyright (c) 2014-2016 Leandro T. C. Melo (ltcmelo@gmail.com)\n"
    " *\n"
    " * This library is free software; you can redistribute it and/or\n"
    " * modify it under the terms of the GNU Lesser General Public\n"
    " * License as published by the Free Software Foundation; either\n"
    " * version 2.1 of the License, or (at your option) any later version.\n"
    " *\n"
    " * This library is distributed in the hope that it will be useful,\n"
    " * but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    " * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n"
    " * Lesser General Public License for more details.\n"
    " *\n"
    " * You should have received a copy of the GNU Lesser General Public\n"
    " * License along with this library; if not, write to the Free Software\n"
    " * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301\n"
    " * USA\n"
    " *****************************************************************************/\n"
    "\n"
    "/*---------------------------------------------------------------*/\n"
    "/*---                    The UaiSo! Project                   ---*/\n"
    "/*---                                                         ---*/\n"
    "/*---         This file was AUTOMATICALLY GENERATED           ---*/\n"
    "/*--- It can be AUTOMATICALLY UPDATED if the formatting/style ---*/\n"
    "/*--- of the method declarations is preserved (it's fragile). ---*/\n"
    "/*---------------------------------------------------------------*/\n"
    "\n"
)


def write_visitor_file(header, content_h, impl, content_cpp):
    """ Write the AST Visitor file """

    print "Writing files %s and %s" % (header, impl)

    with open(header, "w") as f:
        f.write(content_h)
    with open(impl, "w") as f:
        f.write(content_cpp)


def run():
    if len(sys.argv) < 2:
        print ("Usage: python Scripts/GenAstVisitor.py Path/AstVisitor "
               "[Dump|Serial|None] [traverse|visit]")
        return

    visitor_path = sys.argv[1]
    dump_ast = False
    serial_ast = False
    method = "visit"

    if len(sys.argv) > 2:
        if sys.argv[2] == "Dump":
            dump_ast = True
        elif sys.argv[2] == "Serial":
            serial_ast = True
        elif sys.argv[2] != "None":
            print "Invalid option: %s" % sys.argv[2]
            return

    if len(sys.argv) > 3:
        method = sys.argv[3]

    header = visitor_path + ".h"
    impl = visitor_path + ".cpp"

    is_create = True
    if os.path.isfile(header):
        print "Running in UPDATE mode"
        is_create = False
    else:
        print "Running in CREATE mode"

    visitor = visitor_path.split('/')
    visitor = visitor[len(visitor) - 1]

    if is_create:
        # Class declaration
        contents_h = _file_header

        contents_h += (
            "#ifndef UAISO_%s_H__\n"
            "#define UAISO_%s_H__\n"
            "\n"
        ) % (visitor.upper(), visitor.upper())

        contents_h += (
            "#include \"Ast/Ast.h\"\n"
            "#include \"Ast/AstVisitor.h\"\n"
            "\n"
            "namespace uaiso {\n"
            "\n"
        )

        # Class definition.
        contents_h += (
            "class %s final : public AstVisitor<%s>\n"
            "{\n"
            "private:\n"
        ) % (visitor, visitor)

        # Typically needed...
        contents_h += (
            "    friend class AstVisitor<%s>;\n"
            "    using Base = AstVisitor<%s>;\n"
        ) % (visitor, visitor)

        # Basic implementation file.
        contents_cpp = _file_header
        contents_cpp += (
            "#include \"%s.h\"\n"
            "\n"
            "using namespace uaiso;\n"
            "\n"
        ) % visitor_path

    else:
        with open(header, "r") as f:
            contents_h = f.read()
        with open(impl, "r") as f:
            contents_cpp = open(impl, "r").read()

    ast_kind = None
    defs_file = "Ast/AstDefs.h"
    with open(defs_file, "r") as f:
        for line in f:
            line = line.strip()

            if line.startswith("#define NAME_AST_MIXIN"):
                ast_kind = "Name"
                if is_create:
                    contents_h += "\n    // AST names\n"
                continue
            elif line.startswith("#define SPEC_AST_MIXIN"):
                ast_kind = "Spec"
                if is_create:
                    contents_h += "\n    // AST specifiers\n"
                continue
            elif line.startswith("#define DECOR_AST_MIXIN"):
                ast_kind = "Decor"
                if is_create:
                    contents_h += "\n    // AST decorators\n"
                continue
            elif line.startswith("#define DECL_AST_MIXIN"):
                ast_kind = "Decl"
                if is_create:
                    contents_h += "\n    // AST declarations\n"
                continue
            elif line.startswith("#define EXPR_AST_MIXIN"):
                ast_kind = "Expr"
                if is_create:
                    contents_h += "\n    // AST expressions\n"
                continue
            elif line.startswith("#define STMT_AST_MIXIN"):
                ast_kind = "Stmt"
                if is_create:
                    contents_h += "\n    // AST statements\n"
                continue
            elif not line.startswith("GEN_CODE_FOR"):
                ast_kind = None
                continue

            # We're in a GEN_CODE_FOR line: GEN_CODE_FOR(AstNode, AstBase)
            if ast_kind:
                # A bit of string processing to extract the `AstNode`.
                line = line[len("GEN_CODE_FOR") + 1 :]
                line = line.split(',')
                if not len(line) > 1:
                    print "Ast/AstDefs.h file got messed up"
                ast_node = line[0]

                # Method prototype
                prototype = ("    VisitResult %s%s%s(%s%sAst* ast);\n") % (method,
                        ast_node, ast_kind, ast_node, ast_kind)

                # Method definition
                if dump_ast:
                    code = ('    *output__ << level << "%s%s" << std::endl;\n'
                            '    return Continue;\n') % (ast_node, ast_kind)
                elif serial_ast:
                    code =  ('    *output__ << "(" << static_cast<size_t>(ast->kind());\n')
                    code += ("    Base::%s%s%s(ast);\n") % (method, ast_node, ast_kind)
                    code += ('    *output__ << ")";\n'
                             '    return Continue;\n')
                else:
                    code = '    return Continue;\n'
                definition = ("%s::VisitResult %s::%s%s%s(%s%sAst* ast)\n"
                              "{\n"
                              "%s"
                              "}\n"
                              "\n") % (visitor, visitor, method,
                                       ast_node, ast_kind, ast_node, ast_kind,
                                       code)

                if is_create:
                    contents_h += prototype
                    contents_cpp += definition
                else:
                    if contents_h.find(prototype) == -1:
                        print "Missing visit for %s" % ast_node
                        # Insert it to both header and implementation files.
                        contents_h = contents_h.replace("};",
                                                        "\n" + prototype + "};")
                        contents_cpp += definition

    if is_create:
        contents_h += (
            "};\n"
            "\n"
            "} // namespace uaiso\n"
            "\n"
            "#endif\n"
        )

    write_visitor_file(header, contents_h, impl, contents_cpp)


if __name__ == "__main__":
    run()
