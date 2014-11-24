#include "circuits/circuit.h"
#include "matrix/matrix.h"
#include "gtest/gtest.h"
#include <iostream>
#include <stdio.h>


using namespace std;


TEST(ElementStampsTest, Resistor) {
    // Arrange
                     // (val)(a)(b)
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


TEST(ElementStampsTest, VCCS) {
    /*
     * Voltage Controlled Current Source
     * (a.k.a. Transconductance)
     */

    // Arrange
                               // (val)(a)(b)(c)(d)
    Element transconductance("G1", 0.5, 1, 2, 3, 4);
    int numVariables = 4;

    double matrix[MAX_NODES+1][MAX_NODES+2];
    init(numVariables, matrix);

    // Act
    transconductance.applyStamp(matrix, numVariables);

    // Assert
    double expected[MAX_NODES+1][MAX_NODES+2] = {
        {0, 0 , 0 ,    0 ,    0 , 0},
        {0, 0 , 0 ,  0.5 , -0.5 , 0},
        {0, 0 , 0 , -0.5 ,  0.5 , 0},
        {0, 0 , 0 ,    0 ,    0 , 0},
        {0, 0 , 0 ,    0 ,    0 , 0}
    };
    for (int i=1; i<=numVariables; i++) {
        for (int j=1; j<=numVariables+1; j++) {
            EXPECT_EQ(expected[i][j], matrix[i][j]);
        }
    }
}


TEST(CircuitStampsTest, SimpleCircuit) {
    // Arrange
    int numNodes = 6;
    int numElements = 9;
    int numVariables = 9;
    vector<Element> netlist(10);
    double matrix[MAX_NODES+1][MAX_NODES+2];
    init(numVariables, matrix);
    // From netlist data/simples.net
    // Changes order (not netlist parameters order)! Value first!
                     // ( name, val, a, b, ... )
    netlist[1] = Element("R0102", 1, 1, 2);
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

    // Act
    applyStamps(numElements, numVariables, netlist, matrix);

    // Assert
    double expected[MAX_NODES+1][MAX_NODES+2] = {
        {  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 },
        {  0 ,  2 , -1 , -1 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 },
        {  0 , -1 ,  2 ,  0 ,  0 , -1 ,  0 ,  0 ,  0 ,  1 ,  0 },
        {  0 , -1 ,  0 ,  2 , -1 ,  0 ,  0 ,  1 ,  0 ,  0 ,  0 },
        {  0 ,  0 ,  0 , -1 ,  2 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 },
        {  0 ,  0 , -1 ,  0 ,  0 ,  2 , -1 ,  0 ,  0 ,  0 ,  0 },
        {  0 ,  0 ,  0 ,  0 ,  0 , -1 ,  1 ,  0 ,  1 ,  0 ,  0 },
        {  0 ,  1 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 },
        {  0 ,  0 ,  0 ,  0 , -1 ,  1 ,  0 ,  0 ,  0 ,  0 ,  0 },
        {  0 ,  0 , -1 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , -1 },
    };
    for (int i=1; i<=numVariables; i++) {
        for (int j=1; j<=numVariables+1; j++) {
            EXPECT_EQ(expected[i][j], matrix[i][j]);
        }
    }
}
