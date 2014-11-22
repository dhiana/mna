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

/* The program starts here */
int main(int argc, char **argv){

// Description Strings
    cout << endl;
    cout << "Modified Nodal Analysis" << endl;
    cout << "Originally by Antonio Carlos M. de Queiroz (acmq@coe.ufrj.br)" << endl;
    cout << "Modified by Dhiana Deva, Felipe de Leo and Silvino Vieira" << endl;
    cout << endl;

	/* Creating a the variable "netlistFile" to use as input file to get the external data to the program */
    ifstream netlistFile;

    if (readNetlistFile(argc, argv, netlistFile)){
        exitPolitely(EXIT_FAILURE);
    };

    int numVariables=0,
        numElements=0,
        numNodes=0;
    vector<string> lista(MAX_NAME+2); /*Tem que caber jx antes do nome */
    vector<Element> netlist(MAX_ELEMS);
    double Yn[MAX_NODES+1][MAX_NODES+2];

    // XXX Magic! Really important!
    // If it goes after readElements everything mixes up!
    lista[0] = "0";

    if (readElementsFromNetlist(numElements, numVariables, netlistFile, lista, netlist)){
        exitPolitely(EXIT_FAILURE);
    }

    if (addCurrentVariablesToNetlist(numElements,
                                     numVariables,
                                     numNodes,
                                     lista,
                                     netlist)){
        exitPolitely(EXIT_FAILURE);
    }

    /* Lista tudo */
    cout << "Variaveis internas: " << endl;
    for (int i=0; i<=numVariables; i++)
        cout << i << " -> " << lista[i] << endl;
    cout << endl;

    /* Monta o sistema nodal modificado */
    printSummary(numNodes, numVariables, numElements);

    /* Zera sistema */
    init(numVariables, Yn);

    /* Monta estampas */
    applyStamps(numElements, numVariables, netlist, Yn);

    /* Resolve o sistema */
    if (solve(numVariables, Yn)) {
        cout << "FAILURE: Could not solve!" << endl;
        exitPolitely(EXIT_FAILURE);
    }

#ifdef DEBUG
    /* Opcional: Mostra o sistema resolvido */
    cout << "Sistema resolvido:" << endl;
    print(numVariables, Yn);
#endif

    /* Mostra solucao */
    printSolution(numVariables, numNodes, Yn, lista);

    /* Save solution to File */
    string OutputFile;
    OutputFile = "output.tab";

    WriteSolutionToFile(OutputFile, numVariables, numNodes, Yn, lista);

    /* Finished the Analysis - Exit the program */
    exitPolitely(EXIT_SUCCESS);
}
