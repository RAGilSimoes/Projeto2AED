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

//CREDITS: "https://www.geeksforgeeks.org/introduction-to-red-black-tree/"

void salvarDados(int n) {
    FILE *arquivoTempos = fopen("arvoreVP.txt", "w");
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
    

    fprintf(gnuplot, "set title 'Tempo de Execucao da Arvore VP'\n");
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

    fprintf(gnuplot, "fit regressaoA(x) 'arvoreVP.txt' using 1:2 via a1, b1\n");
    fprintf(gnuplot, "fit regressaoB(x) 'arvoreVP.txt' using 1:3 via a2, b2\n");
    fprintf(gnuplot, "fit regressaoC(x) 'arvoreVP.txt' using 1:4 via a3, b3\n");
    fprintf(gnuplot, "fit regressaoD(x) 'arvoreVP.txt' using 1:5 via a4, b4\n");

    fprintf(gnuplot, "plot 'arvoreVP.txt' using 1:2 with points title 'arrayA', regressaoA(x) with lines title 'Regressao Log(n) A', \\\n");
    fprintf(gnuplot, "     'arvoreVP.txt' using 1:3 with points title 'arrayB', regressaoB(x) with lines title 'Regressao Log(n) B', \\\n");
    fprintf(gnuplot, "     'arvoreVP.txt' using 1:4 with points title 'arrayC', regressaoC(x) with lines title 'Regressao Log(n) C', \\\n");
    fprintf(gnuplot, "     'arvoreVP.txt' using 1:5 with points title 'arrayD', regressaoD(x) with lines title 'Regressao Log(n) D'\n");

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

struct node {
    int data; // valor
    int color; // 1-red, 0-black
    struct node* parent; // pai
    struct node* right; 
    struct node* left;
};

struct node* root;

struct node* insert(struct node* trav, struct node* temp) {
    // se raiz estiver vazia adiciona à raiz
    if (trav == NULL)
        return temp;
 
    // percorre a árvore
    if (temp->data < trav->data) 
    {
        trav->left = insert(trav->left, temp);
        trav->left->parent = trav;
    }
    else if (temp->data > trav->data) 
    {
        trav->right = insert(trav->right, temp);
        trav->right->parent = trav;
    }
 
    // devolve o nó
    return trav;
}

//rotação à direita
void rightrotate(struct node* temp, int* rotacoes) {
    struct node* left = temp->left;
    temp->left = left->right;
    if (temp->left)
        temp->left->parent = temp;
    left->parent = temp->parent;
    if (!temp->parent)
        root = left;
    else if (temp == temp->parent->left)
        temp->parent->left = left;
    else
        temp->parent->right = left;
    left->right = temp;
    temp->parent = left;

    (*rotacoes)++;
}

//rotação à esquerda
void leftrotate(struct node* temp, int* rotacoes) {
    struct node* right = temp->right;
    temp->right = right->left;
    if (temp->right)
        temp->right->parent = temp;
    right->parent = temp->parent;
    if (!temp->parent)
        root = right;
    else if (temp == temp->parent->left)
        temp->parent->left = right;
    else
        temp->parent->right = right;
    right->left = temp;
    temp->parent = right;

    (*rotacoes)++;
}

void fixup(struct node* root, struct node* pt, int* rotacoes) {
    struct node* parent_pt = NULL;
    struct node* grand_parent_pt = NULL;
 
    while ((pt != root) && (pt->color != 0) && (pt->parent != NULL) && (pt->parent->color == 1)) 
    {
        parent_pt = pt->parent;
        grand_parent_pt = pt->parent->parent;
 
        /*  Case : A
             Parent of pt is left child 
             of Grand-parent of
           pt */
        if (parent_pt == grand_parent_pt->left) 
        {
 
            struct node* uncle_pt = grand_parent_pt->right;
 
            /* Case : 1
                The uncle of pt is also red
                Only Recoloring required */
            if (uncle_pt != NULL && uncle_pt->color == 1) 
            {
                grand_parent_pt->color = 1;
                parent_pt->color = 0;
                uncle_pt->color = 0;
                pt = grand_parent_pt;
            }
 
            else {
 
                /* Case : 2
                     pt is right child of its parent
                     Left-rotation required */
                if (pt == parent_pt->right) {
                    leftrotate(parent_pt, rotacoes);
                    pt = parent_pt;
                    parent_pt = pt->parent;
                }
 
                /* Case : 3
                     pt is left child of its parent
                     Right-rotation required */
                rightrotate(grand_parent_pt, rotacoes);
                int t = parent_pt->color;
                parent_pt->color = grand_parent_pt->color;
                grand_parent_pt->color = t;
                pt = parent_pt;
            }
        }
 
        /* Case : B
             Parent of pt is right 
             child of Grand-parent of
           pt */
        else {
            struct node* uncle_pt = grand_parent_pt->left;
 
            /*  Case : 1
                The uncle of pt is also red
                Only Recoloring required */
            if ((uncle_pt != NULL) && (uncle_pt->color == 1)) 
            {
                grand_parent_pt->color = 1;
                parent_pt->color = 0;
                uncle_pt->color = 0;
                pt = grand_parent_pt;
            }
            else {
                /* Case : 2
                   pt is left child of its parent
                   Right-rotation required */
                if (pt == parent_pt->left) {
                    rightrotate(parent_pt, rotacoes);
                    pt = parent_pt;
                    parent_pt = pt->parent;
                }
 
                /* Case : 3
                     pt is right child of its parent
                     Left-rotation required */
                leftrotate(grand_parent_pt, rotacoes);
                int t = parent_pt->color;
                parent_pt->color = grand_parent_pt->color;
                grand_parent_pt->color = t;
                pt = parent_pt;
            }
        }
    }
}

void printTree(struct node *root, int tamanho) {
    if (root == NULL) return;

    int capacity = tamanho * 2;  // Garante espaço suficiente na fila
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
            printf("%d-(%d) ", node->data, node->color);

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
    int rotacoes = 0;

    clock_t start, end;

    float segundos;

    int *arrayA;
    int *arrayB;
    int *arrayC;
    int *arrayD;

    srand(time(NULL)); 

    for(int i = 0; i < (int)((sizeof(tamanhoArrays) / sizeof(tamanhoArrays[0]))); i++){
        tamanho = tamanhoArrays[i];

        root = NULL;
        arrayA = malloc(tamanho * sizeof(int));
        arrayA[0] = 0;

        for(int l = 1; l < tamanho; l++){
            if(rand() % 100 + 1 <= 10){
                arrayA[l] = arrayA[l - 1];
            } else {
                arrayA[l] = arrayA[l - 1] + 1;
            }
        }

        start = clock();
        for (int l = 0; l < tamanho; l++) {
            //inicializa nó
            struct node* temp = (struct node*)malloc(sizeof(struct node));
            temp->right = NULL;
            temp->left = NULL;
            temp->parent = NULL;
            temp->data = arrayA[l];
            temp->color = 1;
    
            //insere
            root = insert(root, temp);
    
            //verifica se árvore está correta
            fixup(root, temp, &rotacoes);
            root->color = 0;
        }
        end = clock();

        printf("\n");
        printTree(root, tamanho);

        segundos = (float)(end-start) / CLOCKS_PER_SEC;

        printf("\nTempo A para %d valores-> %.6f segundos\n", tamanho, segundos);        
        arrayTemposA[i] = segundos;
        arrayRotacoesA[i] = rotacoes;

        free(arrayA);
        free(root);
        rotacoes = 0;
        //-----------------------------------------------------------------------------
        root = NULL;
        arrayB = malloc(tamanho * sizeof(int));
        arrayB[0] = tamanho;

        for(int l = 1; l < tamanho; l++){
            if(rand() % 100 + 1 <= 10){
                arrayB[l] = arrayB[l - 1];
            } else {
                arrayB[l] = arrayB[l - 1] - 1;
            }
        }

        start = clock();
        for (int l = 0; l < tamanho; l++) {
            struct node* temp = (struct node*)malloc(sizeof(struct node));
            temp->right = NULL;
            temp->left = NULL;
            temp->parent = NULL;
            temp->data = arrayB[l];
            temp->color = 1;
    
            root = insert(root, temp);
    
            fixup(root, temp, &rotacoes);
            root->color = 0;
        }
        end = clock();
        
        printf("\n");
        printTree(root, tamanho);

        segundos = (float)(end-start) / CLOCKS_PER_SEC;

        printf("\nTempo B para %d valores-> %.6f segundos\n", tamanho, segundos);        
        arrayTemposB[i] = segundos;
        arrayRotacoesB[i] = rotacoes;

        free(arrayB);
        free(root);
        rotacoes = 0;
        //-----------------------------------------------------------------------------
        root = NULL;
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

        start = clock();
        for (int l = 0; l < tamanho; l++) {
            struct node* temp = (struct node*)malloc(sizeof(struct node));
            temp->right = NULL;
            temp->left = NULL;
            temp->parent = NULL;
            temp->data = arrayC[l];
            temp->color = 1;

            root = insert(root, temp);
    
            fixup(root, temp, &rotacoes);
            root->color = 0;
        }
        end = clock();
        
        printf("\n");
        printTree(root, tamanho);

        segundos = (float)(end-start) / CLOCKS_PER_SEC;

        printf("\nTempo C para %d valores-> %.6f segundos\n", tamanho, segundos);        
        arrayTemposC[i] = segundos;
        arrayRotacoesC[i] = rotacoes;

        free(arrayC);
        free(root);
        rotacoes = 0;
        //------------------------------------------------------------------------
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
            struct node* temp = (struct node*)malloc(sizeof(struct node));
            temp->right = NULL;
            temp->left = NULL;
            temp->parent = NULL;
            temp->data = arrayD[l];
            temp->color = 1;
    
            root = insert(root, temp);
    
            fixup(root, temp, &rotacoes);
            root->color = 0;
        }
        end = clock();
        
        printf("\n");
        printTree(root, tamanho);

        segundos = (float)(end-start) / CLOCKS_PER_SEC;

        printf("\nTempo D para %d valores-> %.6f segundos\n", tamanho, segundos);        
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