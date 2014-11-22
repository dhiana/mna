#ifndef MNA_CIRCUIT_CIRCUIT_H_
#define MNA_CIRCUIT_CIRCUIT_H_
#include <vector>
#include <string>
#include "circuits/element.h"
#include "matrix/matrix.h"
#include "consts.h"


using namespace std;

void applyStamps(int numElements, int numVariables, vector<Element> netlist, double Yn[MAX_NOS+1][MAX_NOS+2]);
void printSolution(int numVariables, int numNodes, double Yn[MAX_NOS+1][MAX_NOS+2], vector<string> lista);

#endif
