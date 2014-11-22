#include "circuits/circuit.h"
#include "matrix/matrix.h"
#include "gtest/gtest.h"
#include <iostream>
#include <stdio.h>


using namespace std;


TEST(CircuitStampsTest, SimpleResistor) {
    // Arrange
    int numElements = 1;
    int numVariables = 2;
    double matrix[MAX_NOS+1][MAX_NOS+2];
    init(numVariables, matrix);
    Element resistor("R1", 4, 1, 2);
    vector<Element> netlist(2);
    netlist[1] = resistor;
    // Act
    applyStamps(numElements, numVariables, netlist, matrix);
    // Assert
    double expected[MAX_NOS+1][MAX_NOS+2] = {
        {0,    0,    0, 0},
        {0,  0.25, -0.25, 0},
        {0, -0.25,  0.25, 0}
    };
    for (int i=1; i<=numVariables; i++) {
        for (int j=1; j<=numVariables+1; j++) {
            EXPECT_EQ(expected[i][j], matrix[i][j]);
        }
    }
}
