#include "circuits/circuit.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;


void printSolution(int nv, int nn, double Yn[MAX_NOS+1][MAX_NOS+2], vector<string> lista){
    string txt;
    cout << "Solucao:" << endl;
    txt = "Tensao";
    for (int i=1; i<=nv; i++) {
        if (i==nn+1)
            txt = "Corrente";
        cout << txt << " " << lista[i] << ": " << Yn[i][nv+1] << endl;
    }
}
