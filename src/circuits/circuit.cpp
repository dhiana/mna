#include "consts.h"
#include "circuits/circuit.h"
#include "circuits/element.h"
#include "matrix/matrix.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>

using namespace std;


int addCurrentVariablesToNetlist(int &numElements,
                                 int &numVariables,
                                 int &numNodes,
                                 vector<string> &variablesList,
                                 vector<Element> &netlist){
    char tipo;
    numNodes=numVariables;
    for (int i=1; i<=numElements; i++) {
        tipo=netlist[i].getType();
        if (tipo=='V' || tipo=='E' || tipo=='F' || tipo=='O') {
            numVariables++;
            if (numVariables>MAX_NODES) {
                cout << "As correntes extra excederam o numero de variaveis permitido (" << MAX_NODES << ")" << endl;
                return(EXIT_FAILURE);
            }
            variablesList[numVariables] = "j"; /* Tem espaco para mais dois caracteres */
            variablesList[numVariables].append( netlist[i].getName() );
            netlist[i].x=numVariables;
        }
        else if (tipo=='H') {
            numVariables=numVariables+2;
            if (numVariables>MAX_NODES) {
                cout << "As correntes extra excederam o numero de variaveis permitido (" << MAX_NODES << ")" << endl;
                return(EXIT_FAILURE);
            }
            variablesList[numVariables-1] = "jx";
            variablesList[numVariables-1].append(netlist[i].getName());
            netlist[i].x=numVariables-1;
            variablesList[numVariables] = "jy";
            variablesList[numVariables].append( netlist[i].getName() );
            netlist[i].y=numVariables;
        }
    }
    cout << endl;
    return 0;
}


int readElementsFromNetlist(int &numElements,
                            int &numVariables,
                            ifstream &netlistFile,
                            vector<string> &variablesList,
                            vector<Element> &netlist){
    string txt;
    getline(netlistFile, txt);
    cout << "Title: " << txt;
    while (getline(netlistFile, txt)) {
        numElements++; /* XXX Starts from netlist[1] */
        if (numElements>MAX_ELEMS) {
            cout << "Invalid number of elements. Maximum number of elements is " << MAX_ELEMS << endl;
            return(EXIT_FAILURE);
        }
        netlist[numElements] = Element(txt,
                                       numElements,
                                       numVariables,
                                       variablesList
                                      );
    }
    netlistFile.close();
    return 0;
}

void printVariables(const int &numVariables, const vector<string> &lista){
    for (int i=0; i<=numVariables; i++)
        cout << i << " -> " << lista[i] << endl;
    cout << endl;
}

void printSummary(int numNodes,
                  int numVariables,
                  int numElements){
    cout << "# nodes:\t" << numNodes << endl;
    cout << "# variables:\t" << numVariables << endl;
    cout << "# elements:\t" << numElements << endl;
    cout << endl;
}


void applyStamps(int numElements, int numVariables, vector<Element> netlist, double Yn[MAX_NODES+1][MAX_NODES+2]){
    double g;
    char tipo;
    for (int i=1; i<=numElements; i++) {
        tipo=netlist[i].getType();
        if (tipo=='R') {
            g=1/netlist[i].value;
            Yn[netlist[i].a][netlist[i].a]+=g;
            Yn[netlist[i].b][netlist[i].b]+=g;
            Yn[netlist[i].a][netlist[i].b]-=g;
            Yn[netlist[i].b][netlist[i].a]-=g;
        }
        else if (tipo=='G') {
            g=netlist[i].value;
            Yn[netlist[i].a][netlist[i].c]+=g;
            Yn[netlist[i].b][netlist[i].d]+=g;
            Yn[netlist[i].a][netlist[i].d]-=g;
            Yn[netlist[i].b][netlist[i].c]-=g;
        }
        else if (tipo=='I') {
            g=netlist[i].value;
            Yn[netlist[i].a][numVariables+1]-=g;
            Yn[netlist[i].b][numVariables+1]+=g;
        }
        else if (tipo=='V') {
            Yn[netlist[i].a][netlist[i].x]+=1;
            Yn[netlist[i].b][netlist[i].x]-=1;
            Yn[netlist[i].x][netlist[i].a]-=1;
            Yn[netlist[i].x][netlist[i].b]+=1;
            Yn[netlist[i].x][numVariables+1]-=netlist[i].value;
        }
        else if (tipo=='E') {
            g=netlist[i].value;
            Yn[netlist[i].a][netlist[i].x]+=1;
            Yn[netlist[i].b][netlist[i].x]-=1;
            Yn[netlist[i].x][netlist[i].a]-=1;
            Yn[netlist[i].x][netlist[i].b]+=1;
            Yn[netlist[i].x][netlist[i].c]+=g;
            Yn[netlist[i].x][netlist[i].d]-=g;
        }
        else if (tipo=='F') {
            g=netlist[i].value;
            Yn[netlist[i].a][netlist[i].x]+=g;
            Yn[netlist[i].b][netlist[i].x]-=g;
            Yn[netlist[i].c][netlist[i].x]+=1;
            Yn[netlist[i].d][netlist[i].x]-=1;
            Yn[netlist[i].x][netlist[i].c]-=1;
            Yn[netlist[i].x][netlist[i].d]+=1;
        }
        else if (tipo=='H') {
            g=netlist[i].value;
            Yn[netlist[i].a][netlist[i].y]+=1;
            Yn[netlist[i].b][netlist[i].y]-=1;
            Yn[netlist[i].c][netlist[i].x]+=1;
            Yn[netlist[i].d][netlist[i].x]-=1;
            Yn[netlist[i].y][netlist[i].a]-=1;
            Yn[netlist[i].y][netlist[i].b]+=1;
            Yn[netlist[i].x][netlist[i].c]-=1;
            Yn[netlist[i].x][netlist[i].d]+=1;
            Yn[netlist[i].y][netlist[i].x]+=g;
        }
        else if (tipo=='O') {
            Yn[netlist[i].a][netlist[i].x]+=1;
            Yn[netlist[i].b][netlist[i].x]-=1;
            Yn[netlist[i].x][netlist[i].c]+=1;
            Yn[netlist[i].x][netlist[i].d]-=1;
        }

#ifdef DEBUG
        cout << "System after stamp of " << netlist[i].getName() << endl;
        print(numVariables, Yn);
#endif

    }
}

void printSolution(int numVariables,
                   int numNodes,
                   double Yn[MAX_NODES+1][MAX_NODES+2],
                   vector<string> lista){
    for (int i=1; i<=numVariables; i++) {
        cout << lista[i] << ":\t" << Yn[i][numVariables+1] << endl;
    }
}

/* Function to write the Solution into an Output File */
bool WriteSolutionToFile(string filename, int numVariables, int numNodes, double Yn[MAX_NODES+1][MAX_NODES+2], vector<string> lista){
    // Opening the File for Writing
    ofstream file(filename.c_str(), ofstream::out);

    /* Writing the Header */
    for (int i = 0; i<=numVariables; i++){
        if (i == 0)
            file << "t ";
        // The Nodal Tensions
        else if (i <= numNodes)
            file << lista[i] << " ";
        // The currents
        else if (i > numNodes){
            file << lista[i] << " ";
        }
    }
    file << endl;
    /* End of Header */

    /* Start of Values Writing */
    for (int i = 0; i<=numVariables; i++){
        if (i == 0)
            file << "0 ";
        else if (i>0)
            file << Yn[i][numVariables+1] << " ";
    }
    file << std::endl;

    /* Finish of Values Writing */

    //Closing The File
    file.close();
    return true;
}
