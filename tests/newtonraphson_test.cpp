#include "circuits/circuit.h"
#include "matrix/newtonraphson.h"
#include "matrix/matrix.h"
#include "gtest/gtest.h"
#include <iostream>
#include <stdio.h>
#include <math.h>


using namespace std;


TEST(NewtonRaphsonTest, SimpleCircuit) {
    // Arrange
    int numNodes = 6;
    int numElements = 9;
    int numVariables = 9;
   
    vector<double> params(MAX_PARAMS);
    // 0 -0.7 0 0.4 0 -0.2 0 0.027
    params[0] = 0;
    params[1] = -0.7;
    params[2] = 0;
    params[3] = 0.4;
    params[4] = 0;
    params[5] = -0.2;
    params[6] = 0;
    params[7] = 0.027;

    vector<Element> netlist(MAX_ELEMS);
                     // ( name,   params, a, b, ... )
    netlist[1] = Element("RPol8", params, 1, 2);
    netlist[2] = Element("R0301", 1, 3, 1);
    netlist[3] = Element("R0403", 1, 4, 3);
    netlist[4] = Element("R0004", 1, 0, 4);
    netlist[5] = Element("R0502", 1, 5, 2);
    netlist[6] = Element("R0605", 1, 6, 5);
    netlist[7] = Element("O0300", 0, 3, 0, 1, 0); // OpAmps have no value!!!
    netlist[8] = Element("O0600", 0, 6, 0, 5, 4);
    netlist[9] = Element("V0200", 1, 2, 0);
    // Bad smell about the need of this member function...
    // Without it, only first part of matrix would be populated!
    vector<string> dummyVariablesList(10);
    int pivotNumVariables = numNodes;
    netlist[7].addCurrentVariables(pivotNumVariables, dummyVariablesList);
    netlist[8].addCurrentVariables(pivotNumVariables, dummyVariablesList);
    netlist[9].addCurrentVariables(pivotNumVariables, dummyVariablesList);

    Circuit circuit(
        numElements,
        numNodes,
        numVariables,
        netlist
    );

    // Act
    double solution[MAX_NODES+1];
    runNewtonRaphson(circuit, solution);

    // Assert
    // Result from mnae program
    double expected[MAX_NODES+1] = {
        0, 0, 1, 0.473, 0.2365, 0.2365, -0.527, -0.7095, 0.7635, -0.2905
    };
    for (int i=1; i<=circuit.getNumVariables(); i++) {
        EXPECT_NEAR(expected[i], solution[i], TOLG);
    }
}
