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

Element::Element(string netlistLine,
                 int &numElements,
                 int &numVariables,
                 vector<string> &list)
{
    stringstream sstream(netlistLine);
    char na[MAX_NAME], nb[MAX_NAME], nc[MAX_NAME], nd[MAX_NAME];

    sstream >> name;
    setType(name[0]);
    sstream.str( string(netlistLine, name.size(), string::npos) );
    if (type=='R' || type=='I' || type=='V') {
        sstream >> na >> nb >> value;
        cout << name << " " << na << " " << nb << " " << value << endl;
        a = number(na, numVariables, list);
        b = number(nb, numVariables, list);
    }
    else if (type=='G' || type=='E' || type=='F' || type=='H') {
        sstream >> na >> nb >> nc >> nd >> value;
        cout << name << " " << na << " " << nb << " " << nc << " "
             << nd << " "<< value << endl;
        a = number(na, numVariables, list);
        b = number(nb, numVariables, list);
        c = number(nc, numVariables, list);
        d = number(nd, numVariables, list);
    }
    else if (type=='O') {
        sstream >> na >> nb >> nc >> nd;
        cout << name << " " << na << " " << nb << " " << nc << " " << nd << " " << endl;
        a = number(na, numVariables, list);
        b = number(nb, numVariables, list);
        c = number(nc, numVariables, list);
        d = number(nd, numVariables, list);
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

Element::Element(string name,
                 double value,
                 int a,
                 int b,
                 int c,
                 int d,
                 int x,
                 int y)
{
    setType(name[0]);
    setName(name);
    this->value = value;
    this->a = a;
    this->b = b;
    this->c = c;
    this->d = d;
    this->x = x;
    this->y = y;
}

void Element::applyStamp(double Yn[MAX_NODES+1][MAX_NODES+2],
                         const int &numVariables){
    double g;
    char tipo;
    tipo=getType();
    if (tipo=='R') {
        g=1/value;
        Yn[a][a]+=g;
        Yn[b][b]+=g;
        Yn[a][b]-=g;
        Yn[b][a]-=g;
    }
    else if (tipo=='G') {
        g=value;
        Yn[a][c]+=g;
        Yn[b][d]+=g;
        Yn[a][d]-=g;
        Yn[b][c]-=g;
    }
    else if (tipo=='I') {
        g=value;
        Yn[a][numVariables+1]-=g;
        Yn[b][numVariables+1]+=g;
    }
    else if (tipo=='V') {
        Yn[a][x]+=1;
        Yn[b][x]-=1;
        Yn[x][a]-=1;
        Yn[x][b]+=1;
        Yn[x][numVariables+1]-=value;
    }
    else if (tipo=='E') {
        g=value;
        Yn[a][x]+=1;
        Yn[b][x]-=1;
        Yn[x][a]-=1;
        Yn[x][b]+=1;
        Yn[x][c]+=g;
        Yn[x][d]-=g;
    }
    else if (tipo=='F') {
        g=value;
        Yn[a][x]+=g;
        Yn[b][x]-=g;
        Yn[c][x]+=1;
        Yn[d][x]-=1;
        Yn[x][c]-=1;
        Yn[x][d]+=1;
    }
    else if (tipo=='H') {
        g=value;
        Yn[a][y]+=1;
        Yn[b][y]-=1;
        Yn[c][x]+=1;
        Yn[d][x]-=1;
        Yn[y][a]-=1;
        Yn[y][b]+=1;
        Yn[x][c]-=1;
        Yn[x][d]+=1;
        Yn[y][x]+=g;
    }
    else if (tipo=='O') {
        Yn[a][x]+=1;
        Yn[b][x]-=1;
        Yn[x][c]+=1;
        Yn[x][d]-=1;
    }
}

/** Rotina que conta os nos e atribui Element::numbers a eles */
int Element::number(const char *name,
                    int &numVariables,
                    vector<string> &list)
{
    int i=0, achou=0;

    while (!achou && i<=numVariables)
        if (!(achou=!list[i].compare(name))) i++;
    if (!achou) {
        if (numVariables==MAX_NODES) {
            cout << "O programa so aceita ate " << numVariables <<  " nos" << endl;
            #if defined (WIN32) || defined(_WIN32)
            cin.get();
            #endif
            exit(EXIT_FAILURE);
        }
        numVariables++;
        list[numVariables] = name;
        return numVariables; /* novo no */
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


