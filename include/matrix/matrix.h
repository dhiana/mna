#ifndef MNA_MATRIX_MATRIX_H_
#define MNA_MATRIX_MATRIX_H_
#include "consts.h"


void init(int numVariables, double Yn[MAX_NOS+1][MAX_NOS+2]);
void print(int numVariables, double Yn[MAX_NOS+1][MAX_NOS+2]);
int solve(int numVariables, double Yn[MAX_NOS+1][MAX_NOS+2]);

#endif
