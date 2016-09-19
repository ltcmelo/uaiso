# Uaiso

Uaiso is a parsing infrastructure that provides a unifying AST (Abstract Syntax Interface) and semantic services across programming languages. Ideally it can be used to:

* Produce code completion on IDEs/text editors
* Perform static analysis on programs
* Generate LLVM code (or any other)

## Supported languages

Language | Version | Parser | AST | Semantics | Active | Demo
---------|---------|--------|-----|-----------|--------|------
D | 2.0 | bison, GLR | partial | ok | no | [D completion](https://youtu.be/71aqIwv3vJs)
Go | 1.5+ | bison, GLR | complete | ok | yes | [Go completion](https://youtu.be/nUpcVBAw0DM)
Haskell | 2010 | hand, LL(k) | WIP | ? | yes | -
Python | 2.x | hand, LL(1) | complete | basic | yes | [Overall intro](https://youtu.be/71aqIwv3vJs)

## Prerequisities

The source is written in standard C++. You should be able to build it anywhere, but I'm developing on Clang/OSX. Make sure to use --recursive to clone.

* Cmake
* Python
* Flex/Bison
* C++14 compiler

Obs: There's code relying on Unix-like paths, which I need to work on for Windows.

## Plugins

Uaiso is a library. In order to use it within an IDE/text editor you need to write a plugin. There's an experimental one available for Qt Creator: https://github.com/ltcmelo/uaiso-plugins
