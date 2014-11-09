Modified Nodal Analysis
=======================
[![Build Status](https://travis-ci.org/dhiana/mna.svg?branch=master)](https://travis-ci.org/dhiana/mna) [![Coverity Scan Build Status](https://scan.coverity.com/projects/3453/badge.svg)](https://scan.coverity.com/projects/3453) 

Cross-platform version of MNA1 and MNA1AMP programs by Prof. Moreirão available at
[http://www.coe.ufrj.br/~acmq/cursos/](http://www.coe.ufrj.br/~acmq/cursos/)


Compiling
---------

You may use g++:

    $ g++ -Wall src/MNA1.C -o bin/MNA1 -lncurses
    $ g++ -Wall src/MNA1AMP.C -o bin/MNA1AMP -lncurses

You may also use clang:

    $ clang src/MNA1AMP.C -o bin/MNA1AMP -lncurses

Or, if you have Scons installed:

    $ scons

And if you are using Windows... Feel free to use Visual Studio!

    1 - New project
    2 - Visual C++ -> Empty Project
    3 - Name: "mna"
    4 - Location: Onde estiver este repositório!
    5 - Right click on src/MNA.C
    6 - Include in Project

But apperently strcpy is unsafe...

    1 - Project Properties
    2 - Configuration Properties
    3 - C/C++
    4 - Preprocessor
    5 - Preprocessor Definitions
    6 - Edit
    7 - Add _CRT_SECURE_NO_WARNINGS

Running
-------

At Linux:

    $ ./bin/MNA1
    $ ./bin/MNA1AMP

Or at Visual Studio:

    F5 (Start Debugging!)

When prompted, try using data/simples.net and see MNA in action!
