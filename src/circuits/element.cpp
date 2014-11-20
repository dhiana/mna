#include "circuits/element.h"

#include "matrix/solve.h"

//TODO: not to use cout inside Element class
#include <iostream>
//

Element::Element()
{
}

/** Rotina que conta os nos e atribui Element::numbers a eles */
int Element::number(const char *name, int &nv, vector<string> &list)
{
    int i=0, achou=0;

    while (!achou && i<=nv)
        if (!(achou=!list[i].compare(name))) i++;
    if (!achou) {
        if (nv==MAX_NOS) {
            cout << "O programa so aceita ate " << nv <<  " nos" << endl;
            #if defined (WIN32) || defined(_WIN32)
            cin.get();
            #endif
            exit(EXIT_FAILURE);
        }
        nv++;
        list[nv] = name;
        return nv; /* novo no */
    }
    else {
        return i; /* no ja conhecido */
    }
}
