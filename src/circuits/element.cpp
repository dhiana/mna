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
                 vector<string> &variablesList):
    polynomial(false)
{
    stringstream sstream(netlistLine);
    char na[MAX_NAME], nb[MAX_NAME], nc[MAX_NAME], nd[MAX_NAME];

    vector<double> _params(MAX_PARAMS);
    for (int i=0; i<MAX_PARAMS; i++){
        _params[i] = 0;
    }

    sstream >> name;
    cout << name << " ";
    type = getType();
    sstream.str( string(netlistLine, name.size(), string::npos) );
    if (type=='R' || type=='I' || type=='V') {
        sstream >> na >> nb;
        cout << na << " " << nb << " ";
        for (int i = 0; i < MAX_PARAMS; i++){
            sstream >> _params[i];
            if (i>=1 && _params[i]!=0)
                polynomial = true;
        }
        if (!polynomial){
            value = _params[0];
            cout << value << endl;
        }
        else {
            params = _params;
            for (int i = 0; i < MAX_PARAMS; i++){
                cout << params[i] << " ";
            }
            cout << endl;
        }
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
    y(y),
    polynomial(false)
{
    type = getType();
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
    d(d),
    polynomial(true)
{
    type = getType();
}


void Element::calcNewtonRaphsonParameters(const double &Xn){
    dFx = params[1];
    FxMinusdFxTimesXn = params[0];
    for (int i = 2; i < MAX_PARAMS; i++){
        dFx += params[i]*(i)*pow(Xn, i-1);
        FxMinusdFxTimesXn -= params[i]*(i-1)*pow(Xn, i);
    }
}


void Element::applyStamp(double Yn[MAX_NODES+1][MAX_NODES+2],
                         const int &numVariables,
                         double previousSolution[MAX_NODES+1],
                         double t,
                         double lastStepSolution[MAX_NODES+1])
{
    if (type=='R') {
        double G;
        if (!polynomial){
            G=1/value;
        } else {
            double Vab = previousSolution[a]-previousSolution[b];
            calcNewtonRaphsonParameters(Vab);
            G = dFx;
            double I0;
            I0 = FxMinusdFxTimesXn;
            Yn[a][numVariables+1]-=I0;
            Yn[b][numVariables+1]+=I0;
        }
        Yn[a][a]+=G;
        Yn[b][b]+=G;
        Yn[a][b]-=G;
        Yn[b][a]-=G;
    }
    else if (type=='G') {
        double G;
        if (!polynomial){
            G=value;
        } else {
            double Vcd = previousSolution[c]-previousSolution[d];
            calcNewtonRaphsonParameters(Vcd);
            G = dFx;
            double I0 = FxMinusdFxTimesXn;
            Yn[a][numVariables+1]-=I0;
            Yn[b][numVariables+1]+=I0;
        }
        Yn[a][c]+=G;
        Yn[b][d]+=G;
        Yn[a][d]-=G;
        Yn[b][c]-=G;
    }
    else if (type=='I') {
        double I=value;
        Yn[a][numVariables+1]-=I;
        Yn[b][numVariables+1]+=I;
    }
    else if (type=='V') {
        double V=value;
        Yn[a][x]+=1;
        Yn[b][x]-=1;
        Yn[x][a]-=1;
        Yn[x][b]+=1;
        Yn[x][numVariables+1]-=V;
    }
    else if (type=='E') {
        // Voltage Amplifier
        double A;
        double k;
        if (!polynomial){
            A=value;
            k=1;
        } else {
            double Vcd = previousSolution[c]-previousSolution[d];
            calcNewtonRaphsonParameters(Vcd);
            A = dFx;
            double V0 = FxMinusdFxTimesXn;
            Yn[x][numVariables+1]-=V0;
            k=2;
        }
        Yn[a][x]+=k*1;
        Yn[b][x]-=k*1;
        Yn[x][a]-=k*1;
        Yn[x][b]+=k*1;
        Yn[x][c]+=A;
        Yn[x][d]-=A;
    }
    else if (type=='F') {
        // Current Amplifier
        double B;
        if (!polynomial){
            B=value;
        } else {
            double Jcd = previousSolution[x];
            calcNewtonRaphsonParameters(Jcd);
            B=dFx;
            double I0 = FxMinusdFxTimesXn;
            Yn[a][numVariables+1]-=I0;
            Yn[b][numVariables+1]+=I0;
        }
        Yn[a][x]+=B;
        Yn[b][x]-=B;
        Yn[c][x]+=1;
        Yn[d][x]-=1;
        Yn[x][c]-=1;
        Yn[x][d]+=1;
    }
    else if (type=='H') {
        // CCVS: Transresistance
        double Rm;
        if (!polynomial){
            Rm=value;
        } else {
            double Jcd = previousSolution[y];
            calcNewtonRaphsonParameters(Jcd);
            Yn[a][x]+=1;
            Yn[b][x]-=1;
            Yn[x][a]-=1;
            Yn[x][b]+=1;
            Rm=dFx;
            double V0 = FxMinusdFxTimesXn;
            Yn[x][numVariables+1]-=V0;
        }
        Yn[a][y]+=1;
        Yn[b][y]-=1;
        Yn[c][x]+=1;
        Yn[d][x]-=1;
        Yn[y][a]-=1;
        Yn[y][b]+=1;
        Yn[x][c]-=1;
        Yn[x][d]+=1;
        Yn[y][x]+=Rm;
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
