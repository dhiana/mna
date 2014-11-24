#include "matrix/matrix.h"
#include "gtest/gtest.h"
#include <stdio.h>


TEST(MatrixSolveTest, Dummy) {
    // Arrange
    int numVariables = 3;
    // XXX this function expects a column and a row filled with zeros!
    double matrix[MAX_NODES+1][MAX_NODES+2] = {
        {0, 0, 0, 0, 0},
        {0, 1, 0, 0, 1},
        {0, 0, 1, 0, 2},
        {0, 0, 0, 1, 3}
    };
    // Act
    solve(numVariables, matrix);
    // Assert
	EXPECT_EQ(1, matrix[1][numVariables+1]);
	EXPECT_EQ(2, matrix[2][numVariables+1]);
	EXPECT_EQ(3, matrix[3][numVariables+1]);
}

TEST(MatrixSolveTest, AlmostDummy) {
    // Arrange
    int numVariables = 3;
    // XXX this function expects a column and a row filled with zeros!
    double matrix[MAX_NODES+1][MAX_NODES+2] = {
        {0, 0, 0, 0, 0},
        {0, 1, 0, 1, 1},
        {0, 0, 1, 1, 2},
        {0, 1, 1, 0, 0}
    };
    // Act
    solve(numVariables, matrix);
    // Assert
	EXPECT_EQ(-0.5, matrix[1][numVariables+1]);
	EXPECT_EQ(0.5, matrix[2][numVariables+1]);
	EXPECT_EQ(1.5, matrix[3][numVariables+1]);
}

TEST(MatrixSolveTest, SimpleCircuit) {
    // Arrange
    int numVariables = 9;
    // Augmented matrix for data/simples.net
    // Needs row zero and column zero... ¯\_(ツ)_/¯
    // XXX this function expects a column and a row filled with zeros!
    // XXX result is totally different without it!
    double matrix[MAX_NODES+1][MAX_NODES+2] = {
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
    // Act
    solve(numVariables, matrix);
    // Assert
    double solution[MAX_NODES+1] = {0, 0, 1, -1, -0.5, -0.5, -2.0, 1.5, 1.5, -2.5};
    double result[MAX_NODES];
    for (int i=1; i<=numVariables; i++) {
        result[i] = matrix[i][numVariables+1];
        // Using EXPECT_NEAR because of floating point issues. See:
        // https://code.google.com/p/googletest/wiki/V1_7_AdvancedGuide#Floating-Point_Comparison
	    EXPECT_NEAR(solution[i], result[i], TOLG);
        // Verifying if the rest of the matrix is well-behaved
	    EXPECT_EQ(1.0, matrix[i][i]);
        for (int j=1; j<=numVariables; j++) {
            if(j==i)
                EXPECT_EQ(1.0, matrix[i][j]);
            else
                EXPECT_EQ(0.0, matrix[i][j]);
        }
    }
}
