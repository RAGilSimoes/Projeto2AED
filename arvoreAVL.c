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

//CREDITS: "https://www.geeksforgeeks.org/insertion-in-an-avl-tree/"

void salvarDados(int n) {
    FILE *arquivoTempos = fopen("arvoreAVL.txt", "w");
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
    

    fprintf(gnuplot, "set title 'Tempo de Execucao da Arvore AVL'\n");
    fprintf(gnuplot, "set xlabel 'Tamanhos'\n");
    fprintf(gnuplot, "set ylabel 'Tempo (s)'\n");
    fprintf(gnuplot, "set key left top\n");

    fprintf(gnuplot, "set yrange [0:*]\n");
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

    fprintf(gnuplot, "fit regressaoA(x) 'arvoreAVL.txt' using 1:2 via a1, b1\n");
    fprintf(gnuplot, "fit regressaoB(x) 'arvoreAVL.txt' using 1:3 via a2, b2\n");
    fprintf(gnuplot, "fit regressaoC(x) 'arvoreAVL.txt' using 1:4 via a3, b3\n");
    fprintf(gnuplot, "fit regressaoD(x) 'arvoreAVL.txt' using 1:5 via a4, b4\n");

    fprintf(gnuplot, "plot 'arvoreAVL.txt' using 1:2 with points title 'arrayA', regressaoA(x) with lines title 'Regressao Log(n) A', \\\n");
    fprintf(gnuplot, "     'arvoreAVL.txt' using 1:3 with points title 'arrayB', regressaoB(x) with lines title 'Regressao Log(n) B', \\\n");
    fprintf(gnuplot, "     'arvoreAVL.txt' using 1:4 with points title 'arrayC', regressaoC(x) with lines title 'Regressao Log(n) C', \\\n");
    fprintf(gnuplot, "     'arvoreAVL.txt' using 1:5 with points title 'arrayD', regressaoD(x) with lines title 'Regressao Log(n) D'\n");

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

// nó de árvore
struct node
{
    int key;
    struct node *left;
    struct node *right;
    int height;
};

// função para obter altura do nó
int height(struct node *N)
{
    if (N == NULL)
        return 0;
    return N->height;
}

// função para obter o máximo entre 2 valores
int max(int a, int b)
{
    return (a > b)? a : b;
}

//inicializar nó
struct node* newNode(int key)
{
    struct node* node = (struct node*)
                        malloc(sizeof(struct node));
    node->key   = key;
    node->left   = NULL;
    node->right  = NULL;
    node->height = 1; 
    return(node);
}

//função para fazer rotação à direita
struct node *rightRotate(struct node *y, int *rotacoes)
{
    if (y == NULL || y->left == NULL) {
        // Retorna o nó original se y ou y->left for NULL
        return y;
    }

    struct node *x = y->left;
    struct node *T2 = x->right;

    // faz rotação
    x->right = y;
    y->left = T2;

    // altera altura
    y->height = max(height(y->left),
                    height(y->right)) + 1;
    x->height = max(height(x->left),
                    height(x->right)) + 1;

    // devolve a nova raiz
    (*rotacoes)++;
    return x;
}

//função para fazer rotação à esquerda
struct node *leftRotate(struct node *x, int *rotacoes)
{
    if (x == NULL || x->right == NULL) {
        // Retorna o nó original se x ou x->right for NULL
        return x;
    }

    struct node *y = x->right;
    struct node *T2 = y->left;

    // faz rotação
    y->left = x;
    x->right = T2;

    // obtém alturas
    x->height = max(height(x->left),   
                    height(x->right)) + 1;
    y->height = max(height(y->left),
                    height(y->right)) + 1;

    // devolve raiz
    (*rotacoes)++;
    return y;
}


// obtém o fator de equilíbrio do nó
int getBalance(struct node *N)
{
    if (N == NULL)
        return 0;
    return height(N->left) - height(N->right);
}

//função para inserir o nó
struct node* insert(struct node* node, int key, int *rotacoes)
{
    
    //insere o nó
    if (node == NULL)
        return(newNode(key));

    if (key < node->key)
        node->left  = insert(node->left, key, rotacoes);
    else if (key > node->key)
        node->right = insert(node->right, key, rotacoes);
    else
        return node;

    //modifica altura
    node->height = 1 + max(height(node->left),
                        height(node->right));

    //obtém o fator de equilíbrio
    int balance = getBalance(node);

    // Left Left Case
    if (balance > 1 && key < node->left->key)
        return rightRotate(node, rotacoes);

    // Right Right Case
    if (balance < -1 && key > node->right->key)
        return leftRotate(node, rotacoes);

    // Left Right Case
    if (balance > 1 && key > node->left->key)
    {
        node->left =  leftRotate(node->left, rotacoes);
        return rightRotate(node, rotacoes);
    }

    // Right Left Case
    if (balance < -1 && key < node->right->key)
    {
        node->right = rightRotate(node->right, rotacoes);
        return leftRotate(node, rotacoes);
    }

    //devolve o nó
    return node;
}

void printTree(struct node *root, int tamanho) {
    if (root == NULL) return;

    int capacity = tamanho * 2;   // para não haver problemas de espaço
    struct node **queue = (struct node**)malloc(capacity * sizeof(struct node*));
    if (!queue) {
        printf("Erro ao alocar memória para a fila!\n");
        return;
    }

    int front = 0, rear = 0;
    queue[rear++] = root;

    while (front < rear) {
        struct node *node = queue[front++];

        if (node) {
            printf("%d ", node->key);

            // Verifica se a fila está cheia antes de adicionar novos nós
            if (rear >= capacity) {
                printf("\nErro: Fila cheia, aumentando capacidade...\n");
                capacity *= 2;
                queue = realloc(queue, capacity * sizeof(struct node*));
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

int main(){
    int tamanho;
    struct node *root;
    float segundos;
    int *arrayA;
    int *arrayB;
    int *arrayC;
    int *arrayD;
    int rotacoes = 0;

    clock_t start, end;

    srand(time(NULL)); 

    for(int i = 0; i < (int)(sizeof(tamanhoArrays) / sizeof(tamanhoArrays[0])); i++){
        root = (struct node *)malloc(sizeof(struct node));
        tamanho = tamanhoArrays[i]; //obtém o tamanho do array

        arrayA = malloc(tamanho * sizeof(int));
        arrayA[0] = 0; 
        root->key = arrayA[0]; //inicializa a raiz

        for(int l = 1; l < tamanho; l++){
            if(rand() % 100 + 1 <= 10){
                arrayA[l] = arrayA[l - 1];
            } else {
                arrayA[l] = arrayA[l - 1] + 1; //adiciona valores ao array
            }
            printf("%d ", arrayA[l]);
        }

        start = clock();
        for(int l = 1; l < tamanho; l++){
            root = insert(root, arrayA[l], &rotacoes); //insere os valores do array
        }
        end = clock();

        printf("\n");
        printTree(root, tamanho);

        segundos = (float)(end-start) / CLOCKS_PER_SEC;

        printf("\nTempo A para %d valores-> %.10f segundos\n", tamanho, segundos);        
        arrayTemposA[i] = segundos;
        arrayRotacoesA[i] = rotacoes;

        free(arrayA);
        free(root);
        
        rotacoes = 0;

        //--------------------------------------------------------------

        root = (struct node *)malloc(sizeof(struct node));
        arrayB = malloc(tamanho * sizeof(int));
        arrayB[0] = tamanho;

        for(int l = 1; l < tamanho; l++){
            if(rand() % 100 + 1 <= 10){
                arrayB[l] = arrayB[l - 1];
            } else {
                arrayB[l] = arrayB[l - 1] - 1;
            }
        }

        root->key = arrayB[0];

        start = clock();
        for(int l = 1; l < tamanho; l++){
            root = insert(root, arrayB[l], &rotacoes);
        }
        end = clock();

        printf("\n");
        printTree(root, tamanho);

        segundos = (float)(end-start) / CLOCKS_PER_SEC;

        printf("\nTempo B para %d valores-> %.10f segundos\n", tamanho, segundos);        
        arrayTemposB[i] = segundos;
        arrayRotacoesB[i] = rotacoes;

        free(arrayB);
        free(root);
        rotacoes = 0;

        //------------------------------------------------------------

        root = (struct node *)malloc(sizeof(struct node));
        arrayC = malloc(tamanho * sizeof(int));
        arrayC[0] = 0;

        for(int l = 1; l < tamanho; l++){
            if(rand() % 100 + 1 <= 10){
                arrayC[l] = arrayC[l - 1];
            } else {
                arrayC[l] = arrayC[l - 1] + 1;
            }
        }

        shuffle(arrayC, tamanho);

        root->key = arrayC[0];

        start = clock();
        for(int l = 1; l < tamanho; l++){
            root = insert(root, arrayC[l], &rotacoes);
        }
        end = clock();

        printf("\n");
        printTree(root, tamanho);

        segundos = (float)(end-start) / CLOCKS_PER_SEC;

        printf("\nTempo C para %d valores-> %.10f segundos\n", tamanho, segundos);        
        arrayTemposC[i] = segundos;
        arrayRotacoesC[i] = rotacoes;

        free(arrayC);
        free(root);
        rotacoes = 0;
        //------------------------------------------------------------

        root = (struct node *)malloc(sizeof(struct node));
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

        root->key = arrayD[0];

        start = clock();
        for(int l = 1; l < tamanho; l++){
            root = insert(root, arrayD[l], &rotacoes);
        }
        end = clock();

        printf("\n");
        printTree(root, tamanho);

        segundos = (float)(end-start) / CLOCKS_PER_SEC;

        printf("\nTempo D para %d valores-> %.10f segundos\n", tamanho, segundos);        
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
    for(int j = 0; j < 10; j++){
        printf("%d %.6f %.6f %.6f %.6f\n", tamanhoArrays[j], arrayTemposA[j], arrayTemposB[j], arrayTemposC[j], arrayTemposD[j]);
    }

    printf("\nTamanho - Rotações A - Rotações B - Rotações C - Rotações D\n");
    for(int j = 0; j < 10; j++){
        printf("%d %d %d %d %d\n", tamanhoArrays[j], arrayRotacoesA[j], arrayRotacoesB[j], arrayRotacoesC[j], arrayRotacoesD[j]);
    }

    return 0;
}