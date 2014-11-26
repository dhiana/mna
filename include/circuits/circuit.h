#ifndef MNA_CIRCUIT_CIRCUIT_H_
#define MNA_CIRCUIT_CIRCUIT_H_
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
        void applyStamps(double Yn[MAX_NODES+1][MAX_NODES+2]);

        void printVariables();
        void printSummary();
        void printSolution(double Yn[MAX_NODES+1][MAX_NODES+2]);

        bool WriteSolutionToFile(
            string filename,
            double Yn[MAX_NODES+1][MAX_NODES+2]
        );

        int getNumElements();
        int getNumNodes();
        int getNumVariables();

    private:

        int numElements;
        int numNodes;
        int numVariables;
        string title;
        vector<string> variablesList;
        vector<Element> netlist;

};

#endif
