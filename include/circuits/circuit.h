#ifndef MNA_CIRCUIT_CIRCUIT_H_
#define MNA_CIRCUIT_CIRCUIT_H_
#include <vector>
#include <string>
#include "circuits/element.h"
#include "matrix/matrix.h"
#include "consts.h"


using namespace std;

int readElementsFromNetlist(int &numElements,
                            int &numNodes,
                            int &numVariables,
                            ifstream &netlistFile,
                            vector<string> &variablesList,
                            vector<Element> &netlist);

void applyStamps(int numElements, int numVariables, vector<Element> netlist, double Yn[MAX_NODES+1][MAX_NODES+2]);
void printSolution(int numVariables, double Yn[MAX_NODES+1][MAX_NODES+2], vector<string> variablesList);
void printSummary(int numNodes, int numVariables, int numElements);
void printVariables(const int &numVariables, const vector<string> &variablesList);
bool WriteSolutionToFile(string filename, int numVariables, double Yn[MAX_NODES+1][MAX_NODES+2], vector<string> variablesList);

#endif
