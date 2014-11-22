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
    Element(string name, double value, int a, int b, int c = 0, int d = 0, int x = 0, int y = 0);

    //TODO: retirar definicao daqui e de main.cpp
    static const int MAX_NOME = 11;

    static int number(const char *name, int &nv, vector<string> &list);

    double valor;
    int a,b,c,d,x,y;

    char getType() const;
    void setType(char value);

    string getName() const;
    void setName(const string &value);

private:
    char type;
    string name;

};

#endif // ELEMENT_H
