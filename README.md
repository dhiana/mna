Modified Nodal Analysis
=======================
[![Build Status](https://travis-ci.org/dhiana/mna.svg?branch=master)](https://travis-ci.org/dhiana/mna)

Cross-platform version of MNA1 program by Prof. Moreirão available at
[http://www.coe.ufrj.br/~acmq/cursos/](http://www.coe.ufrj.br/~acmq/cursos/)


Usage
-----

#### Executing the program with no arguments

    $ ./bin/mna
    # Will prompt for netlist file path

#### Executing the program giving netlist file path

    $ ./bin/mna data/simples.net


Compiling and Running
---------------------

### Linux

    $ scons
    $ ./bin/mna

### Visual Studio

1. Open mna.sln
2. Run (F5)!


Running the tests!
------------------

### Linux

1. Install gtest libs

        $ sudo yum install gtest-devel

2. Run tests:

        $ scons
        $ ./bin/test

### Visual Studio

1. Open mna.sln file

2. Configure Solution Startup

    Solution "mna" -> Properties -> Commom Properties -> Startup Properties

    On Multiple Startup Projects:

    * mna     -> Start

    * tests   -> Start without Debugging

3. Start (or F5)!

    Start will run both the program and the tests!
