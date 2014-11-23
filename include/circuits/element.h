#ifndef ELEMENT_H
#define ELEMENT_H

#include "consts.h"
#include <string>
#include <vector>

using namespace std;

class Element
{
public:
    Element();

    Element(string netlistLine,
            int &numElements,
            int &numVariables,
            vector<string> &list);

    Element(string name,
            double value,
            int a,
            int b,
            int c = 0,
            int d = 0,
            int x = 0,
            int y = 0);

    void applyStamp(double Yn[MAX_NODES+1][MAX_NODES+2],
                    const int &numVariables);

    static int number(const char *name,
                      int &numVariables,
                      vector<string> &list);

    double value;
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
