#include "consts.h"
#include "utils.h"
#include "circuits/circuit.h"
#include "circuits/element.h"
#include "matrix/matrix.h"
#include "matrix/newtonraphson.h"
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

    int rc=0;
    ifstream netlistFile;
    double Yn[MAX_NODES+1][MAX_NODES+2];
    Circuit circuit;


    // Reads netlist file
    string netlistFileName;
    readNetlistFile(argc, argv, netlistFileName, netlistFile);
    // Prepares solutions file
    string outputFileName;
    outputFileName = netlistFileName.substr(0, netlistFileName.find(".")).append(".tab");
    ofstream solutionsFile(outputFileName.c_str(), ofstream::out);


    // Parses netlist file (constructs Circuit)
    cout << "Reading netlist:" << endl;
    circuit = Circuit(netlistFile);

    // Write solutions file header
    circuit.writeSolutionsHeader(solutionsFile);

    #ifdef DEBUG
    cout << "Internal variables:" << endl;
    circuit.printVariables();

    cout << "Summary:" << endl;
    circuit.printSummary();
    #endif


    // Bias Analysis
    double initialSolution[MAX_NODES+1];

    runNewtonRaphson(circuit, initialSolution);

    circuit.appendSolutionToFile(solutionsFile, initialSolution);


    //Closing The File
    cout << endl << "Created: " << outputFileName << endl;
    solutionsFile.close();


    exitPolitely(EXIT_SUCCESS);
}
