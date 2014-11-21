#ifndef ELEMENT_H
#define ELEMENT_H

#include <string>
#include <vector>

using namespace std;

class Element
{
public:
    Element();
    Element(string netlistLine, int &numElements, int &nv, vector<string> &list);

    //TODO: retirar definicao daqui e de main.cpp
    static const int MAX_NOME = 11;

    static int number(const char *name, int &nv, vector<string> &list);

    char type;
    string name;
    double valor;
    int a,b,c,d,x,y;
};

#endif // ELEMENT_H
