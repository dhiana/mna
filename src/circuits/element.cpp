#include "consts.h"
#include "circuits/element.h"
#include "matrix/matrix.h"
#include <sstream>
#include <cstdlib>
#include <set>
#include <math.h>

//TODO: not to use cout inside Element class
#include <iostream>
//

Element::Element()
{
}

Element::Element(string netlistLine,
                 int &numNodes,
                 vector<string> &variablesList)
{
    stringstream sstream(netlistLine);
    char na[MAX_NAME], nb[MAX_NAME], nc[MAX_NAME], nd[MAX_NAME];

    sstream >> name;
    type = getType();
    sstream.str( string(netlistLine, name.size(), string::npos) );
    if (type=='R' || type=='I' || type=='V') {
        sstream >> na >> nb >> value;
        cout << name << " " << na << " " << nb << " " << value << endl;
        a = getNodeNumber(na, numNodes, variablesList);
        b = getNodeNumber(nb, numNodes, variablesList);
    }
    else if (type=='G' || type=='E' || type=='F' || type=='H') {
        sstream >> na >> nb >> nc >> nd >> value;
        cout << name << " " << na << " " << nb << " " << nc << " "
             << nd << " "<< value << endl;
        a = getNodeNumber(na, numNodes, variablesList);
        b = getNodeNumber(nb, numNodes, variablesList);
        c = getNodeNumber(nc, numNodes, variablesList);
        d = getNodeNumber(nd, numNodes, variablesList);
    }
    else if (type=='O') {
        sstream >> na >> nb >> nc >> nd;
        cout << name << " " << na << " " << nb << " " << nc << " " << nd << " " << endl;
        a = getNodeNumber(na, numNodes, variablesList);
        b = getNodeNumber(nb, numNodes, variablesList);
        c = getNodeNumber(nc, numNodes, variablesList);
        d = getNodeNumber(nd, numNodes, variablesList);
    }
}

void Element::addCurrentVariables(int &numVariables, vector<string> &variablesList){
    if (type=='V' || type=='E' || type=='F' || type=='O') {
        numVariables++;
        if (numVariables>MAX_NODES) {
            cout << "As correntes extra excederam o numero de variaveis permitido (" << MAX_NODES << ")" << endl;
            exit(EXIT_FAILURE);
        }
        x=numVariables;
        variablesList[numVariables] = "j" + name;
    }
    else if (type=='H') {
        numVariables=numVariables+2;
        x=numVariables-1;
        y=numVariables;
        variablesList[numVariables-1] = "jx" + name;
        variablesList[numVariables] = "jy" + name;
    }
}

Element::Element(string name,
                 double value,
                 int a,
                 int b,
                 int c,
                 int d,
                 int x,
                 int y):
    name(name),
    value(value),
    a(a),
    b(b),
    c(c),
    d(d),
    x(x),
    y(y)
{
    type = getType();
    nonLinear = false;
}


Element::Element(string name,
                 vector<double> &params,
                 int a,
                 int b,
                 int c,
                 int d):
    name(name),
    params(params),
    a(a),
    b(b),
    c(c),
    d(d)
{
    type = getType();
    nonLinear = true;
}


void Element::applyStamp(double Yn[MAX_NODES+1][MAX_NODES+2],
                         const int &numVariables,
                         double previousSolution[MAX_NODES+1])
{
    double g;
    if (type=='R') {
        if (!nonLinear){
            g=1/value;
            Yn[a][a]+=g;
            Yn[b][b]+=g;
            Yn[a][b]-=g;
            Yn[b][a]-=g;
        } else {
            // TODO: Optimize!
            double G0=0;
            double I0=0;
            double Xn = previousSolution[a] - previousSolution[b];
            for (int i = 0; i < MAX_PARAMS; i++){
                G0 += params[i]*i*pow(Xn, i-1);
                I0 += params[i]*pow(Xn, i);
            }
            I0 -= G0*(Xn);
            Yn[a][a]+=G0;
            Yn[b][b]+=G0;
            Yn[a][b]-=G0;
            Yn[b][a]-=G0;
            Yn[a][numVariables+1]-=I0;
            Yn[b][numVariables+1]+=I0;
        }
    }
    else if (type=='G') {
        g=value;
        Yn[a][c]+=g;
        Yn[b][d]+=g;
        Yn[a][d]-=g;
        Yn[b][c]-=g;
    }
    else if (type=='I') {
        g=value;
        Yn[a][numVariables+1]-=g;
        Yn[b][numVariables+1]+=g;
    }
    else if (type=='V') {
        Yn[a][x]+=1;
        Yn[b][x]-=1;
        Yn[x][a]-=1;
        Yn[x][b]+=1;
        Yn[x][numVariables+1]-=value;
    }
    else if (type=='E') {
        g=value;
        Yn[a][x]+=1;
        Yn[b][x]-=1;
        Yn[x][a]-=1;
        Yn[x][b]+=1;
        Yn[x][c]+=g;
        Yn[x][d]-=g;
    }
    else if (type=='F') {
        g=value;
        Yn[a][x]+=g;
        Yn[b][x]-=g;
        Yn[c][x]+=1;
        Yn[d][x]-=1;
        Yn[x][c]-=1;
        Yn[x][d]+=1;
    }
    else if (type=='H') {
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
    else if (type=='O') {
        Yn[a][x]+=1;
        Yn[b][x]-=1;
        Yn[x][c]+=1;
        Yn[x][d]-=1;
    }
}


int Element::getNodeNumber(const char *name,
                           int &numNodes,
                           vector<string> &variablesList)
{
    int i=0, achou=0;

    while (!achou && i<=numNodes)
        if (!(achou=!variablesList[i].compare(name))) i++;

    if (!achou) {
        if (numNodes==MAX_NODES) {
            cout << "Maximum number of nodes reached: " << MAX_NODES << endl;
            #if defined (WIN32) || defined(_WIN32)
            cin.get();
            #endif
            exit(EXIT_FAILURE);
        }
        numNodes++;
        variablesList[numNodes] = name;
        return numNodes; /* new node */
    }
    else {
        return i; /* known node */
    }
}

bool Element::isValidElement(const char &netlistLinePrefix){
    // Initializing set with tmpElementPrefixes
    char tmpElementPrefixes[] = {
        'R', 'I', 'V', 'G', 'E', 'F', 'H', 'O'
    };
    set<char> elementPrefixes(
        tmpElementPrefixes,
        tmpElementPrefixes + 
            sizeof(tmpElementPrefixes) / 
            sizeof(tmpElementPrefixes[0])
    );

    // Returns whether or not line prefix corresponds to valid Element
    // verifying if netlistLinePrefix is in elementPrefixes
    return elementPrefixes.find(netlistLinePrefix) != elementPrefixes.end();
}

string Element::getName() const
{
    return name;
}

char Element::getType() const
{
    return name[0];
}

bool Element::isNonLinear() const
{
    return nonLinear;
}
