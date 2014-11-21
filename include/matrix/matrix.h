#ifndef MNA_MATRIX_MATRIX_H_
#define MNA_MATRIX_MATRIX_H_


static const int MAX_NOS = 50;
static const double TOLG = 1e-9;

void print(int nv, double Yn[MAX_NOS+1][MAX_NOS+2]);
int solve(int nv, double Yn[MAX_NOS+1][MAX_NOS+2]);

#endif
