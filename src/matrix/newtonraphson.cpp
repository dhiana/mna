#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include "consts.h"
#include "circuits/circuit.h"
#include "matrix/newtonraphson.h"


using namespace std;

static const int MAX_ATTEMPTS = 2;
static const int MAX_LOOPS = 3;
static const int TOLERANCE = 1e-2;


void randomize(int numVariables, double solution[MAX_NODES+1]){
    solution[0]; // gnd
    for(int i=1; i<=numVariables; i++){
        solution[i] = (double) (rand()%10);
    }
}


double calcDistance(int numVariables, double x[MAX_NODES+1], double y[MAX_NODES+1]){
    double sum=0;
    double distance=0;
    for(int i=1; i<=numVariables ;i++) {
        sum += pow((x[i]-y[i]),2);
        distance = sqrt(sum);
    }
    return distance;
}


void printSolution(int numVariables, double solution[MAX_NODES+1]){
    for(int i=1; i<=numVariables; i++){
        cout << solution[i] << endl;
    }
    cout << endl;
}


int runNewtonRaphson(Circuit circuit,
                     double finalSolution[MAX_NODES+1]){

    int rc=0;
    int numAttempts=0;
    int numLoops=0;
    bool converged=false;
    double solution[MAX_NODES+1];
    double distance=0;
    double previousSolution[MAX_NODES+1];
    double Yn[MAX_NODES+1][MAX_NODES+2];

    while(!converged && numAttempts <= MAX_ATTEMPTS ){
        numAttempts++;
        numLoops=0;

        randomize(circuit.getNumVariables(), previousSolution);
        cout << "Previous" << endl;
        printSolution(circuit.getNumVariables(), previousSolution);

        while(!converged && numLoops <= MAX_LOOPS){
            numLoops++;
            init(circuit.getNumVariables(), Yn);
            circuit.applyStamps(Yn,
                                previousSolution);
            cout << "Matrix" << endl;
            print(circuit.getNumVariables(), Yn);

            rc = solve(circuit.getNumVariables(), Yn);
            cout << "rc" << rc << endl;
            if (rc)
                // Let's try a new randomized initial solution! 
                break;

            getSolution(circuit.getNumVariables(),
                        Yn,
                        solution);

            cout << "Solution" << endl;
            printSolution(circuit.getNumVariables(), solution);
            cout << endl;

            distance = calcDistance(circuit.getNumVariables(),
                                    solution,
                                    previousSolution);
            cout << "distance: " << distance << endl;
            if (distance < TOLERANCE){
                converged = true;
                solution[0] = 0; // Ground!
                copySolution(circuit.getNumVariables(),
                             solution,
                             finalSolution);
            } else { 
                cout << "Solution before" << endl;
                printSolution(circuit.getNumVariables(), previousSolution);
                cout << endl;

                copySolution(circuit.getNumVariables(),
                             solution,
                             previousSolution);

                cout << "Solution after" << endl;
                printSolution(circuit.getNumVariables(), solution);
                cout << endl;

            }
        }

        if (converged)
            finalSolution = solution;
        else
            return (EXIT_FAILURE);
    }
    return 0;
}
