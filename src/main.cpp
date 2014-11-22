#include "consts.h"
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


void exitPolitely(int exitCode) {
	#if defined (WIN32) || defined(_WIN32)
	cout << endl << "Press any key to exit...";
	cin.get();
	cin.get();
	#endif
	exit(exitCode);
}


int readNetlistFile(int argc, char** argv, ifstream& netlistFile){
    string filepath;

    switch(argc) {
        case 1: {
            cout << "Enter path to netlist file: ";
            cin >> filepath;
            break;
        }
        case 2: {
            filepath = argv[1];
            break;
        }
        default:
            cerr << "FAILURE: Too much information!" << endl;
            return EXIT_FAILURE;
    }

    netlistFile.open(filepath.c_str(), ifstream::in);
    if(!netlistFile.is_open()){
        cerr << "FAILURE: Cannot open file " << filepath << endl;
		return EXIT_FAILURE;
    }
    return 0;
}

void printIntro(){
    cout << endl << "Modified Nodal Analysis"
         << endl << "Originally by Antonio Carlos M. de Queiroz (acmq@coe.ufrj.br)"
         << endl << "Modified by Dhiana Deva, Felipe de Leo and Silvino Vieira" << endl << endl;
}


/* The program starts here */
int main(int argc, char **argv){
    printIntro();

	/* Creating a the variable "netlistFile" to use as input file to get the external data to the program */
    int numVariables=0,
        numElements=0,
        numNodes=0,
        rc=0;
    ifstream netlistFile;
    vector<string> lista(MAX_NAME+2); /*Tem que caber jx antes do nome */
    vector<Element> netlist(MAX_ELEMS);
    double Yn[MAX_NODES+1][MAX_NODES+2];

    // XXX Magic! Really important!
    // If it goes after readElements everything mixes up!
    lista[0] = "0";


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
    printVariables(numVariables, lista);

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
    printSolution(numVariables, numNodes, Yn, lista);
    #endif


    /* Save solution to File */
    string OutputFile;
    OutputFile = "output.tab";
    WriteSolutionToFile(OutputFile, numVariables, numNodes, Yn, lista);


    exitPolitely(EXIT_SUCCESS);
}
