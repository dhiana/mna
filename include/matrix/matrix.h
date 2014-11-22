#ifndef MNA_MATRIX_MATRIX_H_
#define MNA_MATRIX_MATRIX_H_
#include "consts.h"


void init(int numVariables, double Yn[MAX_NODES+1][MAX_NODES+2]);
void print(int numVariables, double Yn[MAX_NODES+1][MAX_NODES+2]);
int solve(int numVariables, double Yn[MAX_NODES+1][MAX_NODES+2]);

#endif
