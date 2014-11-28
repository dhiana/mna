#include "consts.h"
#include "circuits/circuit.h"
#include "circuits/element.h"
#include "matrix/matrix.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>


using namespace std;


Circuit::Circuit():
     variablesList(MAX_NODES+1),
     netlist(MAX_ELEMS)
{
}

Circuit::Circuit(int numElements,
                 int numNodes,
                 int numVariables,
                 vector<Element> &netlist):
     variablesList(MAX_NODES+1),
     netlist(netlist),
     numElements(numElements),
     numNodes(numNodes),
     numVariables(numVariables)
{
}


Circuit::Circuit(ifstream &netlistFile):
     variablesList(MAX_NODES+1),
     netlist(MAX_ELEMS)
{
    numElements=0;
    numNodes=0;
    numVariables=0;
    // XXX Magic! Really important!
    // Ground node!
    variablesList[0] = "0";

    string netlistLine;
    bool isValidElement;
    char netlistLinePrefix;
    getline(netlistFile, netlistLine);
    title = netlistLine;
    while (getline(netlistFile, netlistLine)) {
        netlistLinePrefix = netlistLine[0];
        isValidElement = Element::isValidElement(netlistLinePrefix);

        if (isValidElement){
            // Element is valid!
            numElements++; /* XXX Starts from netlist[1] */
            if (numElements>MAX_ELEMS) {
                cout << "Invalid number of elements. Maximum number of elements is " << MAX_ELEMS << endl;
                exit(EXIT_FAILURE);
            }
            netlist[numElements] = Element(netlistLine, numNodes, variablesList);
        }
        else if (netlistLinePrefix == '.'){
            stringstream ss(netlistLine);
            string name;
            ss >> name;
            if ( !name.compare(".TRAN") ){
                string analysisType; //value not used, always equal to BE
                ss >> finalTime >> step >> analysisType >> internSteps;
            }else{
                cout << "Invalid line: " << netlistLine << endl;
                exit(EXIT_FAILURE);
            }
        }
        else if (netlistLinePrefix != '*') {
            // Not a comment, not a valid element...
            // Invalid line!
            cout << "Invalid line: " << netlistLine << endl;
            exit(EXIT_FAILURE);
        }// Ignores comments!
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
        exit(EXIT_FAILURE);
    }
}


void Circuit::printVariables(){
    for (int i=0; i<=numVariables; i++){
        cout << i << " -> " << variablesList[i] << endl;
    }
    cout << endl;
}


void Circuit::printSummary(){
    cout << "# nodes:\t" << numNodes << endl;
    cout << "# variables:\t" << numVariables << endl;
    cout << "# elements:\t" << numElements << endl;
    cout << endl;
}


void Circuit::applyStamps(double Yn[MAX_NODES+1][MAX_NODES+2],
                          double (&previousSolution)[MAX_NODES+1],
                          double t,
                          double (&lastStepSolution)[MAX_NODES+1]){
    Element element;
    for (int i=1; i<=numElements; i++) {
        element = netlist[i];
        // Will pass previousSolution in the near future...
        element.applyStamp(Yn, numVariables, previousSolution, t, lastStepSolution);
        #ifdef DEBUG
        cout << "System after stamp of " << element.getName() << endl;
        print(numVariables, Yn);
        #endif

    }
}

void Circuit::printSolution(double Yn[MAX_NODES+1][MAX_NODES+2]){
    for (int i=1; i<=numVariables; i++) {
        cout << variablesList[i] << ":\t" << Yn[i][numVariables+1] << endl;
    }
}


void Circuit::writeSolutionsHeader(ofstream &file){
    for (int i = 0; i <= numVariables; i++){
        if (i == 0)
            file << "t ";
        else
            file << variablesList[i] << " ";
    }
    file << endl;
}

void Circuit::appendSolutionToFile(ofstream &file, double solution[MAX_NODES+1], double t){
    file << t << " ";
    for (int i=1; i<= numVariables; i++)
        file << solution[i] << " ";
    file << std::endl;
}

int Circuit::getNumElements(){
    return numElements;
};

int Circuit::getNumNodes(){
    return numNodes;
};

int Circuit::getNumVariables(){
    return numVariables;
};

