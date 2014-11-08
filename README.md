Modified Nodal Analysis
=======================

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

Running
-------

    $ ./bin/MNA1
    $ ./bin/MNA1AMP
