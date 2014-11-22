#ifndef MNA_MATRIX_MATRIX_H_
#define MNA_MATRIX_MATRIX_H_
#include "consts.h"


void init(int nv, double Yn[MAX_NOS+1][MAX_NOS+2]);
void print(int nv, double Yn[MAX_NOS+1][MAX_NOS+2]);
int solve(int nv, double Yn[MAX_NOS+1][MAX_NOS+2]);

#endif
