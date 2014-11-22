#include "consts.h"
#include "circuits/element.h"
#include "matrix/matrix.h"
#include <sstream>
#include <cstdlib>

//TODO: not to use cout inside Element class
#include <iostream>
//

Element::Element()
{
}

Element::Element(string netlistLine, int &numElements,  int &nv, vector<string> &list)
{
    stringstream sstream(netlistLine);
    char na[MAX_NOME], nb[MAX_NOME], nc[MAX_NOME], nd[MAX_NOME];

    sstream >> name;
    setType(name[0]);
    sstream.str( string(netlistLine, name.size(), string::npos) );
    if (type=='R' || type=='I' || type=='V') {
        sstream >> na >> nb >> valor;
        cout << name << " " << na << " " << nb << " " << valor << endl;
        a = number(na, nv, list);
        b = number(nb, nv, list);
    }
    else if (type=='G' || type=='E' || type=='F' || type=='H') {
        sstream >> na >> nb >> nc >> nd >> valor;
        cout << name << " " << na << " " << nb << " " << nc << " "
             << nd << " "<< valor << endl;
        a = number(na, nv, list);
        b = number(nb, nv, list);
        c = number(nc, nv, list);
        d = number(nd, nv, list);
    }
    else if (type=='O') {
        sstream >> na >> nb >> nc >> nd;
        cout << name << " " << na << " " << nb << " " << nc << " " << nd << " " << endl;
        a = number(na, nv, list);
        b = number(nb, nv, list);
        c = number(nc, nv, list);
        d = number(nd, nv, list);
    }
    else if (type=='*') { /* Comentario comeca com "*" */
        cout << "Comentario: " << netlistLine;
        numElements--;
    }
    else {
        cout << "Elemento desconhecido: " << netlistLine << endl;
        #if defined (WIN32) || defined(_WIN32)
        cin.get();
        #endif
        exit(EXIT_FAILURE);
    }
}

Element::Element(string name, double value, int a, int b, int c, int d, int x, int y)
{
    setType(name[0]);
    setName(name);
    this->valor = value;
    this->a = a;
    this->b = b;
    this->c = c;
    this->d = d;
    this->x = x;
    this->y = y;
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

char Element::getType() const
{
    return type;
}

void Element::setType(char value)
{
    //TODO: comparar com tipos permitidos
    type = toupper(value);
}

string Element::getName() const
{
    return name;
}

void Element::setName(const string &value)
{
    name = value;
}


