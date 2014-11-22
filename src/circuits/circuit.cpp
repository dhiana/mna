#include "circuits/circuit.h"
#include "circuits/element.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;


void applyStamps(int numElements, int nv, vector<Element> netlist, double Yn[MAX_NOS+1][MAX_NOS+2]){
    double g;
    char tipo;
    for (int i=1; i<=numElements; i++) {
        tipo=netlist[i].getType();
        if (tipo=='R') {
            g=1/netlist[i].valor;
            Yn[netlist[i].a][netlist[i].a]+=g;
            Yn[netlist[i].b][netlist[i].b]+=g;
            Yn[netlist[i].a][netlist[i].b]-=g;
            Yn[netlist[i].b][netlist[i].a]-=g;
        }
        else if (tipo=='G') {
            g=netlist[i].valor;
            Yn[netlist[i].a][netlist[i].c]+=g;
            Yn[netlist[i].b][netlist[i].d]+=g;
            Yn[netlist[i].a][netlist[i].d]-=g;
            Yn[netlist[i].b][netlist[i].c]-=g;
        }
        else if (tipo=='I') {
            g=netlist[i].valor;
            Yn[netlist[i].a][nv+1]-=g;
            Yn[netlist[i].b][nv+1]+=g;
        }
        else if (tipo=='V') {
            Yn[netlist[i].a][netlist[i].x]+=1;
            Yn[netlist[i].b][netlist[i].x]-=1;
            Yn[netlist[i].x][netlist[i].a]-=1;
            Yn[netlist[i].x][netlist[i].b]+=1;
            Yn[netlist[i].x][nv+1]-=netlist[i].valor;
        }
        else if (tipo=='E') {
            g=netlist[i].valor;
            Yn[netlist[i].a][netlist[i].x]+=1;
            Yn[netlist[i].b][netlist[i].x]-=1;
            Yn[netlist[i].x][netlist[i].a]-=1;
            Yn[netlist[i].x][netlist[i].b]+=1;
            Yn[netlist[i].x][netlist[i].c]+=g;
            Yn[netlist[i].x][netlist[i].d]-=g;
        }
        else if (tipo=='F') {
            g=netlist[i].valor;
            Yn[netlist[i].a][netlist[i].x]+=g;
            Yn[netlist[i].b][netlist[i].x]-=g;
            Yn[netlist[i].c][netlist[i].x]+=1;
            Yn[netlist[i].d][netlist[i].x]-=1;
            Yn[netlist[i].x][netlist[i].c]-=1;
            Yn[netlist[i].x][netlist[i].d]+=1;
        }
        else if (tipo=='H') {
            g=netlist[i].valor;
            Yn[netlist[i].a][netlist[i].y]+=1;
            Yn[netlist[i].b][netlist[i].y]-=1;
            Yn[netlist[i].c][netlist[i].x]+=1;
            Yn[netlist[i].d][netlist[i].x]-=1;
            Yn[netlist[i].y][netlist[i].a]-=1;
            Yn[netlist[i].y][netlist[i].b]+=1;
            Yn[netlist[i].x][netlist[i].c]-=1;
            Yn[netlist[i].x][netlist[i].d]+=1;
            Yn[netlist[i].y][netlist[i].x]+=g;
        }
        else if (tipo=='O') {
            Yn[netlist[i].a][netlist[i].x]+=1;
            Yn[netlist[i].b][netlist[i].x]-=1;
            Yn[netlist[i].x][netlist[i].c]+=1;
            Yn[netlist[i].x][netlist[i].d]-=1;
        }

#ifdef DEBUG
        /* Opcional: Mostra o sistema apos a montagem da estampa */
        cout << "Sistema apos a estampa de " << netlist[i].name << endl;
        print(nv, Yn);
#endif

    }
}

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
