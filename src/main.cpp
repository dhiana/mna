#include "consts.h"
#include "utils.h"
#include "circuits/circuit.h"
#include "circuits/element.h"
#include "matrix/matrix.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstdlib>

using namespace std;


/* The program starts here */
int main(int argc, char **argv){
    printIntro();

    int numVariables=0,
        numElements=0,
        numNodes=0,
        rc=0;
    ifstream netlistFile;
    vector<string> variablesList(MAX_NODES+1);
    vector<Element> netlist(MAX_ELEMS);
    double Yn[MAX_NODES+1][MAX_NODES+2];

    // XXX Magic! Really important!
    // If it goes after readElements everything mixes up!
    variablesList[0] = "0";


    rc = readNetlistFile(argc, argv, netlistFile);
    if (rc) // if not return code 0 (success) 
        exitPolitely(EXIT_FAILURE);


    cout << "Reading netlist:" << endl;
    rc = readElementsFromNetlist(numElements,
                                numVariables,
                                netlistFile,
                                lista,
                                netlist);
    if (rc) // if not return code 0 (success) 
        exitPolitely(EXIT_FAILURE);


    rc = addCurrentVariablesToNetlist(numElements,
                                     numVariables,
                                     numNodes,
                                     lista,
                                     netlist);
    if (rc) // if not return code 0 (success) 
        exitPolitely(EXIT_FAILURE);


    #ifdef DEBUG
    cout << "Internal variables:" << endl;
    printVariables(numVariables, variablesList);

    cout << "Summary:" << endl;
    printSummary(numNodes, numVariables, numElements);
    #endif


    // Operations on the modified matrix...
    init(numVariables, Yn);
    applyStamps(numElements, numVariables, netlist, Yn);
    rc = solve(numVariables, Yn);
    if (rc) // if not return code 0 (success) 
        exitPolitely(EXIT_FAILURE);


    #ifdef DEBUG
    cout << "Final system:" << endl;
    print(numVariables, Yn);

    cout << "Solution:" << endl;
    printSolution(numVariables, Yn, variablesList);
    #endif


    /* Save solution to File */
    string OutputFile;
    OutputFile = "output.tab";
    WriteSolutionToFile(OutputFile, numVariables, Yn, variablesList);


    exitPolitely(EXIT_SUCCESS);
}
