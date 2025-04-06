#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int tamanhoArrays[] = {100000,200000,300000,400000,500000,600000,700000,800000,900000,1000000};
float arrayTemposA[] = {0,0,0,0,0,0,0,0,0,0};
float arrayTemposB[] = {0,0,0,0,0,0,0,0,0,0};
float arrayTemposC[] = {0,0,0,0,0,0,0,0,0,0};
float arrayTemposD[] = {0,0,0,0,0,0,0,0,0,0};
int arrayRotacoesA[] = {0,0,0,0,0,0,0,0,0,0};
int arrayRotacoesB[] = {0,0,0,0,0,0,0,0,0,0};
int arrayRotacoesC[] = {0,0,0,0,0,0,0,0,0,0};
int arrayRotacoesD[] = {0,0,0,0,0,0,0,0,0,0};

//CREDITS: "AI" 

void salvarDados(int n) {
    FILE *arquivoTempos = fopen("arvoreTreap.txt", "w");
    if (!arquivoTempos) {
        printf("Erro ao abrir o arquivo!\n");
        return;
    }
    
    for (int i = 0; i < n; i++) {
        fprintf(arquivoTempos, "%d %.6f %.6f %.6f %.6f\n", tamanhoArrays[i], arrayTemposA[i], arrayTemposB[i], arrayTemposC[i], arrayTemposD[i]);
    }
    
    fclose(arquivoTempos);
}

void gerarGrafico() {
    FILE *gnuplot = popen("gnuplot -persistent", "w");
    if (!gnuplot) {
        printf("Erro ao abrir o GNUplot!\n");
        return;
    }

    int maximoX = tamanhoArrays[9] + 10000;
    

    fprintf(gnuplot, "set title 'Tempo de Execucao da Arvore Treap'\n");
    fprintf(gnuplot, "set xlabel 'Tamanhos'\n");
    fprintf(gnuplot, "set ylabel 'Tempo (s)'\n");
    fprintf(gnuplot, "set key left top\n");

    fprintf(gnuplot, "set yrange [0:*]\n");  // Ajusta automaticamente o eixo Y
    fprintf(gnuplot, "set xrange [%d:%d]\n", tamanhoArrays[0], maximoX);
    fprintf(gnuplot, "set xtics 0,100000,%d\n", tamanhoArrays[9]);
    fprintf(gnuplot, "regressaoA(x) = a1 + b1 * log10(x)\n");
    fprintf(gnuplot, "regressaoB(x) = a2 + b2 * log10(x)\n");
    fprintf(gnuplot, "regressaoC(x) = a3 + b3 * log10(x)\n");
    fprintf(gnuplot, "regressaoD(x) = a4 + b4 * log10(x)\n");

    fprintf(gnuplot, "a1 = 1; b1 = 0.1\n");
    fprintf(gnuplot, "a2 = 1; b2 = 0.1\n");
    fprintf(gnuplot, "a3 = 1; b3 = 0.1\n");
    fprintf(gnuplot, "a4 = 1; b4 = 0.1\n");

    fprintf(gnuplot, "fit regressaoA(x) 'arvoreTreap.txt' using 1:2 via a1, b1\n");
    fprintf(gnuplot, "fit regressaoB(x) 'arvoreTreap.txt' using 1:3 via a2, b2\n");
    fprintf(gnuplot, "fit regressaoC(x) 'arvoreTreap.txt' using 1:4 via a3, b3\n");
    fprintf(gnuplot, "fit regressaoD(x) 'arvoreTreap.txt' using 1:5 via a4, b4\n");

    fprintf(gnuplot, "plot 'arvoreTreap.txt' using 1:2 with points title 'arrayA', regressaoA(x) with lines title 'Regressao Log(n) A', \\\n");
    fprintf(gnuplot, "     'arvoreTreap.txt' using 1:3 with points title 'arrayB', regressaoB(x) with lines title 'Regressao Log(n) B', \\\n");
    fprintf(gnuplot, "     'arvoreTreap.txt' using 1:4 with points title 'arrayC', regressaoC(x) with lines title 'Regressao Log(n) C', \\\n");
    fprintf(gnuplot, "     'arvoreTreap.txt' using 1:5 with points title 'arrayD', regressaoD(x) with lines title 'Regressao Log(n) D'\n");

    pclose(gnuplot);
}

void shuffle(int *array, int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}


typedef struct TreapNode {
    int key;             // Chave para ordenação da árvore binária de busca
    int priority;        // Prioridade aleatória para manter a propriedade heap
    struct TreapNode *left, *right;
} TreapNode;

// Cria um novo nó da Treap com chave e prioridade aleatória
TreapNode* newNode(int key, int priority) {
    TreapNode *node = (TreapNode *) malloc(sizeof(TreapNode));
    if (!node) {
        fprintf(stderr, "Erro na alocação de memória.\n");
        exit(EXIT_FAILURE);
    }
    node->key = key;
    node->priority = priority;
    node->left = node->right = NULL;
    return node;
}

// Rotação à direita
TreapNode* rightRotate(TreapNode *y, int *rotacoes) {
    TreapNode *x = y->left;
    TreapNode *T2 = x->right;
    
    // Realiza a rotação
    x->right = y;
    y->left = T2;
    
    (*rotacoes)++;

    return x;
}

// Rotação à esquerda
TreapNode* leftRotate(TreapNode *x, int *rotacoes) {
    TreapNode *y = x->right;
    TreapNode *T2 = y->left;
    
    // Realiza a rotação
    y->left = x;
    x->right = T2;

    (*rotacoes)++;
    
    return y;
}

// Insere um novo nó na Treap e garante a propriedade heap
TreapNode* treapInsert(TreapNode *root, int key, int priority, int *rotacoes) {
    if (root == NULL)
        return newNode(key, priority);
    
    if (key < root->key) {
        root->left = treapInsert(root->left, key, priority, rotacoes);
        if (root->left && root->left->priority > root->priority)
            root = rightRotate(root, rotacoes);
    } else if (key > root->key) {
        root->right = treapInsert(root->right, key, priority, rotacoes);
        if (root->right && root->right->priority > root->priority)
            root = leftRotate(root, rotacoes);
    }
    return root;
}


void printTree(TreapNode *root, int tamanho) {
    if (root == NULL) return;

    int capacity = tamanho * 2;  // Garante espaço suficiente na fila
    TreapNode **queue = (TreapNode**)malloc(capacity * sizeof(TreapNode*));
    if (!queue) {
        printf("Erro ao alocar memória para a fila!\n");
        return;
    }

    int front = 0, rear = 0;
    queue[rear++] = root;

    while (front < rear) {
        TreapNode *node = queue[front++];

        if (node) {
            printf("%d - (%d) ", node->key, node->priority);

            // Verifica se a fila está cheia antes de adicionar novos nós
            if (rear >= capacity) {
                printf("\nErro: Fila cheia, aumentando capacidade...\n");
                capacity *= 2;
                queue = realloc(queue, capacity * sizeof(TreapNode*));
                if (!queue) {
                    printf("Erro ao realocar memória para a fila!\n");
                    return;
                }
            }

            if (node->left) queue[rear++] = node->left;
            if (node->right) queue[rear++] = node->right;
        }
    }
    
    free(queue); // Liberta a memória alocada para a fila
}

int main() {
    int tamanho;
    int rotacoes = 0;

    clock_t start, end;
    float segundos;

    int *arrayA;
    int *arrayB;
    int *arrayC;
    int *arrayD;

    TreapNode *root;

    srand(time(NULL)); 

    for (int i = 0; i < (int)((sizeof(tamanhoArrays) / sizeof(tamanhoArrays[0]))); i++) {
        tamanho = tamanhoArrays[i];

        root = NULL;
        arrayA = malloc(tamanho * sizeof(int));
        arrayA[0] = 0;

        for (int l = 1; l < tamanho; l++) {
            if (rand() % 100 + 1 <= 10) {
                arrayA[l] = arrayA[l - 1];
            } else {
                arrayA[l] = arrayA[l - 1] + 1;
            }
        }

        start = clock();
        for (int l = 0; l < tamanho; l++) {
            int prioridade = rand() % 1000001;
            root = treapInsert(root, arrayA[l], prioridade, &rotacoes);
        }
        end = clock();

        printf("\n");
        printTree(root, tamanho);

        segundos = (float)(end - start) / CLOCKS_PER_SEC;

        printf("\nTempo A para %d valores -> %.6f segundos\n", tamanho, segundos);        
        arrayTemposA[i] = segundos;
        arrayRotacoesA[i] = rotacoes;

        free(arrayA);
        free(root);
        rotacoes = 0;

        //-----------------------------------------------------------------------------
        root = NULL;
        arrayB = malloc(tamanho * sizeof(int));
        arrayB[0] = tamanho;

        for (int l = 1; l < tamanho; l++) {
            if (rand() % 100 + 1 <= 10) {
                arrayB[l] = arrayB[l - 1];
            } else {
                arrayB[l] = arrayB[l - 1] - 1;
            }
        }

        start = clock();
        for (int l = 0; l < tamanho; l++) {
            int prioridade = rand() % 1000001;
            root = treapInsert(root, arrayB[l], prioridade, &rotacoes);
        }
        end = clock();

        printf("\n");
        printTree(root, tamanho);

        segundos = (float)(end - start) / CLOCKS_PER_SEC;

        printf("\nTempo B para %d valores -> %.6f segundos\n", tamanho, segundos);        
        arrayTemposB[i] = segundos;
        arrayRotacoesB[i] = rotacoes;

        free(arrayB);
        free(root);
        rotacoes = 0;

        //-----------------------------------------------------------------------------
        root = NULL;
        arrayC = malloc(tamanho * sizeof(int));
        arrayC[0] = 0;

        for (int l = 1; l < tamanho; l++) {
            if (rand() % 100 + 1 <= 10) {
                arrayC[l] = arrayC[l - 1];
            } else {
                arrayC[l] = arrayC[l - 1] + 1;
            }
        }

        shuffle(arrayC, tamanho);

        start = clock();
        for (int l = 0; l < tamanho; l++) {
            int prioridade = rand() % 1000001;
            root = treapInsert(root, arrayC[l], prioridade, &rotacoes);
        }
        end = clock();

        printf("\n");
        printTree(root, tamanho);

        segundos = (float)(end - start) / CLOCKS_PER_SEC;

        printf("\nTempo C para %d valores -> %.6f segundos\n", tamanho, segundos);        
        arrayTemposC[i] = segundos;
        arrayRotacoesC[i] = rotacoes;

        free(arrayC);
        free(root);
        rotacoes = 0;

        //-----------------------------------------------------------------------------
        root = NULL;
        arrayD = malloc(tamanho * sizeof(int));
        arrayD[0] = 0;

        for (int l = 1; l < tamanho; l++) {
            if (rand() % 100 + 1 <= 90) {
                arrayD[l] = arrayD[l - 1];
            } else {
                arrayD[l] = arrayD[l - 1] + 1;
            }
        }

        shuffle(arrayD, tamanho);

        start = clock();
        for (int l = 0; l < tamanho; l++) {
            int prioridade = rand() % 1000001;
            root = treapInsert(root, arrayD[l], prioridade, &rotacoes);
        }
        end = clock();

        printf("\n");
        printTree(root, tamanho);

        segundos = (float)(end - start) / CLOCKS_PER_SEC;

        printf("\nTempo D para %d valores -> %.6f segundos\n", tamanho, segundos);        
        arrayTemposD[i] = segundos;
        arrayRotacoesD[i] = rotacoes;

        free(arrayD);
        free(root);
        rotacoes = 0;
    }

    printf("\n");

    salvarDados(10);
    gerarGrafico();

    printf("\nTamanho - Tempo A - Tempo B - Tempo C - Tempo D\n");
    for (int j = 0; j < 10; j++) {
        printf("%d %.6f %.6f %.6f %.6f\n", tamanhoArrays[j], arrayTemposA[j], arrayTemposB[j], arrayTemposC[j], arrayTemposD[j]);
    }

    printf("\nTamanho - Rotações A - Rotações B - Rotações C - Rotações D\n");
    for (int j = 0; j < 10; j++) {
        printf("%d %d %d %d %d\n", tamanhoArrays[j], arrayRotacoesA[j], arrayRotacoesB[j], arrayRotacoesC[j], arrayRotacoesD[j]);
    }

    return 0;
}
