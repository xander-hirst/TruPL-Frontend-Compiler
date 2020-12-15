# TruPL-Frontend-Compiler
A compiler for a rudimentary programming language called TruPL

This is the frontend (buffer, lexical analyzer, parser, and semantic analyzer) of the TruPL compiler.
This progam takes an input text file and determines if the source code is valid TruPL.


    -./truc t1.txt
    
The Buffer (buffer.cpp) is responsible for removing all unnecessary whitespace and comments from the source text file.

The lexical analyzer determines which tokens are present in the source file by advancing character by character through the buffer.

The parser is a recursive descent parser that attempts to build a valid parse tree from the tokens found in the lexical analysis step of compilation.

The semantic analyzer is part of the parser that ensures proper programming practices are followed such as declaring variables before they are used, type checking, and so on. 
