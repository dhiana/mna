#ifndef MNA_CIRCUIT_CIRCUIT_H_
#define MNA_CIRCUIT_CIRCUIT_H_
#include <vector>
#include <string>
#include "circuits/element.h"
#include "matrix/matrix.h"


using namespace std;

void applyStamps(int numberElements, int nv, vector<Element> netlist, double Yn[MAX_NOS+1][MAX_NOS+2]);
void printSolution(int nv, int nn, double Yn[MAX_NOS+1][MAX_NOS+2], vector<string> lista);

#endif
