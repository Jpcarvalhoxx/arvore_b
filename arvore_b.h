#ifndef ARVORE_B
#define ARVORE_B

#define M 2
#define MM (M * 2)

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "estruturas.h"

typedef struct Page *TApontador;

typedef struct Page
{
    int n;
    Registro r[MM];
    TApontador p[MM + 1];
} Page;

bool initialTree(TApontador *Dict);
void printLvlTree(TApontador p, int nivel);
void printTree(TApontador p);
void search(Registro **reg, TApontador Ap, int k, Runtime *_run);
void fillTheTree(TApontador *tree, char *nameArq, int quant, Runtime *_run);
void insertToPage(TApontador Ap, Registro reg, TApontador ApDir, Runtime *_run);
void ins(Registro reg, TApontador Ap, bool *cresceu, Registro *regReturn,
         TApontador *ApReturn, Runtime *_run);
void freeTree(TApontador tree);
void insert(Registro reg, TApontador *Ap, Runtime *_run);
#endif