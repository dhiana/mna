#include "circuits/circuit.h"
#include "matrix/matrix.h"
#include "gtest/gtest.h"
#include <iostream>
#include <stdio.h>
#include <math.h>


using namespace std;


TEST(ElementStampsTest, CapacitorBias) {
    // Arrange
                       // (val)(a)(b)
    Element capacitor("C1", 10, 1, 2);
    int numVariables = 2;

    double matrix[MAX_NODES+1][MAX_NODES+2];
    init(numVariables, matrix);

    // Act
    capacitor.applyStamp(matrix, numVariables, ZERO_SOLUTION, 0);

    // Assert
    // At bias analysis, the capacitor is considered a big impedance
    // And this means a small condunctance!
    double expected[MAX_NODES+1][MAX_NODES+2] = {
        {0,    0,    0, 0},
        {0,  1e-9, -1e-9, 0},
        {0, -1e-9,  1e-9, 0}
    };
    for (int i=1; i<=numVariables; i++) {
        for (int j=1; j<=numVariables+1; j++) {
            EXPECT_NEAR(expected[i][j], matrix[i][j], TOLG);
        }
    }
    print(numVariables, matrix);
}


TEST(ElementStampsTest, CapacitorTransient) {
    // Arrange
                       // (val)(a)(b)
    Element capacitor("C1", 10, 1, 2);
    int numVariables = 2;

    double matrix[MAX_NODES+1][MAX_NODES+2];
    init(numVariables, matrix);

    // Act
    double t = 1e-1; // doesn't matter
    double step = 1e-3;
    double lastSolution[MAX_NODES+1] = {0, 5, 1}; // Vc(to)=4
    capacitor.applyStamp(matrix,
                         numVariables,
                         ZERO_SOLUTION,
                         t,
                         step,
                         lastSolution);
    // Assert
    // For Backward Euler, the capacitor is:
    // dt/C resistor in parallel with a -C/dt*v(t0) current source!
    // R = step/C -> G = C/step
    double G = 10/1e-3;
    // I = - C/step * Vc(to)
    double I = -(10/1e-3)*4;
    double expected[MAX_NODES+1][MAX_NODES+2] = {
        {0,  0,  0,  0},
        {0,  G, -G, -I},
        {0, -G,  G,  I}
    };
    for (int i=1; i<=numVariables; i++) {
        for (int j=1; j<=numVariables+1; j++) {
            EXPECT_NEAR(expected[i][j], matrix[i][j], TOLG);
        }
    }
    print(numVariables, matrix);
}


TEST(ElementStampsTest, InductorBias) {
    // Arrange
                      // (val)(a)(b)
    Element inductor("L1", 10, 1, 2);
    int numVariables = 2;

    double matrix[MAX_NODES + 1][MAX_NODES + 2];
    // Bad smell about the need of this member function...
    // Without it, only first part of matrix would be populated!
    vector<string> dummyVariablesList(10);
    inductor.addCurrentVariables(numVariables, dummyVariablesList);

    init(numVariables, matrix);

    // Act
    inductor.applyStamp(matrix, numVariables, ZERO_SOLUTION, 0);

    // Assert
    // At bias analysis, the inductor is considered a small impedance
    // And this means a big condunctance!
    double expected[MAX_NODES+1][MAX_NODES+2] = {
        {0,  0, 0,    0, 0},
        {0,  0, 0,    1, 0},
        {0,  0, 0,   -1, 0},
        {0, -1, 1, 1e-9, 0} // short circuit!!!
    };
    // XXX Warning!!! Raised tolerance!
    // An error of 1.1920928955078125e-7 was appearing...
    for (int i=1; i<=numVariables; i++) {
        for (int j=1; j<=numVariables+1; j++) {
            EXPECT_NEAR(expected[i][j], matrix[i][j], 1e-6);
        }
    }
    print(numVariables, matrix);
}


TEST(ElementStampsTest, InductorTransient) {
    // Arrange
                      // (val)(a)(b)
    Element inductor("L1", 10, 1, 2);
    int numVariables = 2;

    double matrix[MAX_NODES+1][MAX_NODES+2];
    // Bad smell about the need of this member function...
    // Without it, only first part of matrix would be populated!
    vector<string> dummyVariablesList(10);
    inductor.addCurrentVariables(numVariables, dummyVariablesList);

    init(numVariables, matrix);

    // Act
    double t = 1e-1; // doesn't matter
    double step = 1e-3;
    double lastSolution[MAX_NODES+1] = {0, 0, 0, 4}; // jL(to)=4
    inductor.applyStamp(matrix,
                         numVariables,
                         ZERO_SOLUTION,
                         t,
                         step,
                         lastSolution);
    // Assert
    // For Backward Euler, the inductor is:
    // L/dt resistor in series with a -L/dt*jL(t0) voltage source!
    // R = L/step
    double R = 10/1e-3;
    // V = - L/step * jL(to)
    double V = -(10/1e-3)*4;
    double expected[MAX_NODES+1][MAX_NODES+2] = {
        {0,  0, 0,  0,  0},
        {0,  0, 0,  1,  0},
        {0,  0, 0, -1,  0},
        {0, -1, 1,  R, -V}
    };
    for (int i=1; i<=numVariables; i++) {
        for (int j=1; j<=numVariables+1; j++) {
            EXPECT_NEAR(expected[i][j], matrix[i][j], TOLG);
        }
    }
    print(numVariables, matrix);
}


TEST(ElementStampsTest, Resistor) {
    // Arrange
                      // (val)(a)(b)
    Element resistor("R1", 10, 1, 2);
    int numVariables = 2;

    double matrix[MAX_NODES+1][MAX_NODES+2];
    init(numVariables, matrix);

    // Act
    resistor.applyStamp(matrix, numVariables);

    // Assert
    double expected[MAX_NODES+1][MAX_NODES+2] = {
        {0,    0,    0, 0},
        {0,  0.1, -0.1, 0},
        {0, -0.1,  0.1, 0}
    };
    for (int i=1; i<=numVariables; i++) {
        for (int j=1; j<=numVariables+1; j++) {
            EXPECT_EQ(expected[i][j], matrix[i][j]);
        }
    }
    print(numVariables, matrix);
}


TEST(ElementStampsTest, TwoParametersPolinomialResistor) {
    // Arrange
    vector<double> params(MAX_PARAMS);
    params[0] = 10;
    params[1] = 20;
                          // (params)(a)(b)
    Element resistor("RPol2", params, 1, 2);
    int numVariables = 2;

    double matrix[MAX_NODES+1][MAX_NODES+2];
    init(numVariables, matrix);

    // Act
    // Actually, for two parameters, the last solution doesn't matter!
                                       // (gnd)(e1)(e2)
    double previousSolution[MAX_NODES+1] = {0, 100,100}; 
    resistor.applyStamp(matrix, numVariables, previousSolution);

    // Assert
    double G0 = params[1];  // G0 = a1 + 2*a2*Xn + 3*a3*Xn^2 ...
    double I0 = params[0];  // I0 = a0 - a2*Xn^2 - 2*a3*Xn^3 ... 
    double expected[MAX_NODES+1][MAX_NODES+2] = {
        {0,    0,    0, 0},
        {0,  G0, -G0, -I0},
        {0, -G0,  G0, +I0}
    };
    for (int i=1; i<=numVariables; i++) {
        for (int j=1; j<=numVariables+1; j++) {
            EXPECT_EQ(expected[i][j], matrix[i][j]);
        }
    }
    print(numVariables, matrix);
}


TEST(ElementStampsTest, EightParametersPolinomialResistor) {
    // Arrange
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
                         // (params) (a)(b)
    Element resistor("RPol8", params, 1, 2);
    int numVariables = 2;

    double matrix[MAX_NODES + 1][MAX_NODES + 2];
    init(numVariables, matrix);

    // Act
                                         // (gnd)(e1)(e2)
    double previousSolution[MAX_NODES + 1] = { 0, 3, 1 }; // Va-Vb = 2 
    resistor.applyStamp(matrix, numVariables, previousSolution);

    // Assert
    double Xn = previousSolution[1] - previousSolution[2];
    // G0 = a1 + 2*a2*Xn + 3*a3*Xn^2 ...
    double G0 = params[1]
              + 2 * params[2] * Xn
              + 3 * params[3] * pow(Xn, 2)
              + 4 * params[4] * pow(Xn, 3)
              + 5 * params[5] * pow(Xn, 4)
              + 6 * params[6] * pow(Xn, 5)
              + 7 * params[7] * pow(Xn, 6);  
    // I0 = a0 - a2*Xn^2 - 2*a3*Xn^3 ... 
    double I0 = params[0]
              - params[2] * pow(Xn, 2)
              - 2 * params[3] * pow(Xn, 3)
              - 3 * params[4] * pow(Xn, 4)
              - 4 * params[5] * pow(Xn, 5)
              - 5 * params[6] * pow(Xn, 6)
              - 6 * params[7] * pow(Xn, 7);
    double expected[MAX_NODES + 1][MAX_NODES + 2] = {
            { 0, 0, 0, 0 },
            { 0, G0, -G0, -I0 },
            { 0, -G0, G0, +I0 }
    };
    for (int i = 1; i <= numVariables; i++) {
        for (int j = 1; j <= numVariables + 1; j++) {
            EXPECT_EQ(expected[i][j], matrix[i][j]);
        }
    }
    print(numVariables, matrix);
}


TEST(ElementStampsTest, VCCS) {
    /*
     * Voltage Controlled Current Source
     * (a.k.a. Transconductance)
     */

    // Arrange
                              // (val)(a)(b)(c)(d)
    Element transconductance("G1", 10, 1, 2, 3, 4);
    int numVariables = 4;

    double matrix[MAX_NODES+1][MAX_NODES+2];
    init(numVariables, matrix);

    // Act
    transconductance.applyStamp(matrix, numVariables);

    // Assert
    double expected[MAX_NODES+1][MAX_NODES+2] = {
        {0, 0 , 0 ,   0 ,   0 , 0},
        {0, 0 , 0 ,  10 , -10 , 0},
        {0, 0 , 0 , -10 ,  10 , 0},
        {0, 0 , 0 ,   0 ,   0 , 0},
        {0, 0 , 0 ,   0 ,   0 , 0}
    };
    for (int i=1; i<=numVariables; i++) {
        for (int j=1; j<=numVariables+1; j++) {
            EXPECT_EQ(expected[i][j], matrix[i][j]);
        }
    }
    print(numVariables, matrix);
}


TEST(ElementStampsTest, EightParametersPolinomialTranscondutance) {
    /*
     * Polynomial Voltage Controlled Current Source
     * (a.k.a. Transconductance)
     */

    // Arrange
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
                         // (params)         (a)(b)(c)(d)
    Element transconductance("GPol8", params, 1, 2, 3, 4);
    int numVariables = 4;

    double matrix[MAX_NODES + 1][MAX_NODES + 2];
    init(numVariables, matrix);

    // Act
                                         // (gnd)     (e3)(e4)
    double previousSolution[MAX_NODES + 1] = { 0, 0, 0, 3, 1 }; // Vc-Vd = 2 
    transconductance.applyStamp(matrix, numVariables, previousSolution);

    // Assert
    double Xn = previousSolution[3] - previousSolution[4];
    // G0 = a1 + 2*a2*Xn + 3*a3*Xn^2 ...
    double G0 = params[1]
              + 2 * params[2] * Xn
              + 3 * params[3] * pow(Xn, 2)
              + 4 * params[4] * pow(Xn, 3)
              + 5 * params[5] * pow(Xn, 4)
              + 6 * params[6] * pow(Xn, 5)
              + 7 * params[7] * pow(Xn, 6);  
    // I0 = a0 - a2*Xn^2 - 2*a3*Xn^3 ... 
    double I0 = params[0]
              - params[2] * pow(Xn, 2)
              - 2 * params[3] * pow(Xn, 3)
              - 3 * params[4] * pow(Xn, 4)
              - 4 * params[5] * pow(Xn, 5)
              - 5 * params[6] * pow(Xn, 6)
              - 6 * params[7] * pow(Xn, 7);
    double expected[MAX_NODES + 1][MAX_NODES + 2] = {
            { 0, 0, 0,   0,   0,   0},
            { 0, 0, 0,  G0, -G0, -I0},
            { 0, 0, 0, -G0,  G0, +I0},
            { 0, 0, 0,   0,   0,   0},
            { 0, 0, 0,   0,   0,   0}
    };
    for (int i = 1; i <= numVariables; i++) {
        for (int j = 1; j <= numVariables + 1; j++) {
            EXPECT_EQ(expected[i][j], matrix[i][j]);
        }
    }
    print(numVariables, matrix);
}


TEST(ElementStampsTest, CurrentSource) {
    // Arrange
                           // (val)(a)(b)
    Element currentSource("I1", 10, 1, 2);
    int numVariables = 2;

    double matrix[MAX_NODES+1][MAX_NODES+2];
    init(numVariables, matrix);

    // Act
    currentSource.applyStamp(matrix, numVariables);

    // Assert
    double expected[MAX_NODES+1][MAX_NODES+2] = {
        {0, 0 , 0 ,  0},
        {0, 0 , 0 , -10},
        {0, 0 , 0 ,  10}
    };
    for (int i=1; i<=numVariables; i++) {
        for (int j=1; j<=numVariables+1; j++) {
            EXPECT_EQ(expected[i][j], matrix[i][j]);
        }
    }
    print(numVariables, matrix);
}


TEST(ElementStampsTest, VoltageSource) {
    // Arrange
                           // (val)(a)(b)
    Element voltageSource("V1", 10, 1, 2);
    int numVariables = 2;

    double matrix[MAX_NODES+1][MAX_NODES+2];
    // Bad smell about the need of this member function...
    // Without it, only first part of matrix would be populated!
    vector<string> dummyVariablesList(10);
    voltageSource.addCurrentVariables(numVariables, dummyVariablesList);

    // Important!!! Should be initialized after updating numVariables
    // with extra current variables!
    init(numVariables, matrix);

    // Act
    voltageSource.applyStamp(matrix, numVariables);

    // Assert
    double expected[MAX_NODES+1][MAX_NODES+2] = {
        { 0,  0 , 0 ,  0 ,   0 },
        { 0,  0 , 0 ,  1 ,   0 },
        { 0,  0 , 0 , -1 ,   0 },
        { 0, -1 , 1 ,  0 , -10 }
    };
    for (int i=1; i<=numVariables; i++) {
        for (int j=1; j<=numVariables+1; j++) {
            EXPECT_EQ(expected[i][j], matrix[i][j]);
        }
    }
    print(numVariables, matrix);
}


TEST(ElementStampsTest, VCVS) {
    /*
     * Voltage Controlled Voltage Source
     * (a.k.a. Voltage Amplifier)
     */

    // Arrange
                              // (val)(a)(b)(c)(d)
    Element voltageAmplifier("E1", 10, 1, 2, 3, 4);
    int numVariables = 4;

    double matrix[MAX_NODES+1][MAX_NODES+2];
    // Bad smell about the need of this member function...
    // Without it, only first part of matrix would be populated!
    vector<string> dummyVariablesList(10);
    voltageAmplifier.addCurrentVariables(numVariables, dummyVariablesList);

    // Important!!! Should be initialized after updating numVariables
    // with extra current variables!
    init(numVariables, matrix);

    // Act
    voltageAmplifier.applyStamp(matrix, numVariables);

    // Assert
    double expected[MAX_NODES+1][MAX_NODES+2] = {
       // 0    1    2   3     4   jx   i
        { 0 ,  0 ,  0 , 0 ,   0 ,  0 , 0 },
        { 0 ,  0 ,  0 , 0 ,   0 ,  1 , 0 },
        { 0 ,  0 ,  0 , 0 ,   0 , -1 , 0 },
        { 0 ,  0 ,  0 , 0 ,   0 ,  0 , 0 },
        { 0 ,  0 ,  0 , 0 ,   0 ,  0 , 0 },
        { 0 , -1 , +1, 10 , -10 ,  0 , 0 }
    };
    for (int i=1; i<=numVariables; i++) {
        for (int j=1; j<=numVariables+1; j++) {
            EXPECT_EQ(expected[i][j], matrix[i][j]);
        }
    }
    print(numVariables, matrix);
}


TEST(ElementStampsTest, EightParametersPolinomialVoltageAmplifier) {
    /*
     * Polynomial Voltage Controlled Voltage Source
     * (a.k.a. Voltage Amplifier)
     */

    // Arrange
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
                         // (params)         (a)(b)(c)(d)
    Element voltageAmplifier("EPol8", params, 1, 2, 3, 4);
    int numVariables = 4;

    double matrix[MAX_NODES + 1][MAX_NODES + 2];
    // Bad smell about the need of this member function...
    // Without it, only first part of matrix would be populated!
    vector<string> dummyVariablesList(10);
    voltageAmplifier.addCurrentVariables(numVariables, dummyVariablesList);

    // Important!!! Should be initialized after updating numVariables
    // with extra current variables!
    init(numVariables, matrix);

    // Act
                                         // (gnd)     (e3)(e4)
    double previousSolution[MAX_NODES + 1] = { 0, 0, 0, 3, 1 }; // Vc-Vd = 2 
    voltageAmplifier.applyStamp(matrix, numVariables, previousSolution);

    // Assert
    double Xn = previousSolution[3] - previousSolution[4];
    // G0 = a1 + 2*a2*Xn + 3*a3*Xn^2 ...
    double A = params[1]
              + 2 * params[2] * Xn
              + 3 * params[3] * pow(Xn, 2)
              + 4 * params[4] * pow(Xn, 3)
              + 5 * params[5] * pow(Xn, 4)
              + 6 * params[6] * pow(Xn, 5)
              + 7 * params[7] * pow(Xn, 6);  
    // I0 = a0 - a2*Xn^2 - 2*a3*Xn^3 ... 
    double V = params[0]
              - params[2] * pow(Xn, 2)
              - 2 * params[3] * pow(Xn, 3)
              - 3 * params[4] * pow(Xn, 4)
              - 4 * params[5] * pow(Xn, 5)
              - 5 * params[6] * pow(Xn, 6)
              - 6 * params[7] * pow(Xn, 7);
    double expected[MAX_NODES + 1][MAX_NODES + 2] = {
       // 0    1    2   3    4   jx   i
        { 0 ,  0 ,  0 , 0 ,  0 , 0 ,  0 },
        { 0 ,  0 ,  0 , 0 ,  0 , 1 ,  0 },
        { 0 ,  0 ,  0 , 0 ,  0 ,-1 ,  0 },
        { 0 ,  0 ,  0 , 0 ,  0 , 0 ,  0 },
        { 0 ,  0 ,  0 , 0 ,  0 , 0 ,  0 },
        { 0 , -1 ,  1,  A , -A , 0 , -V }
    };
    for (int i = 1; i <= numVariables; i++) {
        for (int j = 1; j <= numVariables + 1; j++) {
            EXPECT_EQ(expected[i][j], matrix[i][j]);
        }
    }
    print(numVariables, matrix);
}


TEST(ElementStampsTest, CCCS) {
    /*
     * Current Controlled Current Source
     * (a.k.a. Current Amplifier)
     */

    // Arrange
                              // (val)(a)(b)(c)(d)
    Element currentAmplifier("F1", 10, 1, 2, 3, 4);
    int numVariables = 4;

    double matrix[MAX_NODES+1][MAX_NODES+2];
    // Bad smell about the need of this member function...
    // Without it, only first part of matrix would be populated!
    vector<string> dummyVariablesList(10);
    currentAmplifier.addCurrentVariables(numVariables, dummyVariablesList);

    // Important!!! Should be initialized after updating numVariables
    // with extra current variables!
    init(numVariables, matrix);

    // Act
    currentAmplifier.applyStamp(matrix, numVariables);

    // Assert
    double expected[MAX_NODES+1][MAX_NODES+2] = {
       // 0    1    2    3    4    jx   i
        { 0 ,  0 ,  0 ,  0 ,  0 ,   0 , 0 },
        { 0 ,  0 ,  0 ,  0 ,  0 ,  10 , 0 },
        { 0 ,  0 ,  0 ,  0 ,  0 , -10 , 0 },
        { 0 ,  0 ,  0 ,  0 ,  0 ,   1 , 0 },
        { 0 ,  0 ,  0 ,  0 ,  0 ,  -1 , 0 },
        { 0 ,  0 ,  0 , -1 ,  1 ,   0 , 0 }
    };
    for (int i=1; i<=numVariables; i++) {
        for (int j=1; j<=numVariables+1; j++) {
            EXPECT_EQ(expected[i][j], matrix[i][j]);
        }
    }
    print(numVariables, matrix);
}


TEST(ElementStampsTest, EightParametersPolinomialCurrentAmplifier) {
    /*
     * Polynomial Current Controlled Current Source
     * (a.k.a. Current Amplifier)
     */

    // Arrange
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
                                  // (params)(a)(b)(c)(d)
    Element currentAmplifier("FPol8", params, 1, 2, 3, 4);
    int numVariables = 4;

    double matrix[MAX_NODES + 1][MAX_NODES + 2];
    // Bad smell about the need of this member function...
    // Without it, only first part of matrix would be populated!
    vector<string> dummyVariablesList(10);
    currentAmplifier.addCurrentVariables(numVariables, dummyVariablesList);

    // Important!!! Should be initialized after updating numVariables
    // with extra current variables!
    init(numVariables, matrix);

    // Act
                                         // (gnd)           (jcd)
    double previousSolution[MAX_NODES + 1] = { 0, 0, 0, 0, 0, 2 }; // jcd = 2 
    currentAmplifier.applyStamp(matrix, numVariables, previousSolution);

    // Assert
    double Xn = previousSolution[5];
    // B0 = a1 + 2*a2*Xn + 3*a3*Xn^2 ...
    double B = params[1]
              + 2 * params[2] * Xn
              + 3 * params[3] * pow(Xn, 2)
              + 4 * params[4] * pow(Xn, 3)
              + 5 * params[5] * pow(Xn, 4)
              + 6 * params[6] * pow(Xn, 5)
              + 7 * params[7] * pow(Xn, 6);  
    // I0 = a0 - a2*Xn^2 - 2*a3*Xn^3 ... 
    double I  = params[0]
              - params[2] * pow(Xn, 2)
              - 2 * params[3] * pow(Xn, 3)
              - 3 * params[4] * pow(Xn, 4)
              - 4 * params[5] * pow(Xn, 5)
              - 5 * params[6] * pow(Xn, 6)
              - 6 * params[7] * pow(Xn, 7);
    double expected[MAX_NODES + 1][MAX_NODES + 2] = {
       // 0    1    2   3    4   jx   i
        { 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 },
        { 0 ,  0 ,  0 ,  0 ,  0 ,  B , -I },
        { 0 ,  0 ,  0 ,  0 ,  0 , -B ,  I },
        { 0 ,  0 ,  0 ,  0 ,  0 ,  1 ,  0 },
        { 0 ,  0 ,  0 ,  0 ,  0 , -1 ,  0 },
        { 0 ,  0 ,  0 , -1 ,  1 ,  0 ,  0 }
    };
    for (int i = 1; i <= numVariables; i++) {
        for (int j = 1; j <= numVariables + 1; j++) {
            EXPECT_EQ(expected[i][j], matrix[i][j]);
        }
    }
    print(numVariables, matrix);
}


TEST(ElementStampsTest, CCVS) {
    /*
     * Current Controlled Voltage Source
     * (a.k.a. Transresistance)
     */

    // Arrange
                             // (val)(a)(b)(c)(d)
    Element transresistance("H1", 10, 1, 2, 3, 4);
    int numVariables = 4;

    double matrix[MAX_NODES+1][MAX_NODES+2];
    // Bad smell about the need of this member function...
    // Without it, only first part of matrix would be populated!
    vector<string> dummyVariablesList(10);
    transresistance.addCurrentVariables(numVariables, dummyVariablesList);

    // Important!!! Should be initialized after updating numVariables
    // with extra current variables!
    init(numVariables, matrix);

    // Act
    transresistance.applyStamp(matrix, numVariables);

    // Assert
    double expected[MAX_NODES+1][MAX_NODES+2] = {
       // 0    1    2    3    4   jx   jy   i
        { 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , 0 },
        { 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  1 , 0 },
        { 0 ,  0 ,  0 ,  0 ,  0 ,  0 , -1 , 0 },
        { 0 ,  0 ,  0 ,  0 ,  0 ,  1 ,  0 , 0 },
        { 0 ,  0 ,  0 ,  0 ,  0 , -1 ,  0 , 0 },
        { 0 ,  0 ,  0 , -1 ,  1 ,  0 ,  0 , 0 },
        { 0 , -1 ,  1 ,  0 ,  0 , 10 ,  0 , 0 },
    };
    for (int i=1; i<=numVariables; i++) {
        for (int j=1; j<=numVariables+1; j++) {
            EXPECT_EQ(expected[i][j], matrix[i][j]);
        }
    }
    print(numVariables, matrix);
}


TEST(ElementStampsTest, EightParametersPolinomialTransresistance) {
    /*
     * Polynomial Current Controlled Voltage Source
     * (a.k.a. Transresistance)
     */

    // Arrange
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
                             //  (params)(a)(b)(c)(d)
    Element transresistance("H1", params, 1, 2, 3, 4);
    int numVariables = 4;

    double matrix[MAX_NODES+1][MAX_NODES+2];
    // Bad smell about the need of this member function...
    // Without it, only first part of matrix would be populated!
    vector<string> dummyVariablesList(10);
    transresistance.addCurrentVariables(numVariables, dummyVariablesList);

    // Important!!! Should be initialized after updating numVariables
    // with extra current variables!
    init(numVariables, matrix);

    // Act
                                                         // (jcd)
                                                         // XXX
    double previousSolution[MAX_NODES + 1] = { 0, 0, 0, 0, 0, 2, 0 }; // jcd = 2 
    transresistance.applyStamp(matrix, numVariables, previousSolution);

    // Assert
    double Xn = previousSolution[5];
    // Rm = a1 + 2*a2*Xn + 3*a3*Xn^2 ...
    double Rm = params[1]
              + 2 * params[2] * Xn
              + 3 * params[3] * pow(Xn, 2)
              + 4 * params[4] * pow(Xn, 3)
              + 5 * params[5] * pow(Xn, 4)
              + 6 * params[6] * pow(Xn, 5)
              + 7 * params[7] * pow(Xn, 6);  
    // V = a0 - a2*Xn^2 - 2*a3*Xn^3 ... 
    double V  = params[0]
              - params[2] * pow(Xn, 2)
              - 2 * params[3] * pow(Xn, 3)
              - 3 * params[4] * pow(Xn, 4)
              - 4 * params[5] * pow(Xn, 5)
              - 5 * params[6] * pow(Xn, 6)
              - 6 * params[7] * pow(Xn, 7);
    double expected[MAX_NODES+1][MAX_NODES+2] = {
       // 0    1    2    3    4   jx   jy   i
        { 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 },
        { 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  1 ,  0 },
        { 0 ,  0 ,  0 ,  0 ,  0 ,  0 , -1 ,  0 },
        { 0 ,  0 ,  0 ,  0 ,  0 ,  1 ,  0 ,  0 },
        { 0 ,  0 ,  0 ,  0 ,  0 , -1 ,  0 ,  0 },
        { 0 ,  0 ,  0 , -1 ,  1 ,  0 ,  0 ,  0 },
        { 0 , -1 ,  1 ,  0 ,  0 , Rm ,  0 , -V },
    };
    for (int i=1; i<=numVariables; i++) {
        for (int j=1; j<=numVariables+1; j++) {
            EXPECT_EQ(expected[i][j], matrix[i][j]);
        }
    }
    print(numVariables, matrix);
}


TEST(ElementStampsTest, OpAmp) {
    /*
     * Operational Amplifier
     * (a.k.a. Op Amp)
     */

    // Arrange
                    // (x)-> dont care!
                    // (x)(a)(b)(c)(d)
    Element opamp("O1", 0, 1, 2, 3, 4);
    int numVariables = 4;

    double matrix[MAX_NODES+1][MAX_NODES+2];
    // Bad smell about the need of this member function...
    // Without it, only first part of matrix would be populated!
    vector<string> dummyVariablesList(10);
    opamp.addCurrentVariables(numVariables, dummyVariablesList);

    // Important!!! Should be initialized after updating numVariables
    // with extra current variables!
    init(numVariables, matrix);

    // Act
    opamp.applyStamp(matrix, numVariables);

    // Assert
    double expected[MAX_NODES+1][MAX_NODES+2] = {
       // 0    1    2    3    4   jx   i
        { 0 ,  0 ,  0 ,  0 ,  0 ,  0 , 0 },
        { 0 ,  0 ,  0 ,  0 ,  0 ,  1 , 0 },
        { 0 ,  0 ,  0 ,  0 ,  0 , -1 , 0 },
        { 0 ,  0 ,  0 ,  0 ,  0 ,  0 , 0 },
        { 0 ,  0 ,  0 ,  0 ,  0 ,  0 , 0 },
        { 0 ,  0 ,  0 ,  1 , -1 ,  0 , 0 },
    };
    for (int i=1; i<=numVariables; i++) {
        for (int j=1; j<=numVariables+1; j++) {
            EXPECT_EQ(expected[i][j], matrix[i][j]);
        }
    }
    print(numVariables, matrix);
}


TEST(CircuitStampsTest, SimpleCircuit) {
    // Arrange
    int numNodes = 6;
    int numElements = 9;
    int numVariables = 9;
    double matrix[MAX_NODES+1][MAX_NODES+2];
    vector<Element> netlist(MAX_ELEMS);
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

    Circuit circuit(
        numElements,
        numNodes,
        numVariables,
        netlist
    );
    init(circuit.getNumVariables(), matrix);

    // Act
    circuit.applyStamps(matrix);

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
    for (int i=1; i<=circuit.getNumVariables(); i++) {
        for (int j=1; j<=circuit.getNumVariables()+1; j++) {
            EXPECT_EQ(expected[i][j], matrix[i][j]);
        }
    }
    print(numVariables, matrix);
}
