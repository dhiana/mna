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
            int &numNodes,
            vector<string> &variablesList);

    Element(string name,
            double value,
            int a,
            int b,
            int c = 0,
            int d = 0,
            int x = 0,
            int y = 0);

    Element(string name,
            vector<double> &params, 
            int a,
            int b,
            int c=0,
            int d=0);

    void applyStamp(double Yn[MAX_NODES+1][MAX_NODES+2],
                    const int &numVariables,
                    double previousSolution[MAX_NODES+1]=ZERO_SOLUTION);

    static int getNodeNumber(const char *name,
                             int &numNodes,
                             vector<string> &variablesList);

    void addCurrentVariables(int &numVariables,
                             vector<string> &variablesList);

    string getName() const;
    bool isNonLinear() const;

    static bool isValidElement(const char &netlistLinePrefix);

private:

    char type;
    string name;
    double value;
    vector<double> params;
    int a,b,c,d,x,y;
    bool nonLinear;

    char getType() const;

};

#endif // ELEMENT_H
