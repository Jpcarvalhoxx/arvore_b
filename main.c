#include "arvore_b.h"
#include "estruturas.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/*argv[2] - N° de registros
  arqv[3] - Tipo de Arquivo
  argv[4] - Key

*/

// Função para inicializar a árvore B
bool initialTree(TApontador *tree) {
  *tree = NULL; // Inicializa a árvore como nula
  return true;  // Retorna verdadeiro para indicar que a inicialização foi
                // bem-sucedida
}

// Função para imprimir os registros da árvore por níveis

// Função para inserir um registro na árvore B
void insert(Registro reg, TApontador *Ap, Runtime *_run) {
  bool cresceu = false;
  Registro regReturn;
  TApontador ApReturn = NULL;

  ins(reg, *Ap, &cresceu, &regReturn, &ApReturn,
      _run); // Chama a função de inserção

  if (cresceu) {
    TApontador ApTemp =
        (TApontador)malloc(sizeof(Page)); // Cria uma nova página
    ApTemp->n = 1;
    ApTemp->r[0] = regReturn;
    ApTemp->p[1] = ApReturn;
    ApTemp->p[0] = *Ap;
    *Ap = ApTemp; // Atualiza o ponteiro da raiz se a árvore cresceu
  }
}


// Função para preencher a árvore B com registros de um arquivo binário
void fillTheTree(TApontador *tree, char *nameArq, int quant, Runtime *_run) {
  // Inicializa a árvore B
  if (!initialTree(tree)) {
    printf("Erro na inicialização da árvore\n");
    exit(1);
  }

  // Abre o arquivo binário para leitura
  FILE *bin = fopen(nameArq, "rb");
  if (bin == NULL) {
    printf("Erro na abertura do arquivo\n");
    exit(1);
  }

  // Aloca espaço para armazenar os registros do arquivo
  Registro *regs = (Registro *)malloc(sizeof(Registro) * quant);

  // Lê os registros do arquivo binário
  fread(regs, sizeof(Registro), quant, bin);

  // Insere os registros na árvore B
  for (int i = 0; i < quant; i++) {
    insert(regs[i], tree, _run);
  }

  // Atualiza o contador de interações com a quantidade de registros inseridos
  _run->interactions += quant;

  // Libera a memória alocada para os registros
  free(regs);

  // Fecha o arquivo binário
  fclose(bin);
}

// Função para procurar um registro na árvore B
void search(Registro **reg, TApontador Ap, int key, Runtime *_run) {

  // Se o nó for nulo, atribui NULL ao ponteiro apontado por reg e encerra a
  // função
  if (Ap == NULL) {
    *reg = NULL;
    return;
  }

  long i = 1; // Variável de controle para a posição no nó durante a busca

  // Incrementa o contador de interações, pois estamos acessando um nó
  _run->interactions++;

  // Encontra a posição correta para a chave no nó atual
  while (i < Ap->n && key > Ap->r[i - 1].key) {
    _run->interactions +=
        1; // Incrementa o contador de interações durante a busca
    i++;
  }

  // Se a chave for encontrada no nó atual
  if (key == Ap->r[i - 1].key) {
    // Atribui o endereço do registro encontrado ao ponteiro apontado por reg
    *reg = &(Ap->r[i - 1]);
    _run->interactions += 1; // Incrementa o contador de interações
    return;
  }

  _run->interactions += 1; // Incrementa o contador de interações

  // Se a chave for menor que a chave no índice i-1, faz uma chamada recursiva à
  // esquerda
  if (key < Ap->r[i - 1].key) {
    search(reg, Ap->p[i - 1], key, _run);
  } else { // Se a chave for maior, faz uma chamada recursiva à direita
    search(reg, Ap->p[i], key, _run);
  }
}


// Função para inserir um registro em uma página da árvore B
void insertToPage(TApontador Ap, Registro reg, TApontador ApDir,
                  Runtime *_run) {
  bool notFoundPos;
  int k = Ap->n;

  // Verifica se a posição não foi encontrada
  notFoundPos = (k > 0);

  // Enquanto a posição não for encontrada
  while (notFoundPos) {
    _run->interactions++;

    // Se a chave do registro é maior ou igual à chave na posição k-1
    if (reg.key >= Ap->r[k - 1].key) {
      notFoundPos = false; // Posição encontrada, encerra o loop
      break;
    }

    // Move o registro e o ponteiro para a direita
    Ap->r[k] = Ap->r[k - 1];
    Ap->p[k + 1] = Ap->p[k];
    k--;

    // Se atingir a posição inicial, encerra o loop
    if (k < 1)
      notFoundPos = false;
  }

  // Insere o registro na posição encontrada
  Ap->r[k] = reg;
  Ap->p[k + 1] = ApDir;
  Ap->n++; // Incrementa o número de registros na página
}


// Função para inserir um registro em uma árvore B
void ins(Registro reg, TApontador Ap, bool *cresceu, Registro *regReturn,
         TApontador *ApReturn, Runtime *_run) {
  long i = 1;
  long j;
  TApontador ApTemp;

  // Se a página é nula, indica que houve crescimento na inserção
  if (Ap == NULL) {
    *cresceu = true;
    *regReturn = reg;
    *ApReturn = NULL;
    return;
  }

  _run->interactions++;

  // Encontra a posição correta para a inserção na página atual
  while (i < Ap->n && reg.key > Ap->r[i - 1].key) {
    i++;
    _run->interactions++;
  }

  _run->interactions++;

  // Se a chave já existe na página, imprime uma mensagem e encerra a função
  if (reg.key == Ap->r[i - 1].key) {
    printf("O registro já existe...");
    *cresceu = false;
  }

  _run->interactions++;

  // Se a chave é menor que a chave na posição encontrada, ajusta o índice
  if (reg.key < Ap->r[i - 1].key) {
    i--;
  }

  // Chama recursivamente a função para a próxima página
  ins(reg, Ap->p[i], cresceu, regReturn, ApReturn, _run);

  // Se não houve crescimento, encerra a função
  if (!*cresceu)
    return;

  // Se a página não está cheia, insere na página atual
  if (Ap->n < MM) {
    insertToPage(Ap, *regReturn, *ApReturn, _run);
    *cresceu = false;
    return;
  }

  // Cria uma página temporária para redistribuição
  ApTemp = (TApontador)malloc(sizeof(Page));
  ApTemp->n = 0;
  ApTemp->p[0] = NULL;

  if (i < M + 1) {
    // Insere o último elemento da página atual na página temporária
    insertToPage(ApTemp, Ap->r[MM - 1], Ap->p[MM], _run);
    Ap->n--;

    // Insere o novo registro na página atual
    insertToPage(Ap, *regReturn, *ApReturn, _run);
  } else {
    // Insere o novo registro na página temporária
    insertToPage(ApTemp, *regReturn, *ApReturn, _run);
  }

  // Transfere os elementos restantes da página atual para a página temporária
  for (j = M + 2; j <= MM; j++) {
    insertToPage(ApTemp, Ap->r[j - 1], Ap->p[j], _run);
  }

  Ap->n = M;
  ApTemp->p[0] = Ap->p[M + 1];
  *regReturn = Ap->r[M];
  *ApReturn = ApTemp;
}

// Função para liberar a memória da árvore B
void freeTree(TApontador tree) {
  if (tree == NULL)
    return;

  for (int i = 0; i <= tree->n; i++) {
    freeTree(tree->p[i]); // Recursivamente libera os filhos
  }

  free(tree); // Libera a memória da página atual
}

// Função para calcular o tempo de execução da CPU
void runtimeCPU(Runtime *r) {
  r->totalTime =
      (double)(r->endToCreateTre - r->startToCreateTre) / CLOCKS_PER_SEC;
}

void selectFile(int type, char *nameFile) {
  switch (type) {
  case 1:
    strcpy(nameFile, CRESCENTE);
    break;
  case 2:
    strcpy(nameFile, DECRESCENTE);
    break;
  case 3:
    strcpy(nameFile, ALEATORIO);
    break;
  default:
    printf("Parametro de arquivo ordenado errado (1- Crescente, 2 - "
           "Decrescente, 3 - Aleatorio\n");
    exit(1);
  }
}

bool convertArguments(char *arg, int *valor) {
  if (sscanf(arg, "%d", valor) != 1) {
    printf("Erro ao converter o argumento para inteiro: %s\n", arg);
    return false; // Indica falha na conversão
  }
  return true; // Indica sucesso na conversão
}

bool verifyArguments(int nRegs, int typeArq, int key) {
  if (nRegs <= 0 || nRegs > 2000000) {
    printf("Argumento inválido para quantidade de elementos: 1 <= N <=2000000");
    return false;
  }

  if (key > nRegs) {
    printf("Valor da chave maior que a quantidade de dadosn o arquivo...");
    return false;
  }
  if (typeArq != 1 && typeArq != 2 && typeArq != 3) {
    printf("Parametro de arquivo ordenado errado (1- Crescente, 2 - "
           "Decrescente, 3 - Aleatorio\n");

    return false;
  }

  return true;
}

int main(int argc, char *argv[]) {
  int nRegs = 0, typeArq = 0, key = 0;

  if (!convertArguments(argv[2], &nRegs) ||
      !convertArguments(argv[3], &typeArq) ||
      !convertArguments(argv[4], &key)) {
    exit(1);
  }

  if (!verifyArguments(nRegs, typeArq, key)) {
    exit(1);
  }

  char nameFile[TAM_MAX_NAME_FILE];

  selectFile(typeArq, nameFile);

  Runtime createTree;
  createTree.totalTime = 0.0;
  createTree.interactions = 0;

  Runtime searchInTree;
  searchInTree.totalTime = 0.0;
  searchInTree.interactions = 0;

  TApontador tree;
  Registro *reg;

  printf("-------------------------------------------------------------\n");

  printf("Criando Árvore B...\n");

  createTree.startToCreateTre = clock();

  fillTheTree(&tree, nameFile, nRegs, &createTree);

  createTree.endToCreateTre = clock();

  runtimeCPU(&createTree);

  printf(
      "Houve %d comparações na construção da Árvore B na memória principal\n",
      createTree.interactions);
  printf("Foi levado %fs para montar a árvore\n", createTree.totalTime);
  printf("\n");

  searchInTree.startToCreateTre = clock();
  search(&reg, tree, key, &searchInTree);
  searchInTree.endToCreateTre = clock();
  runtimeCPU(&searchInTree);

  printf("-------------------------------------------------------------\n");

  printf("Tempo de execução da CPU em segundos para pesquisar na árvore:%f\n",
         searchInTree.totalTime);
  printf("Houve %d comparações na pesquisada Árvore B na memória principal\n",
         searchInTree.interactions);
  searchInTree.interactions = 0;

  if (reg != NULL) {
    printf("Key: [%d] - %ld | %s\n", reg->key, reg->cod, reg->word);
  } else {
    printf("O registro não está presente na árvore\n");
  }

  printf("-------------------------------------------------------------\n");

  return 0;
}
