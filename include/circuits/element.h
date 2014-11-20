#ifndef ELEMENT_H
#define ELEMENT_H

#include <string>
#include <vector>

using namespace std;

class Element
{
public:
    Element();

    static int number(const char *name, int &nv, vector<string> &list);

    string nome;
    double valor;
    int a,b,c,d,x,y;
};

#endif // ELEMENT_H
