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

    int rc=0;
    ifstream netlistFile;
    double Yn[MAX_NODES+1][MAX_NODES+2];
    Circuit circuit;


    readNetlistFile(argc, argv, netlistFile);

    cout << "Reading netlist:" << endl;
    circuit = Circuit(netlistFile);


    #ifdef DEBUG
    cout << "Internal variables:" << endl;
    circuit.printVariables();

    cout << "Summary:" << endl;
    circuit.printSummary();
    #endif


    // Operations on the modified matrix...
    init(circuit.getNumVariables(), Yn);
    circuit.applyStamps(Yn);
    rc = solve(circuit.getNumVariables(), Yn);
    if (rc) // if not return code 0 (success)
        exitPolitely(EXIT_FAILURE);


    #ifdef DEBUG
    cout << "Final system:" << endl;
    print(circuit.getNumVariables(), Yn);

    cout << "Solution:" << endl;
    circuit.printSolution(Yn);
    #endif


    /* Save solution to File */
    string OutputFile;
    OutputFile = "output.tab";
    circuit.WriteSolutionToFile(OutputFile, Yn);


    exitPolitely(EXIT_SUCCESS);
}
