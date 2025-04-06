#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int tamanhoArrays[] = {10000,20000,30000,40000,50000,60000,70000,80000,90000,100000};
float arrayTemposA[] = {0,0,0,0,0,0,0,0,0,0};
float arrayTemposB[] = {0,0,0,0,0,0,0,0,0,0};
float arrayTemposC[] = {0,0,0,0,0,0,0,0,0,0};
float arrayTemposD[] = {0,0,0,0,0,0,0,0,0,0};

//Credits : "https://www.geeksforgeeks.org/introduction-to-binary-tree/"
//          "https://unix.stackexchange.com/questions/88659/how-to-plot-multiple-graphs-in-one-plot"
//          "https://weinman.cs.grinnell.edu/courses/CSC213/2008F/labs/10-pingpong-regression.pdf"

void salvarDados(int n) {
    FILE *arquivoTempos = fopen("arvoreBinaria.txt", "w");
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
    

    fprintf(gnuplot, "set title 'Tempo de Execucao da Arvore Binaria'\n");
    fprintf(gnuplot, "set xlabel 'Tamanhos'\n");
    fprintf(gnuplot, "set ylabel 'Tempo (s)'\n");
    fprintf(gnuplot, "set key left top\n");

    fprintf(gnuplot, "set yrange [0:*]\n");
    fprintf(gnuplot, "set xrange [%d:%d]\n", tamanhoArrays[0], maximoX);
    fprintf(gnuplot, "set xtics 0,10000,%d\n", tamanhoArrays[9]);
    fprintf(gnuplot, "regressaoA(x) = a1 + b1 * x\n");
    fprintf(gnuplot, "regressaoB(x) = a2 + b2 * x\n");
    fprintf(gnuplot, "regressaoC(x) = a3 + b3 * x\n");
    fprintf(gnuplot, "regressaoD(x) = a4 + b4 * x\n");

    fprintf(gnuplot, "a1 = 1; b1 = 1\n");
    fprintf(gnuplot, "a2 = 1; b2 = 1\n");
    fprintf(gnuplot, "a3 = 1; b3 = 1\n");
    fprintf(gnuplot, "a4 = 1; b4 = 1\n");

    fprintf(gnuplot, "fit regressaoA(x) 'arvoreBinaria.txt' using 1:2 via a1, b1\n");
    fprintf(gnuplot, "fit regressaoB(x) 'arvoreBinaria.txt' using 1:3 via a2, b2\n");
    fprintf(gnuplot, "fit regressaoC(x) 'arvoreBinaria.txt' using 1:4 via a3, b3\n");
    fprintf(gnuplot, "fit regressaoD(x) 'arvoreBinaria.txt' using 1:5 via a4, b4\n");

    fprintf(gnuplot, "plot 'arvoreBinaria.txt' using 1:2 with points title 'arrayA', regressaoA(x) with lines title 'Regressao Log(n) A', \\\n");
    fprintf(gnuplot, "     'arvoreBinaria.txt' using 1:3 with points title 'arrayB', regressaoB(x) with lines title 'Regressao Log(n) B', \\\n");
    fprintf(gnuplot, "     'arvoreBinaria.txt' using 1:4 with points title 'arrayC', regressaoC(x) with lines title 'Regressao Log(n) C', \\\n");
    fprintf(gnuplot, "     'arvoreBinaria.txt' using 1:5 with points title 'arrayD', regressaoD(x) with lines title 'Regressao Log(n) D'\n");

    pclose(gnuplot);
}

void shuffle(int *array, int size) { //função para misturar os valores dos arrays
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}


struct Node { //estrutura de um nó da árvore
    int data;
    struct Node *left;
    struct Node *right;
};

struct Node *createNode(int data) { //cria um nó da árvore e atribui os valores
    struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
    temp->data = data;
    temp->left = temp->right = NULL;

    return temp;
}

struct Node* insert(struct Node* root, int key, int tamanho) {
    if (root == NULL) return createNode(key); //se não houver raiz coloca o novo nó na raiz

    // cria uma lista para ir colocando os nós que vão ser alterados
    struct Node* queue[tamanho];
    int front = 0, rear = 0;
    queue[rear++] = root; //coloca a raiz no início

    while (front < rear) { //enquanto não chegarmos a valores intermédios
        struct Node* temp = queue[front++]; //vai buscar o nó no início da queue de nós

        if(key != temp->data){
            // se não existe nenhum nó à esqueda do atual
            if (temp->left == NULL) {
                temp->left = createNode(key); //cria um novo nó e adiciona
                break;
            } else {
                if(key != temp->left->data){
                    queue[rear++] = temp->left; //senão mete o nó à esquerda para ser alterado no fim da queue
                } else {
                    break;
                }
            }

            // se não existe nenhum nó à direita do atual
            if (temp->right == NULL) {
                temp->right = createNode(key); //cria um novo nó e adiciona
                break;
            } else {
                if(key != temp->right->data){
                    queue[rear++] = temp->right; //senão mete o nó à esquerda para ser alterado no fim da queue
                } else {
                    break;
                }
            }
        } else {
            break;
        }
    }
    return root;
}

void printTree(struct Node *root, int tamanho) {
    if (root == NULL) return;

    int capacity = tamanho * 2;  // para não haver problemas de espaço
    struct Node **queue = (struct Node**)malloc(capacity * sizeof(struct Node*));
    if (!queue) {
        printf("Erro ao alocar memória para a fila!\n");
        return;
    }

    int front = 0, rear = 0;
    queue[rear++] = root;

    while (front < rear) {
        struct Node *node = queue[front++];

        if (node) {
            printf("%d ", node->data);

            // Verifica se a fila está cheia antes de adicionar novos nós
            if (rear >= capacity) {
                printf("\nErro: Fila cheia, aumentando capacidade...\n");
                capacity *= 2;
                queue = realloc(queue, capacity * sizeof(struct Node*));
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
    struct Node *root;
    float segundos;
    int *arrayA;
    int *arrayB;
    int *arrayC;
    int *arrayD;

    clock_t start, end;

    srand(time(NULL)); 

    for(int i = 0; i < (int)(sizeof(tamanhoArrays) / sizeof(tamanhoArrays[0])); i++){
        root = (struct Node *)malloc(sizeof(struct Node) + 1);
        tamanho = tamanhoArrays[i]; //obtém o tamanho do array

        arrayA = malloc(tamanho * sizeof(int));
        arrayA[0] = 0;

        for(int l = 1; l < tamanho; l++){
            if(rand() % 100 + 1 <= 10){
                arrayA[l] = arrayA[l - 1];
            } else {
                arrayA[l] = arrayA[l - 1] + 1; //adiciona valores ao array
            }
        }

        root->data = arrayA[0]; //inicializa a raiz

        start = clock();
        for(int l = 1; l < tamanho; l++){
            insert(root, arrayA[l], tamanho); //insere os valores do array
        }
        end = clock();

        printf("\n");
        printTree(root, tamanho);

        segundos = (float)(end-start) / CLOCKS_PER_SEC;

        printf("\nTempo A para %d valores-> %.6f segundos\n", tamanho, segundos);        
        arrayTemposA[i] = segundos;

        free(arrayA);
        free(root);

        //--------------------------------------------------------------

        root = (struct Node *)malloc(sizeof(struct Node) + 1);
        arrayB = malloc(tamanho * sizeof(int));
        arrayB[0] = tamanho;

        for(int l = 1; l < tamanho; l++){
            if(rand() % 100 + 1 <= 10){
                arrayB[l] = arrayB[l - 1];
            } else {
                arrayB[l] = arrayB[l - 1] - 1;
            }
        }

        root->data = arrayB[0];

        start = clock();
        for(int l = 1; l < tamanho; l++){
            insert(root, arrayB[l], tamanho);
        }
        end = clock();

        printf("\n");
        printTree(root, tamanho);

        segundos = (float)(end-start) / CLOCKS_PER_SEC;

        printf("\nTempo B para %d valores-> %.6f segundos\n", tamanho, segundos);        
        arrayTemposB[i] = segundos;

        free(arrayB);
        free(root);

        //------------------------------------------------------------

        root = (struct Node *)malloc(sizeof(struct Node) + 1);
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

        root->data = arrayC[0];

        start = clock();
        for(int l = 1; l < tamanho; l++){
            insert(root, arrayC[l], tamanho);
        }
        end = clock();

        printf("\n");
        printTree(root, tamanho);

        segundos = (float)(end-start) / CLOCKS_PER_SEC;

        printf("\nTempo C para %d valores-> %.6f segundos\n", tamanho, segundos);        
        arrayTemposC[i] = segundos;

        free(arrayC);
        free(root);
        
        //------------------------------------------------------------

        root = (struct Node *)malloc(sizeof(struct Node) + 1);
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

        root->data = arrayD[0];

        start = clock();
        for(int i = 1; i < tamanho; i++){
            insert(root, arrayD[i], tamanho);
        }
        end = clock();

        printf("\n");
        printTree(root, tamanho);

        segundos = (float)(end-start) / CLOCKS_PER_SEC;

        printf("\nTempo D para %d valores-> %.6f segundos\n", tamanho, segundos);        
        arrayTemposD[i] = segundos;

        free(arrayD);
        free(root);
        
    }
    printf("\n");

    salvarDados(10);
    gerarGrafico();

    printf("\nTamanho - Tempo A - Tempo B - Tempo C - Tempo D\n");
    for(int j = 0; j < 10; j++){
        printf("%d %.6f %.6f %.6f %.6f\n", tamanhoArrays[j], arrayTemposA[j], arrayTemposB[j], arrayTemposC[j], arrayTemposD[j]);
    }

    return 0;
}