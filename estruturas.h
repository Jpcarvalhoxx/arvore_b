#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#define LEN 80
#define CRESCENTE "INSERT PATH"
#define DECRESCENTE "INSERT PATH"
#define ALEATORIO "INSERT PATH"
#define TAM_MAX_NAME_FILE 21

#include <time.h>

typedef int TChave;
typedef struct {
  TChave key;
  long int cod;
  char word[LEN + 2];

} Registro;

typedef struct {
  clock_t startToCreateTre, endToCreateTre;
  double totalTime;
  int interactions;

} Runtime;

void runtimeCPU(Runtime *r);

#endif