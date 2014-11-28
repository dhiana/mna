#ifndef MNA_MATRIX_MATRIX_H_
#define MNA_MATRIX_MATRIX_H_
#include "consts.h"


void init(int numVariables, double Yn[MAX_NODES+1][MAX_NODES+2]);
void print(int numVariables, double Yn[MAX_NODES+1][MAX_NODES+2]);
int solve(int numVariables, double Yn[MAX_NODES+1][MAX_NODES+2]);
void getSolution(int numVariables,
                 double Yn[MAX_NODES+1][MAX_NODES+2],
                 double solution[MAX_NODES+1]);
void copySolution(int numVariables,
                  double originalSolution[MAX_NODES+1],
                  double targetSolution[MAX_NODES+1]);

#endif
