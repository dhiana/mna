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


int main(int argc, char **argv){

    cout << endl;
    cout << "Modified Nodal Analysis" << endl;
    cout << "Originally by Antonio Carlos M. de Queiroz (acmq@coe.ufrj.br)" << endl;
    cout << "Modified by Dhiana Deva, Felipe de Leo and Silvino Vieira" << endl;
    cout << endl;

    ifstream netlistFile;
    if (readNetlistFile(argc, argv, netlistFile)){
        exitPolitely(EXIT_FAILURE);
    };

    string txt;
    vector<string> lista(MAX_NAME+2); /*Tem que caber jx antes do nome */
    lista[0] = "0";
    vector<Element> netlist(MAX_ELEMS);

    int numVariables=0,
        numElements=0,
        numNodes=0;

    char tipo;
    double Yn[MAX_NODES+1][MAX_NODES+2];

    cout << "Lendo netlist:" << endl;
    getline(netlistFile, txt);
    cout << "Titulo: " << txt;
    while (getline(netlistFile, txt)) {
        numElements++; /* Nao usa o netlist[0] */
        if (numElements>MAX_ELEMS) {
            cout << "O programa so aceita ate " << MAX_ELEMS << " elementos" << endl;
			exitPolitely(EXIT_FAILURE);
        }
        netlist[numElements] = Element(txt, numElements, numVariables, lista);
    }
    netlistFile.close();

    /* Acrescenta variaveis de corrente acima dos nos, anotando no netlist */
    numNodes=numVariables;
    for (int i=1; i<=numElements; i++) {
        tipo=netlist[i].getType();
        if (tipo=='V' || tipo=='E' || tipo=='F' || tipo=='O') {
            numVariables++;
            if (numVariables>MAX_NODES) {
                cout << "As correntes extra excederam o numero de variaveis permitido (" << MAX_NODES << ")" << endl;
                exitPolitely(EXIT_FAILURE);
            }
            lista[numVariables] = "j"; /* Tem espaco para mais dois caracteres */
            lista[numVariables].append( netlist[i].getName() );
            netlist[i].x=numVariables;
        }
        else if (tipo=='H') {
            numVariables=numVariables+2;
            if (numVariables>MAX_NODES) {
                cout << "As correntes extra excederam o numero de variaveis permitido (" << MAX_NODES << ")" << endl;
                exitPolitely(EXIT_FAILURE);
            }
            lista[numVariables-1] = "jx";
            lista[numVariables-1].append(netlist[i].getName());
            netlist[i].x=numVariables-1;
            lista[numVariables] = "jy";
            lista[numVariables].append( netlist[i].getName() );
            netlist[i].y=numVariables;
        }
    }
    cout << endl;

    /* Lista tudo */
    cout << "Variaveis internas: " << endl;
    for (int i=0; i<=numVariables; i++)
        cout << i << " -> " << lista[i] << endl;
    cout << endl;

    cout << "Netlist interno final" << endl;
    for (int i=1; i<=numElements; i++) {
        tipo=netlist[i].getType();
        if (tipo=='R' || tipo=='I' || tipo=='V') {
            cout << netlist[i].getName() << " " << netlist[i].a << " " << netlist[i].b << " " << netlist[i].valor << endl;
        }
        else if (tipo=='G' || tipo=='E' || tipo=='F' || tipo=='H') {
            cout << netlist[i].getName() << " " << netlist[i].a << " " << netlist[i].b << " "
                 << netlist[i].c << " " << netlist[i].d   << " " << netlist[i].valor << endl;
        }
        else if (tipo=='O') {
            cout << netlist[i].getName() << " " << netlist[i].a << " " << netlist[i].b << " "
                 << netlist[i].c << " " << netlist[i].d   << endl;
        }
        if (tipo=='V' || tipo=='E' || tipo=='F' || tipo=='O')
            cout << "Corrente jx: " << netlist[i].x << endl;
        else if (tipo=='H')
            cout << "Correntes jx e jy: " << netlist[i].x << ", " << netlist[i].y << endl;
    }
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

	exitPolitely(EXIT_SUCCESS);
}
