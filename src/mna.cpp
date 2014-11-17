/*
Demonstration of Modified Nodal Analysis
Originally by Antonio Carlos M. de Queiroz (acmq@coe.ufrj.br)
Modified by Dhiana Deva, Felipe de Leo, Silvino Vieira
*/

/*
Elementos aceitos e linhas do netlist:

Resistor:  R<nome> <no+> <no-> <resistencia>
VCCS:            G<nome> <io+> <io-> <vi+> <vi-> <transcondutancia>
VCVC:            E<nome> <vo+> <vo-> <vi+> <vi-> <ganho de tensao>
CCCS:            F<nome> <io+> <io-> <ii+> <ii-> <ganho de corrente>
CCVS:            H<nome> <vo+> <vo-> <ii+> <ii-> <transresistencia>
Fonte I:     I<nome> <io+> <io-> <corrente>
Fonte V:     V<nome> <vo+> <vo-> <tensao>
Amp. op.:  O<nome> <vo1> <vo2> <vi1> <vi2>

As fontes F e H tem o ramo de entrada em curto
O amplificador operacional ideal tem a saida suspensa
Os nos podem ser nomes
*/

#include "matrix/solve.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstdlib>
#include <cmath>

using namespace std;

#define MAX_LINHA 80
#define MAX_NOME 11
#define MAX_ELEM 50
#define MAX_NOS 50
#define TOLG 1e-9
#define DEBUG

typedef struct elemento { /* Elemento do netlist */
    string nome;
    double valor;
    int a,b,c,d,x,y;
} elemento;

elemento netlist[MAX_ELEM]; /* Netlist */

int
    ne, /* Elementos */
    nv, /* Variaveis */
    nn, /* Nos */
    i,j,k;

char
/* Foram colocados limites nos formatos de leitura para alguma protecao
     contra excesso de caracteres nestas variaveis */
    tipo,
    na[MAX_NOME],nb[MAX_NOME],nc[MAX_NOME],nd[MAX_NOME],
    *p;

double
    g,
    Yn[MAX_NOS+1][MAX_NOS+2];

/* Rotina que conta os nos e atribui numeros a eles */
int numero(char *nome, int &nv, vector<string> &lista){
    int i=0, achou=0;

    while (!achou && i<=nv)
        if (!(achou=!lista[i].compare(nome))) i++;
    if (!achou) {
        if (nv==MAX_NOS) {
            cout << "O programa so aceita ate " << nv <<  " nos" << endl;
            exit(1);
        }
        nv++;
        lista[nv] = nome;
        return nv; /* novo no */
    }
    else {
        return i; /* no ja conhecido */
    }
}

int main(void)
{
    ifstream arquivo;
    string nomearquivo,
           txt;
    vector<string> lista(MAX_NOME+2); /*Tem que caber jx antes do nome */

    cout << "Modified Nodal Analysis Demo" << endl;
    cout << "Originally by Antonio Carlos M. de Queiroz (acmq@coe.ufrj.br)" << endl;
    cout << "Modified by Dhiana Deva, Felipe de Leo, Silvino Vieira" << endl;
 denovo:
    /* Leitura do netlist */
    ne=0; nv=0; lista[0] = "0";
    cout << "Nome do arquivo com o netlist (ex: mna.net): ";
    cin >> nomearquivo;
    arquivo.open(nomearquivo.c_str(), ifstream::in);

    if( !arquivo.is_open() ){
        cout << "Arquivo " << nomearquivo << " inexistente" << endl;
        goto denovo;
    }
    cout << "Lendo netlist:" << endl;
    getline(arquivo, txt);
    cout << "Titulo: " << txt;
    while (getline(arquivo, txt)) {
        ne++; /* Nao usa o netlist[0] */
        if (ne>MAX_ELEM) {
            cout << "O programa so aceita ate " << MAX_ELEM << " elementos" << endl;
            exit(1);
        }
        txt[0]=toupper(txt[0]);
        tipo=txt[0];
        //TODO: verificar necessidade da string txt
        // ver se eh possivel usar stringstream no getline
        stringstream txtstream(txt);
        txtstream >> netlist[ne].nome;
        //TODO: talvez nao seja preciso usar p
        string p(txt, netlist[ne].nome.size(), string::npos);
        txtstream.str(p);
        /* O que e lido depende do tipo */
        if (tipo=='R' || tipo=='I' || tipo=='V') {
            txtstream >> na >> nb >> netlist[ne].valor;
            cout << netlist[ne].nome << " " << na << " " << nb << " " << netlist[ne].valor << endl;
            netlist[ne].a = numero(na, nv, lista);
            netlist[ne].b = numero(nb, nv, lista);
        }
        else if (tipo=='G' || tipo=='E' || tipo=='F' || tipo=='H') {
            txtstream >> na >> nb >> nc >> nd >> netlist[ne].valor;
            cout << netlist[ne].nome << " " << na << " " << nb << " " << nc << " "
                 << nd << " "<< netlist[ne].valor << endl;
            netlist[ne].a = numero(na, nv, lista);
            netlist[ne].b = numero(nb, nv, lista);
            netlist[ne].c = numero(nc, nv, lista);
            netlist[ne].d = numero(nd, nv, lista);
        }
        else if (tipo=='O') {
            txtstream >> na >> nb >> nc >> nd;
            cout << netlist[ne].nome << " " << na << " " << nb << " " << nc << " " << nd << " " << endl;
            netlist[ne].a = numero(na, nv, lista);
            netlist[ne].b = numero(nb, nv, lista);
            netlist[ne].c = numero(nc, nv, lista);
            netlist[ne].d = numero(nd, nv, lista);
        }
        else if (tipo=='*') { /* Comentario comeca com "*" */
            cout << "Comentario: " << txt;
            ne--;
        }
        else {
            cout << "Elemento desconhecido: " << txt << endl;
            cin.get();
            exit(1);
        }
    }
    arquivo.close();

    /* Acrescenta variaveis de corrente acima dos nos, anotando no netlist */
    nn=nv;
    for (i=1; i<=ne; i++) {
        tipo=netlist[i].nome[0];
        if (tipo=='V' || tipo=='E' || tipo=='F' || tipo=='O') {
            nv++;
            if (nv>MAX_NOS) {
                cout << "As correntes extra excederam o numero de variaveis permitido (" << MAX_NOS << ")" << endl;
                exit(1);
            }
            lista[nv] = "j"; /* Tem espaco para mais dois caracteres */
            lista[nv].append( netlist[i].nome );
            netlist[i].x=nv;
        }
        else if (tipo=='H') {
            nv=nv+2;
            if (nv>MAX_NOS) {
                cout << "As correntes extra excederam o numero de variaveis permitido (" << MAX_NOS << ")" << endl;
                exit(1);
            }
            lista[nv-1] = "jx";
            lista[nv-1].append(netlist[i].nome);
            netlist[i].x=nv-1;
            lista[nv] = "jy";
            lista[nv].append( netlist[i].nome );
            netlist[i].y=nv;
        }
    }
    cin.get();
    /* Lista tudo */
    cout << "Variaveis internas: " << endl;
    for (i=0; i<=nv; i++)
        cout << i << " -> " << lista[i] << endl;
    cin.get();
    cout << "Netlist interno final" << endl;
    for (i=1; i<=ne; i++) {
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
    cin.get();
    /* Monta o sistema nodal modificado */
    cout << "O circuito tem " << nn << " nos, " << nv << " variaveis e " << ne << " elementos" << endl;
    cin.get();
    /* Zera sistema */
    for (i=0; i<=nv; i++) {
        for (j=0; j<=nv+1; j++)
            Yn[i][j]=0;
    }
    /* Monta estampas */
    for (i=1; i<=ne; i++) {
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
        for (k=1; k<=nv; k++) {
            for (j=1; j<=nv+1; j++)
                if (Yn[k][j]!=0){
                    cout << setprecision( 1 ) << fixed << setw( 3 ) << showpos;
                    cout << Yn[k][j] << " ";
                }
                else cout << " ... ";
            cout << endl;
        }
        cin.get();
#endif
    }
    /* Resolve o sistema */
    if (solve(nv, Yn)) {
        cin.get();
        exit(0);
    }
#ifdef DEBUG
    /* Opcional: Mostra o sistema resolvido */
    cout << "Sistema resolvido:" << endl;
    for (i=1; i<=nv; i++) {
        for (j=1; j<=nv+1; j++)
            if (Yn[i][j]!=0){
                cout << setprecision( 1 ) << fixed << setw( 3 ) << showpos;
                cout << Yn[i][j] << " ";
            }
            else cout << " ... ";
        cout << endl;
    }
    cin.get();
#endif
    /* Mostra solucao */
    cout << "Solucao:" << endl;
    txt = "Tensao";
    for (i=1; i<=nv; i++) {
        if (i==nn+1)
            txt = "Corrente";
        cout << txt << " " << lista[i] << ": " << Yn[i][nv+1] << endl;
    }
    cin.get();
}

