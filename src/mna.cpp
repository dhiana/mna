/*
Programa de demonstracao de analise nodal modificada
Por Antonio Carlos M. de Queiroz acmq@coe.ufrj.br
Versao 1.0 - 6/9/2000
Versao 1.0a - 8/9/2000 Ignora comentarios
Versao 1.0b - 15/9/2000 Aumentado Yn, retirado Js
Versao 1.0c - 19/2/2001 Mais comentarios
Versao 1.0d - 16/2/2003 Tratamento correto de nomes em minusculas
Versao 1.0e - 21/8/2008 Estampas sempre somam. Ignora a primeira linha
Versao 1.0f - 21/6/2009 Corrigidos limites de alocacao de Yn
Versao 1.0g - 15/10/2009 Le as linhas inteiras
Versao 1.0h - 18/6/2011 Estampas correspondendo a modelos
Versao 1.0i - 03/11/2013 Correcoes em *p e saida com sistema singular.
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

#define versao "1.0i - 03/11/2013"
#include "matrix/solve.h"
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <cmath>

using namespace std;

#define MAX_LINHA 80
#define MAX_NOME 11
#define MAX_ELEM 50
#define MAX_NOS 50
#define TOLG 1e-9
#define DEBUG

typedef struct elemento { /* Elemento do netlist */
    char nome[MAX_NOME];
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
    nomearquivo[MAX_LINHA+1],
    tipo,
    na[MAX_NOME],nb[MAX_NOME],nc[MAX_NOME],nd[MAX_NOME],
    lista[MAX_NOS+1][MAX_NOME+2], /*Tem que caber jx antes do nome */
    txt[MAX_LINHA+1],
    *p;
FILE *arquivo;

double
    g,
    Yn[MAX_NOS+1][MAX_NOS+2];

/* Rotina que conta os nos e atribui numeros a eles */
int numero(char *nome)
{
    int i,achou;

    i=0; achou=0;
    while (!achou && i<=nv)
        if (!(achou=!strcmp(nome,lista[i]))) i++;
    if (!achou) {
        if (nv==MAX_NOS) {
            cout << "O programa so aceita ate " << nv <<  " nos" << endl;
            exit(1);
        }
        nv++;
        strcpy(lista[nv],nome);
        return nv; /* novo no */
    }
    else {
        return i; /* no ja conhecido */
    }
}

int main(void)
{
    system("clear");
    cout << "Programa demonstrativo de analise nodal modificada" << endl;
    cout << "Por Antonio Carlos M. de Queiroz - acmq@coe.ufrj.br" << endl;
    cout << "Versao " << versao << endl;
 denovo:
    /* Leitura do netlist */
    ne=0; nv=0; strcpy(lista[0],"0");
    cout << "Nome do arquivo com o netlist (ex: mna.net): ";
    scanf("%50s",nomearquivo);
    arquivo=fopen(nomearquivo,"r");

    if (arquivo==0) {
        cout << "Arquivo " << nomearquivo << " inexistente" << endl;
        goto denovo;
    }
    cout << "Lendo netlist:" << endl;
    fgets(txt,MAX_LINHA,arquivo);
    cout << "Titulo: " << txt;
    while (fgets(txt,MAX_LINHA,arquivo)) {
        ne++; /* Nao usa o netlist[0] */
        if (ne>MAX_ELEM) {
            cout << "O programa so aceita ate " << MAX_ELEM << " elementos" << endl;
            exit(1);
        }
        txt[0]=toupper(txt[0]);
        tipo=txt[0];
        sscanf(txt,"%10s",netlist[ne].nome);
        p=txt+strlen(netlist[ne].nome); /* Inicio dos parametros */
        /* O que e lido depende do tipo */
        if (tipo=='R' || tipo=='I' || tipo=='V') {
            sscanf(p,"%10s%10s%lg",na,nb,&netlist[ne].valor);
            cout << netlist[ne].nome << " " << na << " " << nb << " " << netlist[ne].valor << endl;
            netlist[ne].a=numero(na);
            netlist[ne].b=numero(nb);
        }
        else if (tipo=='G' || tipo=='E' || tipo=='F' || tipo=='H') {
            sscanf(p,"%10s%10s%10s%10s%lg",na,nb,nc,nd,&netlist[ne].valor);
            cout << netlist[ne].nome << " " << na << " " << nb << " " << nc << " "
                 << nd << " "<< netlist[ne].valor << endl;
            netlist[ne].a=numero(na);
            netlist[ne].b=numero(nb);
            netlist[ne].c=numero(nc);
            netlist[ne].d=numero(nd);
        }
        else if (tipo=='O') {
            sscanf(p,"%10s%10s%10s%10s",na,nb,nc,nd);
            cout << netlist[ne].nome << " " << na << " " << nb << " " << nc << " " << nd << " " << endl;
            netlist[ne].a=numero(na);
            netlist[ne].b=numero(nb);
            netlist[ne].c=numero(nc);
            netlist[ne].d=numero(nd);
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
    fclose(arquivo);
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
            strcpy(lista[nv],"j"); /* Tem espaco para mais dois caracteres */
            strcat(lista[nv],netlist[i].nome);
            netlist[i].x=nv;
        }
        else if (tipo=='H') {
            nv=nv+2;
            if (nv>MAX_NOS) {
                cout << "As correntes extra excederam o numero de variaveis permitido (" << MAX_NOS << ")" << endl;
                exit(1);
            }
            strcpy(lista[nv-1],"jx"); strcat(lista[nv-1],netlist[i].nome);
            netlist[i].x=nv-1;
            strcpy(lista[nv],"jy"); strcat(lista[nv],netlist[i].nome);
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
    strcpy(txt,"Tensao");
    for (i=1; i<=nv; i++) {
        if (i==nn+1) strcpy(txt,"Corrente");
        cout << txt << " " << lista[i] << ": " << Yn[i][nv+1] << endl;
    }
    cin.get();
}

