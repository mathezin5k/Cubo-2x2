#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <string.h>
#include <locale.h>

#define ANSI_BG_WHITE   "\x1b[47m"
#define ANSI_BG_YELLOW  "\x1b[43m"
#define ANSI_BG_RED     "\x1b[41m"
#define ANSI_BG_ORANGE  "\x1b[48;5;208m"
#define ANSI_BG_BLUE    "\x1b[44m"
#define ANSI_BG_GREEN   "\x1b[42m"
#define ANSI_RESET      "\x1b[0m"

#define HASH_SIZE 100003

// ------------------ Estruturas ------------------
typedef struct Node{
    char cube[6][4];
    struct Node* parent;
    char move;
    int depth;
    struct Node* next;
} Node;

typedef struct Queue{
    Node* front;
    Node* rear;
} Queue;

typedef struct HNode {
    char key[25];     // estado do cubo serializado
    int depth;        // custo até o solved
    struct HNode* next;
} HNode;

typedef struct AStarNode {
    char cube[6][4];
    struct AStarNode* parent;
    char move;
    int g,h,f;
    struct AStarNode* next;
} AStarNode;

typedef struct {
    AStarNode* head;
} OpenList;

HNode* hashTable[HASH_SIZE];

void initCube(char cube[6][4]) {
    char colors[] = {'W','Y','R','O','B','G'};
    for(int f=0; f<6; f++){
        for(int i=0; i<4; i++){
            cube[f][i] = colors[f];
        }
    }
}

void rotateFaceClockwise(char face[4]) {
    char temp = face[0];
    face[0] = face[2];
    face[2] = face[3];
    face[3] = face[1];
    face[1] = temp;
}

void moveF(char c[6][4]) {
    char t0=c[0][2], t1=c[0][3];
    c[0][3]=c[4][1]; c[0][2]=c[4][3];
    c[4][1]=c[1][0]; c[4][3]=c[1][1];
    c[1][0]=c[5][2]; c[1][1]=c[5][0];
    c[5][2]=t1; c[5][0]=t0;
    rotateFaceClockwise(c[2]);
}

void moveB(char c[6][4]) {
    char t0=c[0][0], t1=c[0][1];
    c[0][0]=c[5][1]; c[0][1]=c[5][3];
    c[5][1]=c[1][3]; c[5][3]=c[1][2];
    c[1][2]=c[4][0]; c[1][3]=c[4][2];
    c[4][0]=t1; c[4][2]=t0;
    rotateFaceClockwise(c[3]);
}

void moveU(char c[6][4]) {
    char t0=c[4][0], t1=c[4][1];
    c[4][0]=c[2][0]; c[4][1]=c[2][1];
    c[2][0]=c[5][0]; c[2][1]=c[5][1];
    c[5][0]=c[3][0]; c[5][1]=c[3][1];
    c[3][0]=t0; c[3][1]=t1;
    rotateFaceClockwise(c[0]);
}

void moveD(char c[6][4]) {
    char t0=c[4][2], t1=c[4][3];
    c[4][2]=c[3][2]; c[4][3]=c[3][3];
    c[3][2]=c[5][2]; c[3][3]=c[5][3];
    c[5][2]=c[2][2]; c[5][3]=c[2][3];
    c[2][2]=t0; c[2][3]=t1;
    rotateFaceClockwise(c[1]);
}

void moveL(char c[6][4]) {
    char t0=c[0][0], t1=c[0][2];
    c[0][2]=c[3][1]; c[0][0]=c[3][3];
    c[3][1]=c[1][2]; c[3][3]=c[1][0];
    c[1][0]=c[2][0]; c[1][2]=c[2][2];
    c[2][0]=t0; c[2][2]=t1;
    rotateFaceClockwise(c[4]);
}

void moveR(char c[6][4]) {
    char t0=c[0][1], t1=c[0][3];
    c[0][1]=c[2][1]; c[0][3]=c[2][3];
    c[2][1]=c[1][1]; c[2][3]=c[1][3];
    c[1][3]=c[3][0]; c[1][1]=c[3][2];
    c[3][0]=t1; c[3][2]=t0;
    rotateFaceClockwise(c[5]);
}

void printColorBlock(char color) {
    switch(color){
        case 'W': printf(ANSI_BG_WHITE "  " ANSI_RESET); break;
        case 'Y': printf(ANSI_BG_YELLOW "  " ANSI_RESET); break;
        case 'R': printf(ANSI_BG_RED "  " ANSI_RESET); break;
        case 'O': printf(ANSI_BG_ORANGE "  " ANSI_RESET); break;
        case 'B': printf(ANSI_BG_BLUE "  " ANSI_RESET); break;
        case 'G': printf(ANSI_BG_GREEN "  " ANSI_RESET); break;
        default: printf("  ");
    }
}

void printCubeVisualColored(char cube[6][4]) {
    printf("      UP\n     ");
    printColorBlock(cube[0][0]); printColorBlock(cube[0][1]); printf("\n     ");
    printColorBlock(cube[0][2]); printColorBlock(cube[0][3]); printf("\n\n");

    printf("LEFT  FRONT  RIGHT  BACK\n");
    for(int row=0; row<2; row++){
        printf(" "); printColorBlock(cube[4][row*2]); printColorBlock(cube[4][row*2+1]); printf("   ");
        printColorBlock(cube[2][row*2]); printColorBlock(cube[2][row*2+1]); printf("   ");
        printColorBlock(cube[5][row*2]); printColorBlock(cube[5][row*2+1]); printf("   ");
        printColorBlock(cube[3][row*2]); printColorBlock(cube[3][row*2+1]); printf("\n");
    }

    printf("\n      DOWN\n     ");
    printColorBlock(cube[1][0]); printColorBlock(cube[1][1]); printf("\n     ");
    printColorBlock(cube[1][2]); printColorBlock(cube[1][3]); printf("\n\n");
}

void randCube(char cube[6][4]){
    for(int i=0; i<40; i++){
        int move = rand()%3;
        switch(move){
            case 0: moveF(cube); break;
            case 1: moveU(cube); break;
            case 2: moveR(cube); break;
        }
    }
}

// ------------------ Queue ------------------
void initQueue(Queue* q){
    q->front=NULL; q->rear=NULL;
}

int isEmpty(Queue* q){
    return q->front==NULL;
}

void enqueue(Queue* q, Node* n){
    n->next=NULL;
    if(q->rear==NULL){
        q->front=q->rear=n;
    }else { q->rear->next=n; q->rear=n; }
}

Node* dequeue(Queue* q){
    if(isEmpty(q)) return NULL;
    Node* n=q->front;
    q->front=n->next;
    if(q->front==NULL) q->rear=NULL;
    return n;
}

void copyCube(char dest[6][4], char src[6][4]){
    for(int f=0; f<6; f++){
        for(int i=0; i<4; i++){
            dest[f][i]=src[f][i];
        }
    }
}
int isCompletedCube(char c[6][4]){
    for(int f=0; f<6; f++){
        char col=c[f][0];
        for(int i=1;i<4;i++){
            if(c[f][i]!=col) return 0;
        }
    }
    return 1;
}

void searchSolver(char start[6][4], int maxDepth, int isDFS){
    Queue q;
    initQueue(&q);
    Node* root = (Node*)malloc(sizeof(Node));
    copyCube(root->cube,start);
    root->parent=NULL;
    root->move=' ';
    root->depth=0;
    root->next=NULL;
    enqueue(&q, root);

    void (*funcs[3])(char[6][4]) = {moveF, moveU, moveR};
    char moves[3] = {'F','U','R'};

    int statesVisited = 0;

    while(!isEmpty(&q)){
        Node* cur;

        if(isDFS){ // comportamento de pilha
            Node* prev = NULL;
            cur = q.front;
            while(cur->next != NULL){ prev = cur; cur = cur->next; }
            if(prev != NULL) prev->next = NULL;
            else q.front = NULL;
            if(cur == q.rear) q.rear = prev;
        }else{
            cur = dequeue(&q);
        }

        statesVisited++;

        if(isCompletedCube(cur->cube)){
            printf("Cubo inicial embaralhado:\n"); printCubeVisualColored(start);
            printf("Quantidade de movimentos realizados: %d\n", cur->depth);
            printf("Estados visitados: %d\n", statesVisited);

            if(cur->depth>0){
                Node* path[cur->depth];
                Node* tmp = cur;
                for(int i=cur->depth-1;i>=0;i--){
                    path[i]=tmp; tmp=tmp->parent;
                }

                printf("Movimentos realizados: ");
                int count=1;
                for(int i=0;i<cur->depth;i++){
                    if(i<cur->depth-1 && path[i]->move==path[i+1]->move){
                        count++;
                    } else {
                        if(count==3) printf("%c' ", path[i]->move);
                        else for(int j=0;j<count;j++) printf("%c ", path[i]->move);
                        count=1;
                    }
                }
                printf("\n");
            }

            printf("\nCubo final:\n");
            printCubeVisualColored(cur->cube);

            while(!isEmpty(&q)) free(dequeue(&q));
            free(cur);
            return;
        }

        if(cur->depth<maxDepth){
            for(int i=0;i<3;i++){ // apenas F, U, R
                Node* child = (Node*)malloc(sizeof(Node));
                copyCube(child->cube,cur->cube);
                funcs[i](child->cube);
                child->parent=cur;
                child->move=moves[i];
                child->depth=cur->depth+1;
                child->next=NULL;
                enqueue(&q,child);
            }
        }

        if(cur->parent==NULL) free(cur);
    }

    printf("Nenhuma solução encontrada até profundidade %d\n", maxDepth);
    printf("Estados visitados: %d\n", statesVisited);
}

void playInteractive(char cube[6][4]){
    int playing=1,movs=0;
    do{
        system("cls"); printCubeVisualColored(cube);
        printf("Comandos: F, U, R, L, B, D, ESC (para sair)\n");
        char c = getch();
        switch(c){
            case 27: playing = 0; break;
            case 70: case 102: moveF(cube); playing = !isCompletedCube(cube); break;
            case 85: case 117: moveU(cube); playing = !isCompletedCube(cube); break;
            case 66: case 98:  moveB(cube); playing = !isCompletedCube(cube); break;
            case 82: case 114: moveR(cube); playing = !isCompletedCube(cube); break;
            case 76: case 108: moveL(cube); playing = !isCompletedCube(cube); break;
            case 68: case 100: moveD(cube); playing = !isCompletedCube(cube); break;
            default: movs--; break;
        }
        movs++;
    }while(playing);

    if (isCompletedCube(cube)) {
            system("cls"); printCubeVisualColored(cube);
            printf("\nCompleto em %d movimentos!\n", movs);
    }
}

// ------------------ Heurística ------------------

void cubeToString(char cube[6][4], char* str){
    int k=0;
    for(int f=0;f<6;f++){
        for(int i=0;i<4;i++) str[k++]=cube[f][i];
    }
    str[k]='\0';
}
void cubeFromString(char* str, char cube[6][4]){
    int k=0;
    for(int f=0;f<6;f++){
        for(int i=0;i<4;i++) cube[f][i]=str[k++];
    }
}

unsigned long hashKey(const char* str){
    unsigned long h=5381; int c;
    while((c=*str++)) h=((h<<5)+h)+c;
    return h;
}

void insertHash(char* key,int depth){
    unsigned long h=hashKey(key)%HASH_SIZE;
    HNode* n=(HNode*)malloc(sizeof(HNode));
    strcpy(n->key,key);
    n->depth=depth;
    n->next=hashTable[h];
    hashTable[h]=n;
}
int findHash(char* key){
    unsigned long h=hashKey(key)%HASH_SIZE;
    HNode* cur=hashTable[h];
    while(cur){
        if(strcmp(cur->key,key)==0) return cur->depth;
        cur=cur->next;
    }
    return -1;
}
int hashTableVazia(){
    for(int i=0;i<HASH_SIZE;i++) if(hashTable[i]!=NULL) return 0;
    return 1;
}

void saveHashToFile(const char* filename){
    FILE* f=fopen(filename,"wb");
    if(!f){ printf("Erro ao salvar hash!\n"); return; }
    for(int i=0;i<HASH_SIZE;i++){
        HNode* cur=hashTable[i];
        while(cur){
            fwrite(cur->key,sizeof(char),25,f);
            fwrite(&cur->depth,sizeof(int),1,f);
            cur=cur->next;
        }
    }
    fclose(f);
    printf("Hash salva em %s\n",filename);
}
void loadHashFromFile(const char* filename){
    for(int i=0;i<HASH_SIZE;i++) hashTable[i]=NULL;
    FILE* f=fopen(filename,"rb");
    if(!f){ printf("Arquivo de heurística não encontrado.\n"); return; }
    char key[25]; int depth;
    while(fread(key,sizeof(char),25,f)==25){
        if(fread(&depth,sizeof(int),1,f)!=1) break;
        insertHash(key,depth);
    }
    fclose(f);
    printf("Hash carregada de %s\n",filename);
}

void preprocess(){
    for(int i=0;i<HASH_SIZE;i++){
        HNode* cur = hashTable[i];
        while(cur){
            HNode* t = cur;
            cur = cur->next;
            free(t);
        }
        hashTable[i]=NULL;
    }

    Queue q; initQueue(&q);
    Node* root=(Node*)malloc(sizeof(Node));
    initCube(root->cube);
    root->depth=0; root->parent=NULL; root->next=NULL;
    enqueue(&q,root);

    char key[25];
    cubeToString(root->cube,key);
    insertHash(key,0);

    void (*funcs[3])(char[6][4])={moveF,moveU,moveR};

    while(!isEmpty(&q)){
        Node* cur=dequeue(&q);
        cubeToString(cur->cube,key);
        int curDepth=findHash(key);

        for(int i=0;i<3;i++){
            Node* child=(Node*)malloc(sizeof(Node));
            copyCube(child->cube,cur->cube);
            funcs[i](child->cube);
            cubeToString(child->cube,key);

            if(findHash(key)==-1){
                insertHash(key,curDepth+1);
                child->depth=curDepth+1;
                child->parent=NULL;
                child->next=NULL;
                enqueue(&q,child);
            }else free(child);
        }
        free(cur);
    }
    printf("Pré-processamento concluído!\n");
}

// ------------------ A* ------------------

void pushOpen(OpenList* ol, AStarNode* n){
    if(!ol->head||n->f<ol->head->f){ n->next=ol->head; ol->head=n; return; }
    AStarNode* cur=ol->head;
    while(cur->next && cur->next->f<=n->f) cur=cur->next;
    n->next=cur->next; cur->next=n;
}
AStarNode* popOpen(OpenList* ol){
    if(!ol->head) return NULL;
    AStarNode* n=ol->head; ol->head=n->next; return n;
}

void solveAStar(char start[6][4]){
    char key[25];
    cubeToString(start,key);
    int h=findHash(key);
    if(h==-1){ printf("Estado inicial não encontrado na heurística!\n"); return; }

    OpenList ol={NULL};
    AStarNode* root=(AStarNode*)malloc(sizeof(AStarNode));
    copyCube(root->cube,start);
    root->parent=NULL; root->move=' ';
    root->g=0; root->h=h; root->f=h;
    root->next=NULL;
    pushOpen(&ol,root);

    void (*funcs[3])(char[6][4])={moveF,moveU,moveR};
    char moves[3]={'F','U','R'};

    int states=0;

    while(ol.head){
        AStarNode* cur=popOpen(&ol);
        states++;
        if(isCompletedCube(cur->cube)){
            printf("Solução encontrada!\n");
            printf("Movimentos: ");
            char sol[100]; int len=0;
            AStarNode* tmp=cur;
            while(tmp->parent){ sol[len++]=tmp->move; tmp=tmp->parent; }
            for(int i=len-1;i>=0;i--) printf("%c ",sol[i]);
            printf("\nProfundidade: %d\nEstados expandidos: %d\n",cur->g,states);
            printCubeVisualColored(cur->cube);
            return;
        }

        for(int i=0;i<3;i++){
            AStarNode* child=(AStarNode*)malloc(sizeof(AStarNode));
            copyCube(child->cube,cur->cube);
            funcs[i](child->cube);
            cubeToString(child->cube,key);
            int hh=findHash(key);
            if(hh==-1){ free(child); continue; }
            child->g=cur->g+1;
            child->h=hh;
            child->f=child->g+child->h;
            child->parent=cur;
            child->move=moves[i];
            child->next=NULL;
            pushOpen(&ol,child);
        }
    }
    printf("Nenhuma solução encontrada!\n");
}

// ------------------ Menu ------------------

int main(){
    setlocale(LC_ALL,"portuguese");
    srand((unsigned int)time(NULL));
    for(int i=0;i<HASH_SIZE;i++) hashTable[i]=NULL;

    char cube[6][4];
    initCube(cube);

    int selected=0, running=1, optionsCount=5;

    while(running){
        randCube(cube);
        system("cls");
        printf("Escolha o modo de jogo:\n\n");

        for(int i=0;i<optionsCount;i++){
            if(i==selected) printf("\x1b[34m");
            else printf("\x1b[0m");
            switch(i){
                case 0: printf("Jogar\n"); break;
                case 1: printf("Busca em Largura\n"); break;
                case 2: printf("Busca em Profundidade\n"); break;
                case 3: printf("A*\n"); break;
                case 4: printf("Fechar Programa\n"); break;
            }
        }

        printf("\x1b[0m\nUse as setas para navegar, Enter para selecionar.\n");

        int c=getch();
        if(c==0||c==224){
            c=getch();
            if(c==72){ selected--; if(selected<0) selected=optionsCount-1;}
            else if(c==80){ selected++; if(selected>=optionsCount) selected=0; }
        }
        else if(c==13){
            system("cls");
            switch(selected){
                case 0:
                    printf("Você escolheu Jogar!\n");
                    playInteractive(cube);
                    system("pause");
                    break;
                case 1:
                    printf("Você escolheu Busca em Largura!\n");
                    searchSolver(cube,16,0);
                    system("pause");
                    break;
                case 2:
                    printf("Você escolheu Busca em Profundidade!\n");
                    searchSolver(cube,16,1);
                    system("pause");
                    break;
                case 3: {
                    int opAst=0, rodandoAst=1;
                    while(rodandoAst){
                        system("cls");
                        printf("=== Modo A* ===\n\n");
                        if(opAst==0) printf("-> Pré-processar heurística\n");
                        else printf("   Pré-processar heurística\n");
                        if(opAst==1) printf("-> Resolver com A*\n");
                        else printf("   Resolver com A*\n");

                        printf("\nUse as setas para navegar, Enter para selecionar, ESC para voltar.\n");

                        int c=getch();
                        if(c==0||c==224){
                            c=getch();
                            if(c==72){ opAst--; if(opAst<0) opAst=1; }
                            else if(c==80){ opAst++; if(opAst>1) opAst=0; }
                        }
                        else if(c==13){
                            system("cls");
                            switch(opAst){
                                case 0:
                                    printf("Pré-processando...\n");
                                    preprocess();
                                    system("pause");
                                    break;
                                case 1:
                                    printf("Resolvendo com A*...\n");
                                    solveAStar(cube);
                                    system("pause");
                                    break;
                            }
                        }
                        else if(c==27){ // ESC
                            rodandoAst=0;
                        }
                    }
                    break;
                }
                case 4:
                    printf("Você escolheu fechar o programa!\n");
                    running=0;
                    break;
            }
        }

    }

    return 0;
}
