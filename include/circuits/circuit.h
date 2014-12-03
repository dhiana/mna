#ifndef MNA_CIRCUIT_CIRCUIT_H_
#define MNA_CIRCUIT_CIRCUIT_H_
#include <list>
#include <vector>
#include <string>
#include "circuits/element.h"
#include "matrix/matrix.h"
#include "consts.h"


using namespace std;

class Circuit {

    public:

        Circuit();
        Circuit(ifstream &netlistFile);
        Circuit(
            int numElements,
            int numNodes,
            int numVariables,
            vector<Element> &netlist
        );

        // Will accept previousSolution in the near future...
        // And ppossibly the PoO/Time...
        void applyStamps(double Yn[MAX_NODES+1][MAX_NODES+2],						 
                         double (&previousSolution)[MAX_NODES+1]=ZERO_SOLUTION, // for Newton Raphson
                         double t=0,
                         double (&lastStepSolution)[MAX_NODES+1]=ZERO_SOLUTION); // for Backward Euler

        void AddCoupling();
        void printVariables();
        void printSummary();
        void printSolution(double Yn[MAX_NODES+1][MAX_NODES+2]);

        void writeSolutionsHeader(ofstream &file);
        void appendSolutionToFile(ofstream &file,
                                  double solution[MAX_NODES+1],
                                  double t=0);

        int getNumElements();
        int getNumNodes();
        int getNumVariables();

        double getStep();
        double getFinalTime();

    private:

        int numElements;
        int numNodes;
        int numVariables;
        int CountCoupling;
        /* .TRAN */
        double step;
        double finalTime;
        double internSteps;
        //
        string title;
        vector<string> variablesList;
        vector<Element> netlist;

        list<string> inductorList;
        unsigned numInductor;

};

#endif
