#include "arvore_b.h"
#include <stdio.h>

// Função para inicializar a árvore B
bool initialTree(TApontador *tree) {
  *tree = NULL; // Inicializa a árvore como nula
  return true;  // Retorna verdadeiro para indicar que a inicialização foi
                // bem-sucedida
}

// Função para imprimir os registros da árvore por níveis
void printLvlTree(TApontador p, int nivel) {
  long i;
  if (p == NULL)
    return; // Se o nó for nulo, retorna

  printf("Nivel %d : ", nivel); // Imprime o nível atual
  for (i = 0; i < p->n; i++)
    printf("%ld ", (long)p->r[i].key); // Imprime as chaves dos registros no nó
  putchar('\n');                       // Quebra de linha

  nivel++; // Incrementa o nível
  for (i = 0; i <= p->n; i++)
    printLvlTree(p->p[i], nivel); // Recursivamente imprime os filhos
}

// Função para imprimir a árvore B
void printTree(TApontador p) {
  int nivel = 0;
  printLvlTree(p, nivel); // Chama a função de impressão por níveis
}

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

// Função para preencher a árvore B a partir de um arquivo binário
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

// Função auxiliar para inserir um registro em uma página
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

// Função de inserção de um registro na árvore B
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

int main() {
  // Estrutura Runtime para armazenar o número de comparações e o tempo de
  // execução

  Runtime createTree;
  createTree.totalTime = 0.0;
  createTree.interactions = 0;

  Runtime searchInTree;
  searchInTree.totalTime = 0.0;
  searchInTree.interactions = 0;

  // Ponteiro para a árvore B e ponteiro para registro
  TApontador tree;
  Registro *reg;

  // Variáveis para a entrada do usuário
  int opc = 0;
  int nRegs = 0;
  int typeArq = 0;
  int printOrNot = 0;
  char nameArq[50];

  // Entrada do usuário para o número de registros, tipo de arquivo e opção de
  // impressão
  printf("Entrada 1: número de registros.\n");
  printf("Entrada 2: tipo do arquivo (1 - crescente | 2 - decrescente | 3 - "
         "aleatório).\n");
  printf("Entrada 3: imprimir registro (1 - Sim | 0 - Não).\n");

  scanf("%d %d %d", &nRegs, &typeArq, &printOrNot);

  // Define o nome do arquivo com base no tipo escolhido
  switch (typeArq) {
  case 1:
    strcpy(nameArq, "crescente.bin");
    break;
  case 2:
    strcpy(nameArq, "decrescente.bin");
    break;
  case 3:
    strcpy(nameArq, "aleatorio.bin");
    break;

  default:
    printf("Argumento inválido para o tipo de arquivo...");
    exit(1);
    break;
  }

  // Verificações de entrada do usuário
  if (nRegs <= 0 || nRegs > 2000000) {
    printf(
        "Argumento inválido para quantidade de elementos: 1 <= N <= 2000000");
    exit(1);
  }

  if (printOrNot != 1 && printOrNot != 0) {
    printf("Argumento inválido para a opção de imprimir ou não na tela");
    exit(1);
  }

  printf("-------------------------------------------------------------\n");

  // Criação da Árvore B
  printf("Criando Árvore B...\n");
  // iniando o tempo de execução na crianção da arvore.
  createTree.startToCreateTre = clock();

  fillTheTree(&tree, nameArq, nRegs, &createTree);

  // finalizando o tempo de execução na criação da arvore.
  createTree.endToCreateTre = clock();

  // fazendo o calculo em segundos do total de tempo de execução
  runtimeCPU(&createTree);
  printf(
      "Houve %d comparações na construção da Árvore B na memória principal\n",
      createTree.interactions);
  printf("Foi levado %fs para montar a árvore\n", createTree.totalTime);
  printf("\n");

  int k;

  printf("-------------------------------------------------------------\n");

  // Menu principal
  do {
    printf("Digite:\n  0 - Pesquisar\n  1 - Sair\n  2 - Imprimir a árvore\n");
    scanf("%d", &opc);

    switch (opc) {
    case 0:
      // Pesquisar na árvore
      printf("Digite o valor da chave para pesquisar: ");
      scanf("%d", &k);
      printf("\n");

      // iniando o tempo de execução de uma pesuisa na arvore.
      searchInTree.startToCreateTre = clock();

      search(&reg, tree, k, &searchInTree);

      // finalizando o tempo de execução de uma pesuisa na arvore.

      searchInTree.endToCreateTre = clock();

      // calculando tempo total de uma pesquisa em segundos.

      runtimeCPU(&searchInTree);

      printf("-------------------------------------------------------------\n");

      printf(
          "Tempo de execução da CPU em segundos para pesquisar na árvore: %f\n",
          searchInTree.totalTime);
      printf(
          "Houve %d comparações na pesquisa da Árvore B na memória principal\n",
          searchInTree.interactions);
      searchInTree.interactions = 0;

      // Imprimir registro se a opção estiver ativada
      if (printOrNot == 1) {
        if (reg != NULL) {
          printf("Key: [%d] - %ld | %s\n", reg->key, reg->cod, reg->word);
        } else {
          printf("O registro não está presente na árvore\n");
        }
      }

      printf("-------------------------------------------------------------\n");
      break;

    case 2:
      // Imprimir a árvore por nível
      printf("Imprimindo a árvore:\n");
      printTree(tree);
      break;

    case 1:
      // Sair do programa
      printf("Saindo...\n");
      freeTree(tree); // Libera a memória da árvore antes de sair
      break;

    default:
      printf("Opção errada...\n");
      break;
    }

    printf("\n");
  } while (opc != 1);

  return 0;
}
