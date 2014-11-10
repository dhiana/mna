Modified Nodal Analysis
=======================
[![Build Status](https://travis-ci.org/dhiana/mna.svg?branch=master)](https://travis-ci.org/dhiana/mna) [![Coverity Scan Build Status](https://scan.coverity.com/projects/3453/badge.svg)](https://scan.coverity.com/projects/3453) 

Cross-platform version of MNA1 program by Prof. Moreirão available at
[http://www.coe.ufrj.br/~acmq/cursos/](http://www.coe.ufrj.br/~acmq/cursos/)


MNA1?!
------

Of course! Modified Nodal Analysis! ;)


Compiling
---------

You may use g++:

    $ g++ -Wall src/mna.c -o bin/mna -lncurses

You may also use clang:

    $ clang src/mna.c -o bin/mna -lncurses

Or, if you have Scons installed:

    $ scons

And if you are using Windows... Feel free to use Visual Studio!

    1 - New project
    2 - Visual C++ -> Empty Project
    3 - Name: "mna"
    4 - Location: Onde estiver este repositório!
    5 - Right click on src/mna.c
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

    $ ./bin/mna

Or at Visual Studio:

    F5 (Start Debugging!)

When prompted, try using data/simples.net and see mna in action!
