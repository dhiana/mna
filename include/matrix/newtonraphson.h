#ifndef MATRIX_NEWTONRAPHSON_H
#define MATRIX_NEWTONRAPHSON_H

#include "consts.h"
#include "circuits/circuit.h"


int runNewtonRaphson(Circuit circuit,
                     double finalSolution[MAX_NODES+1]);


#endif
