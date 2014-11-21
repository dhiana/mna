#include "circuits/element.h"
#include "matrix/matrix.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstdlib>

using namespace std;

#define DEBUG

static const int MAX_LINHA = 80;
static const int MAX_NOME = 11;
static const int MAX_ELEM = 50;


void exitPolitely(int exitCode) {
	#if defined (WIN32) || defined(_WIN32)
	cout << endl << "Press any key to exit...";
	cin.get();
	cin.get();
	#endif
	exit(exitCode);
}


int main(int argc, char **argv){

    cout << endl;
    cout << "Modified Nodal Analysis" << endl;
    cout << "Originally by Antonio Carlos M. de Queiroz (acmq@coe.ufrj.br)" << endl;
    cout << "Modified by Dhiana Deva, Felipe de Leo and Silvino Vieira" << endl;
    cout << endl;

    ifstream netlistFile;
    string filepath;

    switch(argc) {
        case 1: {
            cout << "Enter path to netlist file: ";
            cin >> filepath;
            break;
        }
        case 2: {
            filepath = argv[1];
            break;
        }
        default:
            cerr << "FAILURE: Too much information!" << endl;
            exitPolitely(EXIT_FAILURE);
    }

    netlistFile.open(filepath.c_str(), ifstream::in);
    if(!netlistFile.is_open()){
        cerr << "FAILURE: Cannot open file " << filepath << endl;
		exitPolitely(EXIT_FAILURE);
    }

    string txt;
    vector<string> lista(MAX_NOME+2); /*Tem que caber jx antes do nome */
    lista[0] = "0";
    vector<Element> netlist(MAX_ELEM);

    int nv=0, /* Variaveis */
        nn=0; /* Nos */

    /* Foram colocados limites nos formatos de leitura para alguma protecao
       contra excesso de caracteres nestas variaveis */
    char tipo;

    double g,
           Yn[MAX_NOS+1][MAX_NOS+2];

    cout << "Lendo netlist:" << endl;
    getline(netlistFile, txt);
    cout << "Titulo: " << txt;
    while (getline(netlistFile, txt)) {
        Element::ne++; /* Nao usa o netlist[0] */
        if (Element::ne>MAX_ELEM) {
            cout << "O programa so aceita ate " << MAX_ELEM << " elementos" << endl;
			exitPolitely(EXIT_FAILURE);
        }
        netlist[Element::ne] = Element(txt, nv, lista);
    }
    netlistFile.close();

    /* Acrescenta variaveis de corrente acima dos nos, anotando no netlist */
    nn=nv;
    for (int i=1; i<=Element::ne; i++) {
        tipo=netlist[i].nome[0];
        if (tipo=='V' || tipo=='E' || tipo=='F' || tipo=='O') {
            nv++;
            if (nv>MAX_NOS) {
                cout << "As correntes extra excederam o numero de variaveis permitido (" << MAX_NOS << ")" << endl;
                exitPolitely(EXIT_FAILURE);
            }
            lista[nv] = "j"; /* Tem espaco para mais dois caracteres */
            lista[nv].append( netlist[i].nome );
            netlist[i].x=nv;
        }
        else if (tipo=='H') {
            nv=nv+2;
            if (nv>MAX_NOS) {
                cout << "As correntes extra excederam o numero de variaveis permitido (" << MAX_NOS << ")" << endl;
                exitPolitely(EXIT_FAILURE);
            }
            lista[nv-1] = "jx";
            lista[nv-1].append(netlist[i].nome);
            netlist[i].x=nv-1;
            lista[nv] = "jy";
            lista[nv].append( netlist[i].nome );
            netlist[i].y=nv;
        }
    }
    cout << endl;

    /* Lista tudo */
    cout << "Variaveis internas: " << endl;
    for (int i=0; i<=nv; i++)
        cout << i << " -> " << lista[i] << endl;
    cout << endl;

    cout << "Netlist interno final" << endl;
    for (int i=1; i<=Element::ne; i++) {
        tipo=netlist[i].nome[0];
        if (tipo=='R' || tipo=='I' || tipo=='V') {
            cout << netlist[i].nome << " " << netlist[i].a << " " << netlist[i].b << " " << netlist[i].valor << endl;
        }
        else if (tipo=='G' || tipo=='E' || tipo=='F' || tipo=='H') {
            cout << netlist[i].nome << " " << netlist[i].a << " " << netlist[i].b << " "
                 << netlist[i].c << " " << netlist[i].d   << " " << netlist[i].valor << endl;
        }
        else if (tipo=='O') {
            cout << netlist[i].nome << " " << netlist[i].a << " " << netlist[i].b << " "
                 << netlist[i].c << " " << netlist[i].d   << endl;
        }
        if (tipo=='V' || tipo=='E' || tipo=='F' || tipo=='O')
            cout << "Corrente jx: " << netlist[i].x << endl;
        else if (tipo=='H')
            cout << "Correntes jx e jy: " << netlist[i].x << ", " << netlist[i].y << endl;
    }
    cout << endl;

    /* Monta o sistema nodal modificado */
    cout << "O circuito tem " << nn << " nos, " << nv << " variaveis e " << Element::ne << " elementos" << endl;
    cout << endl;

    /* Zera sistema */
    for (int i=0; i<=nv; i++) {
        for (int j=0; j<=nv+1; j++)
            Yn[i][j]=0;
    }

    /* Monta estampas */
    for (int i=1; i<=Element::ne; i++) {
        tipo=netlist[i].nome[0];
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
        cout << "Sistema apos a estampa de " << netlist[i].nome << endl;
        for (int k=1; k<=nv; k++) {
            for (int j=1; j<=nv+1; j++)
                if (Yn[k][j]!=0){
                    cout << setprecision( 1 ) << fixed << setw( 3 ) << showpos;
                    cout << Yn[k][j] << " ";
                }
                else cout << " ... ";
            cout << endl;
        }
        cout << endl;
#endif

    }
    /* Resolve o sistema */
    if (solve(nv, Yn)) {
        cout << "FAILURE: Could not solve!" << endl;
        exitPolitely(EXIT_FAILURE);
    }

#ifdef DEBUG
    /* Opcional: Mostra o sistema resolvido */
    cout << "Sistema resolvido:" << endl;
    for (int i=1; i<=nv; i++) {
        for (int j=1; j<=nv+1; j++)
            if (Yn[i][j]!=0){
                cout << setprecision( 1 ) << fixed << setw( 3 ) << showpos;
                cout << Yn[i][j] << " ";
            }
            else cout << " ... ";
        cout << endl;
    }
    cout << endl;
#endif

    /* Mostra solucao */
    cout << "Solucao:" << endl;
    txt = "Tensao";
    for (int i=1; i<=nv; i++) {
        if (i==nn+1)
            txt = "Corrente";
        cout << txt << " " << lista[i] << ": " << Yn[i][nv+1] << endl;
    }
	exitPolitely(EXIT_SUCCESS);
}
