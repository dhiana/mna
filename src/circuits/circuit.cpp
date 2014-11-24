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


int readElementsFromNetlist(int &numElements,
                            int &numNodes,
                            int &numVariables,
                            ifstream &netlistFile,
                            vector<string> &variablesList,
                            vector<Element> &netlist){
    string netlistLine;
    bool isValidElement;
    char netlistLinePrefix;
    getline(netlistFile, netlistLine);
    cout << "Title: " << netlistLine;
    while (getline(netlistFile, netlistLine)) {
        netlistLinePrefix = netlistLine[0];
        isValidElement = Element::isValidElement(netlistLinePrefix);

        if (isValidElement){
            // Element is valid!
            numElements++; /* XXX Starts from netlist[1] */
            if (numElements>MAX_ELEMS) {
                cout << "Invalid number of elements. Maximum number of elements is " << MAX_ELEMS << endl;
                return(EXIT_FAILURE);
            }
            netlist[numElements] = Element(netlistLine, numNodes, variablesList);
        }
        else if (netlistLinePrefix != '*') {
            // Not a comment, not a valid element...
            // Invalid line!
            cout << "Invalid line: " << netlistLine << endl;
            return(EXIT_FAILURE);
        }
        // Ignores comments!
    }
    netlistFile.close();
    cout << endl;
    // Figured out elements and all nodes

    // Now, will add extra current variables!
    numVariables = numNodes;
    for(int i=1; i<=numElements; i++){
        netlist[i].addCurrentVariables(numVariables, variablesList);
    }
    if (numVariables > MAX_NODES) {
        cout << "Extra current variables exceeded maximum number of variables: " << MAX_NODES << endl;
        return(EXIT_FAILURE);
    }
    return 0;
}

void printVariables(const int &numVariables, const vector<string> &variablesList){
    for (int i=0; i<=numVariables; i++)
        cout << i << " -> " << variablesList[i] << endl;
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


void applyStamps(int numElements,
                 int numVariables,
                 vector<Element> netlist,
                 double Yn[MAX_NODES+1][MAX_NODES+2]){
    Element element;
    for (int i=1; i<=numElements; i++) {
        element = netlist[i];
        element.applyStamp(Yn, numVariables);
        #ifdef DEBUG
        cout << "System after stamp of " << element.getName() << endl;
        print(numVariables, Yn);
        #endif

    }
}

void printSolution(int numVariables,
                   double Yn[MAX_NODES+1][MAX_NODES+2],
                   vector<string> variablesList){
    for (int i=1; i<=numVariables; i++) {
        cout << variablesList[i] << ":\t" << Yn[i][numVariables+1] << endl;
    }
}

/* Function to write the Solution into an Output File */
bool WriteSolutionToFile(string filename, int numVariables, double Yn[MAX_NODES + 1][MAX_NODES + 2], vector<string> variablesList){
    // Opening the File for Writing
    ofstream file(filename.c_str(), ofstream::out);

    /* Writing the Header */
    for (int i = 0; i <= numVariables; i++){
        if (i == 0)
            file << "t ";
        // The Nodal Tensions and Currents
        else
            file << variablesList[i] << " ";
    }

    file << endl;
    /* End of Header */

    /* Start of Values Writing */
    for (int i = 0; i <= numVariables; i++){
        if (i == 0)
            file << "0 ";
        else if (i>0)
            file << Yn[i][numVariables + 1] << " ";
    }
    file << std::endl;
    /* Finish of Values Writing */

    // Printing the message about the file saved
    cout << endl;
    cout << "The output file was saved as " << filename << endl;

        //Closing The File
        file.close();
    return true;
}
