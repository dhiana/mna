#include "circuits/circuit.h"
#include "matrix/matrix.h"
#include "gtest/gtest.h"
#include <iostream>
#include <stdio.h>


using namespace std;


TEST(ElementStampsTest, Resistor) {
    // Arrange
    Element resistor("R1", 4, 1, 2);
    int numVariables = 2;

    double matrix[MAX_NODES+1][MAX_NODES+2];
    init(numVariables, matrix);

    // Act
    resistor.applyStamp(matrix, numVariables);

    // Assert
    double expected[MAX_NODES+1][MAX_NODES+2] = {
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
