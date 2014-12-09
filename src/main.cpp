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
#include <math.h>


using namespace std;


/* The program starts here */
int main(int argc, char **argv){
    printIntro();

    int rc=0;
    ifstream netlistFile;

    // Reads netlist file
    string netlistFileName;
    rc = openNetlistFile(argc, argv, netlistFileName, netlistFile);
    if (rc == EXIT_FAILURE)
        exitPolitely(EXIT_FAILURE);
    // Prepares solutions file
    string outputFileName;
    outputFileName = netlistFileName.substr(0, netlistFileName.find(".")).append("_mnapoly.tab");
    ofstream solutionsFile(outputFileName.c_str(), ofstream::out);


    // Parses netlist file (constructs Circuit)
    cout << "Reading netlist:" << endl;
    Circuit circuit(netlistFile);
    netlistFile.close();

    // Write solutions file header
    circuit.writeSolutionsHeader(solutionsFile);

    #ifdef DEBUG
    cout << "Internal variables:" << endl;
    circuit.printVariables();

    cout << "Summary:" << endl;
    circuit.printSummary();
    #endif


    // Bias Analysis
    double t=0;
    double solution[MAX_NODES+1];
    copySolution(circuit.getNumVariables(), ZERO_SOLUTION, solution);
    runNewtonRaphson(circuit, solution, t);
    circuit.appendSolutionToFile(solutionsFile, solution);

    // Transient Analysis
    double internalStep = circuit.getInternalStep();
    int numInternalSteps = circuit.getNumInternalSteps();
    double finalTime = circuit.getFinalTime();
    double lastSolution[MAX_NODES+1];
    int numIterations = 0;
    do {
        numIterations++;
        t = t + internalStep;
        copySolution(circuit.getNumVariables(),
                     solution,
                     lastSolution);
        runNewtonRaphson(circuit, solution, t, lastSolution);
        if (!(numIterations % numInternalSteps))
            circuit.appendSolutionToFile(solutionsFile, solution, t);
    } while (t<finalTime);

    //Closing The File
    cout << endl << "Created: " << outputFileName << endl;
    solutionsFile.close();


    exitPolitely(EXIT_SUCCESS);
}
