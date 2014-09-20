/*
Programa de demonstracao de analise nodal modificada compacta
Por Antonio Carlos M. de Queiroz acmq@coe.ufrj.br
Versao 1.0  - 16/06/2011 Implementa a analise nodal compacta com amp. ops.
Versao 1.0a - 03/11/2013 - Correcoes em *p e saida com sistema singular.
*/

/*
Elementos aceitos e linhas do netlist:

Resistor:  R<nome> <no+> <no-> <resistencia>
VCCS:      G<nome> <io+> <io-> <vi+> <vi-> <transcondutancia>
VCVC:      E<nome> <vo+> <vo-> <vi+> <vi-> <ganho de tensao>
CCCS:      F<nome> <io+> <io-> <ii+> <ii-> <ganho de corrente>
CCVS:      H<nome> <vo+> <vo-> <ii+> <ii-> <transresistencia>
Fonte I:   I<nome> <io+> <io-> <corrente>
Fonte V:   V<nome> <vo+> <vo-> <tensao>
Amp. op.:  O<nome> <vo1> <vo2> <vi1> <vi2>

As fontes F e H tem o ramo de entrada em curto
O amplificador operacional ideal tem a saida suspensa
Os nos podem ser nomes

Este programa implementa a analise nodal usando modelos baseados em
amplificadores operacionais ideais para os elementos V, F, E, H e O.
Os amplificadores operacionais causam reducoes no tamanho do sistema,
de forma a que o numero de equacoes nunca excede o numero de nos.
*/

#define versao "1.0a - 03/11/2013"
#include <stdio.h>
#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
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

typedef int tabela[MAX_NOS+1];

elemento netlist[MAX_ELEM]; /* Netlist */

int
  ne, /* Elementos */
  nv, /* Variaveis */
  neq,/* Equacoes */
  nn, /* Nos */
  i,j,k;

tabela C,L;

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

/* Resolucao de sistema de equacoes lineares.
   Metodo de Gauss-Jordan com condensacao pivotal */
int resolversistema(void)
{
  int i,j,l, a;
  double t, p;

  for (i=1; i<=neq; i++) {
    t=0.0;
    a=i;
    for (l=i; l<=neq; l++) {
      if (fabs(Yn[l][i])>fabs(t)) {
	a=l;
	t=Yn[l][i];
      }
    }
    if (i!=a) {
      for (l=1; l<=neq+1; l++) {
	p=Yn[i][l];
	Yn[i][l]=Yn[a][l];
	Yn[a][l]=p;
      }
    }
    if (fabs(t)<TOLG) {
      printf("Sistema singular\n");
      return 1;
    }
    for (j=neq+1; j>0; j--) {  /* Basta j>i em vez de j>0 */
      Yn[i][j] /= t;
      p=Yn[i][j];
      for (l=1; l<=neq; l++) {
	if (l!=i)
	  Yn[l][j]-=Yn[l][i]*p;
      }
    }
  }
  return 0;
}

/* Rotina que conta os nos e atribui numeros a eles */
int numero(char *nome)
{
  int i,achou;

  i=0; achou=0;
  while (!achou && i<=nv)
    if (!(achou=!strcmp(nome,lista[i]))) i++;
  if (!achou) {
    if (nv==MAX_NOS) {
      printf("O programa so aceita ate %d nos\n",nv);
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

void testarnos(void) {
 if (nv>MAX_NOS) {
   printf("As variaveis extra excederam o numero de variaveis permitido (%d)\n",MAX_NOS);
   exit(1);
 }
}

void somar(int *Q, int a, int b) {
  int i,a1,b1;

  if (Q[a]>Q[b]) {a1=Q[b]; b1=Q[a];}
  else {a1=Q[a]; b1=Q[b];}
  if (a1==b1) {printf("Circuito invalido"); exit(1);}
  for (i=1; i<=MAX_NOS; i++) {
    if (Q[i]==b1) Q[i]=a1;
    if (Q[i]>b1) Q[i]--;
  }
}

void operacional (int na,int nb,int nc,int nd) {
#ifdef DEBUG
  printf("Saida: %d %d; entrada %d %d\n",na,nb,nc,nd);
#endif
  somar(L,na,nb);
  somar(C,nc,nd);
}

void transcondutancia(double gm,int n1,int n2,int n3,int n4) {
  Yn[L[n1]][C[n3]]+=gm;
  Yn[L[n2]][C[n4]]+=gm;
  Yn[L[n1]][C[n4]]-=gm;
  Yn[L[n2]][C[n3]]-=gm;
}

void condutancia(double g, int a, int b){
  transcondutancia(g,a,b,a,b);
}

void corrente(double i, int a, int b) {
  Yn[L[a]][neq+1]-=i;
  Yn[L[b]][neq+1]+=i;
}

int main(void)
{
  system("clear");
  printf("Programa demonstrativo de analise nodal modificada\n");
  printf("Por Antonio Carlos M. de Queiroz - acmq@coe.ufrj.br\n");
  printf("Versao %s\n",versao);
  for(i=0; i<=MAX_NOS; i++) {C[i]=i; L[i]=i;} /* Inicializa tabelas */
 denovo:
  /* Leitura do netlist */
  ne=0; nv=0; strcpy(lista[0],"0");
  printf("Nome do arquivo com o netlist (ex: mna.net): ");
  scanf("%50s",nomearquivo);
  arquivo=fopen(nomearquivo,"r");
  if (arquivo==0) {
    printf("Arquivo %s inexistente\n",nomearquivo);
    goto denovo;
  }
  printf("Lendo netlist:\n");
  fgets(txt,MAX_LINHA,arquivo);
  printf("Titulo: %s",txt);
  while (fgets(txt,MAX_LINHA,arquivo)) {
    ne++; /* Nao usa o netlist[0] */
    if (ne>MAX_ELEM) {
      printf("O programa so aceita ate %d elementos\n",MAX_ELEM);
      exit(1);
    }
    txt[0]=toupper(txt[0]);
    tipo=txt[0];
    sscanf(txt,"%10s",netlist[ne].nome);
    p=txt+strlen(netlist[ne].nome); /* Inicio dos parametros */
    /* O que e lido depende do tipo */
    if (tipo=='R' || tipo=='I' || tipo=='V') {
      sscanf(p,"%10s%10s%lg",na,nb,&netlist[ne].valor);
      printf("%s %s %s %g\n",netlist[ne].nome,na,nb,netlist[ne].valor);
      netlist[ne].a=numero(na);
      netlist[ne].b=numero(nb);
    }
    else if (tipo=='G' || tipo=='E' || tipo=='F' || tipo=='H') {
      sscanf(p,"%10s%10s%10s%10s%lg",na,nb,nc,nd,&netlist[ne].valor);
      printf("%s %s %s %s %s %g\n",netlist[ne].nome,na,nb,nc,nd,netlist[ne].valor);
      netlist[ne].a=numero(na);
      netlist[ne].b=numero(nb);
      netlist[ne].c=numero(nc);
      netlist[ne].d=numero(nd);
    }
    else if (tipo=='O') {
      sscanf(p,"%10s%10s%10s%10s",na,nb,nc,nd);
      printf("%s %s %s %s %s\n",netlist[ne].nome,na,nb,nc,nd);
      netlist[ne].a=numero(na);
      netlist[ne].b=numero(nb);
      netlist[ne].c=numero(nc);
      netlist[ne].d=numero(nd);
    }
    else if (tipo=='*') { /* Comentario comeca com "*" */
      printf("Comentario: %s",txt);
      ne--;
    }
    else {
      printf("Elemento desconhecido: %s\n",txt);
      getch();
      exit(1);
    }
  }
  fclose(arquivo);
  /* Atualiza as tabelas e acrescenta variaveis de corrente acima dos nos, anotando no netlist */
#ifdef DEBUG
  printf("Amplificadores operacionais:\n");
#endif
  nn=nv; neq=nn;
  for (i=1; i<=ne; i++) {
    tipo=netlist[i].nome[0];
    if (tipo=='V') {
      nv++;
      testarnos();
      strcpy(lista[nv],"j"); /* Tem espaco para mais dois caracteres */
      strcat(lista[nv],netlist[i].nome);
      netlist[i].x=nv;
      operacional(netlist[i].a,netlist[i].b,0,netlist[i].x);
    }
    else if (tipo=='O') {
      operacional(netlist[i].a,netlist[i].b,netlist[i].c,netlist[i].d);
      neq--;
    }
    else if (tipo=='E') {
      nv++;
      testarnos();
      strcpy(lista[nv],"j"); /* Tem espaco para mais dois caracteres */
      strcat(lista[nv],netlist[i].nome);
      netlist[i].x=nv;
      operacional(netlist[i].a,netlist[i].b,0,netlist[i].x);
    }
    else if (tipo=='F') {
      nv++;
      testarnos();
      strcpy(lista[nv],"j"); /* Tem espaco para mais dois caracteres */
      strcat(lista[nv],netlist[i].nome);
      netlist[i].x=nv;
      operacional(netlist[i].x,0,netlist[i].c,netlist[i].d);
    }
    else if (tipo=='H') {
      nv=nv+2;
      testarnos();
      strcpy(lista[nv-1],"jx"); strcat(lista[nv-1],netlist[i].nome);
      netlist[i].x=nv-1;
      strcpy(lista[nv],"jy"); strcat(lista[nv],netlist[i].nome);
      netlist[i].y=nv;
      operacional(netlist[i].a,netlist[i].b,0,netlist[i].y);
      operacional(netlist[i].x,0,netlist[i].c,netlist[i].d);
    }
  }
  getch();
  /* Lista tudo */
  printf("Variaveis internas: \n");
  for (i=0; i<=nv; i++)
    printf("%d -> %s (%d)\n",i,lista[i],C[i]);
  getch();
  printf("Netlist interno final\n");
  for (i=1; i<=ne; i++) {
    tipo=netlist[i].nome[0];
    if (tipo=='R' || tipo=='I' || tipo=='V') {
      printf("%s %d %d %g\n",netlist[i].nome,netlist[i].a,netlist[i].b,netlist[i].valor);
    }
    else if (tipo=='G' || tipo=='E' || tipo=='F' || tipo=='H') {
      printf("%s %d %d %d %d %g\n",netlist[i].nome,netlist[i].a,netlist[i].b,netlist[i].c,netlist[i].d,netlist[i].valor);
    }
    else if (tipo=='O') {
      printf("%s %d %d %d %d\n",netlist[i].nome,netlist[i].a,netlist[i].b,netlist[i].c,netlist[i].d);
    }
    if (tipo=='V' || tipo=='E' || tipo=='F' || tipo=='O')
      printf("Corrente jx: %d\n",netlist[i].x);
    else if (tipo=='H')
      printf("Correntes jx e jy: %d, %d\n",netlist[i].x,netlist[i].y);
  }
  getch();
  /* Monta o sistema nodal modificado */
  printf("O circuito tem %d nos, %d variaveis, %d equacoes e %d elementos\n",nn,nv,neq,ne);
  getch();
  /* Zera sistema */
  for (i=0; i<=neq; i++) {
    for (j=0; j<=neq+1; j++)
      Yn[i][j]=0;
  }
  /* Monta estampas */
  for (i=1; i<=ne; i++) {
    tipo=netlist[i].nome[0];
    if (tipo=='R') condutancia(1/netlist[i].valor,netlist[i].a,netlist[i].b);
    else if (tipo=='G') transcondutancia(netlist[i].valor,netlist[i].a,netlist[i].b,netlist[i].c,netlist[i].d);
    else if (tipo=='I') corrente(netlist[i].valor,netlist[i].a,netlist[i].b);
    else if (tipo=='V') {
      transcondutancia(1,0,netlist[i].x,netlist[i].a,netlist[i].b);
      corrente(netlist[i].valor,netlist[i].x,0);
    }
    else if (tipo=='E') {
      transcondutancia(1,0,netlist[i].x,netlist[i].a,netlist[i].b);
      transcondutancia(netlist[i].valor,netlist[i].x,0,netlist[i].c,netlist[i].d);
    }
    else if (tipo=='F') {
      transcondutancia(netlist[i].valor,netlist[i].a,netlist[i].b,netlist[i].x,0);
      transcondutancia(1,netlist[i].c,netlist[i].d,netlist[i].x,0);
    }
    else if (tipo=='H') {
      transcondutancia(1,0,netlist[i].y,netlist[i].a,netlist[i].b);
      transcondutancia(netlist[i].valor,netlist[i].y,0,netlist[i].x,0);
      transcondutancia(1,netlist[i].c,netlist[i].d,netlist[i].x,0);
    }
    else if (tipo=='O') {
    }
#ifdef DEBUG
    /* Opcional: Mostra o sistema apos a montagem da estampa */
    printf("Sistema apos a estampa de %s\n",netlist[i].nome);
    for (k=1; k<=neq; k++) {
      for (j=1; j<=neq+1; j++)
        if (Yn[k][j]!=0) printf("%+3.1f ",Yn[k][j]);
        else printf(" ... ");
      printf("\n");
    }
    getch();
#endif
  }
  /* Resolve o sistema */
  if (resolversistema()) {
    getch();
    exit(0);   
  }
#ifdef DEBUG
  /* Opcional: Mostra o sistema resolvido */
  printf("Sistema resolvido:\n");
  for (i=1; i<=neq; i++) {
      for (j=1; j<=neq+1; j++)
        if (Yn[i][j]!=0) printf("%+3.1f ",Yn[i][j]);
        else printf(" ... ");
      printf("\n");
    }
  getch();
#endif
  /* Mostra solucao */
  printf("Solucao:\n");
  strcpy(txt,"Tensao");
  for (i=1; i<=nv; i++) {
    if (i==nn+1) strcpy(txt,"Corrente");
    if (C[i]!=0)
      printf("%s %s (%d): %g\n",txt,lista[i],C[i],Yn[C[i]][neq+1]);
    else
      printf("%s %s (%d): nao calculada\n",txt,lista[i],C[i]);
  }
  getch();
}

