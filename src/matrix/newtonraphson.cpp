#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include "consts.h"
#include "circuits/circuit.h"
#include "matrix/newtonraphson.h"


using namespace std;

static const int MAX_ATTEMPTS = 500;
static const int MAX_LOOPS = 1000;
static const double TOLERANCE = 1e-4;


void randomize(int numVariables, double (&solution)[MAX_NODES+1]){
    solution[0] = 0; // gnd
    for(int i=1; i<=numVariables; i++){
        solution[i] = (double(rand()) / double(RAND_MAX)) - 0.5;
    }
}


double calcDistance(int numVariables, double x[MAX_NODES+1], double y[MAX_NODES+1]){
    double sum=0;
    double distance=0;
    for(int i=1; i<=numVariables ;i++) {
        sum += pow((x[i]-y[i]),2);
    }
    distance = sqrt(sum);
    return distance;
}


void printSolution(int numVariables, double solution[MAX_NODES+1]){
    for(int i=1; i<=numVariables; i++){
        cout << solution[i] << endl;
    }
    cout << endl;
}


int runNewtonRaphson(Circuit circuit,
                     double (&finalSolution)[MAX_NODES+1],
                     double t,
                     double (&lastSolution)[MAX_NODES+1]){

    int rc=0;
    int numAttempts=0;
    int numLoops=0;
    bool converged=false;
    double solution[MAX_NODES+1];
    double distance=0;
    double previousSolution[MAX_NODES+1];
    double Yn[MAX_NODES+1][MAX_NODES+2];

    copySolution(circuit.getNumVariables(), ZERO_SOLUTION, previousSolution);

    while(!converged && numAttempts <= MAX_ATTEMPTS ){
        numAttempts++;
        numLoops=0;

        while(!converged && numLoops <= MAX_LOOPS){
            numLoops++;

            init(circuit.getNumVariables(), Yn);
            circuit.applyStamps(Yn,
                                previousSolution,
                                t,
                                lastSolution);

            rc = solve(circuit.getNumVariables(), Yn);
            if (rc)
                // Let's try a new randomized initial solution! 
                break;

            getSolution(circuit.getNumVariables(),
                        Yn,
                        solution);

            distance = calcDistance(circuit.getNumVariables(),
                                    solution,
                                    previousSolution);
            if (distance < TOLERANCE){
                converged = true;
                solution[0] = 0; // Ground!
                copySolution(circuit.getNumVariables(),
                             solution,
                             finalSolution);
            } else {
                copySolution(circuit.getNumVariables(),
                             solution,
                             previousSolution);
            }
        }
        randomize(circuit.getNumVariables(), previousSolution);
    }
    if (!converged){
        cout << "Newton Raphson did not converge.";
        #if defined (WIN32) || defined(_WIN32)
        cout << endl << "Press any key to exit...";
        cin.get();
        cin.get();
        #endif
        exit(EXIT_FAILURE);
    }
    return 0;
}
