#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <string.h>

#define ANSI_BG_WHITE   "\x1b[47m"
#define ANSI_BG_YELLOW  "\x1b[43m"
#define ANSI_BG_RED     "\x1b[41m"
#define ANSI_BG_ORANGE  "\x1b[48;5;208m"
#define ANSI_BG_BLUE    "\x1b[44m"
#define ANSI_BG_GREEN   "\x1b[42m"
#define ANSI_RESET      "\x1b[0m"


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
    for(int i=0; i<20; i++){
        int move = rand()%6;
        switch(move){
            case 0: moveF(cube); break;
            case 1: moveU(cube); break;
            case 2: moveD(cube); break;
            case 3: moveL(cube); break;
            case 4: moveR(cube); break;
            case 5: moveB(cube); break;
        }
    }
}

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

void initQueue(Queue* q){ 
    q->front=NULL; 
    q->rear=NULL; 
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
    if(isEmpty(q)){
        return NULL;
    }
    Node* n=q->front;
    q->front=n->next;
    if(q->front==NULL){
        q->rear=NULL;
    } 
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
            if(c[f][i]!=col){
                return 0;
            } 
        } 
    }
    return 1;
}

void bfsSolver(char start[6][4], int maxDepth){
    Queue q; initQueue(&q);
    Node* root = (Node*)malloc(sizeof(Node));
    copyCube(root->cube,start);
    root->parent=NULL; root->move=' '; root->depth=0; root->next=NULL;
    enqueue(&q, root);

    void (*funcs[3])(char[6][4]) = {moveF, moveU, moveR};
    char moves[3] = {'F','U','R'};

    while(!isEmpty(&q)){
        Node* cur = dequeue(&q);

        if(isCompletedCube(cur->cube)){
            printf("Cubo inicial embaralhado:\n"); printCubeVisualColored(start);
            printf("Quantidade de movimentos realizados: %d\n", cur->depth);

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

    printf("Nenhuma solucao encontrada até profundidade %d\n", maxDepth);
}


void playInteractive(char cube[6][4]){
    int playing=1,movs=0;
    do{
        system("cls"); printCubeVisualColored(cube);
        printf("Comandos F, U, R, L, B, D\n");
        char c = getch();
        switch(c){
            case 70: case 102: moveF(cube); break;
            case 85: case 117: moveU(cube); break;
            case 66: case 98: moveB(cube); break;
            case 82: case 114: moveR(cube); break;
            case 76: case 108: moveL(cube); break;
            case 68: case 100: moveD(cube); break;
            default: movs--; break;
        }
        movs++; playing = !isCompletedCube(cube);
    }while(playing);

    system("cls");
    printf("Completo em %d movimentos\n", movs);
}

int main(){
    srand(time(NULL));
    char cube[6][4];
    initCube(cube);
    randCube(cube);

    int selected=0, running=1, optionsCount=4;

    while(running){
        system("cls");
        printf("Escolha o modo de jogo\n\n");

        for(int i=0;i<optionsCount;i++){
            if(i==selected) printf("\x1b[34m");
            else printf("\x1b[0m");
            switch(i){
                case 0: printf("Jogar\n"); break;
                case 1: printf("Busca em Largura\n"); break;
                case 2: printf("Busca em Profundidade\n"); break;
                case 3: printf("A*\n"); break;
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
            running=0; system("cls");
            switch(selected){
                case 0: printf("Voce escolheu Jogar!\n"); playInteractive(cube); break;
                case 1: printf("Voce escolheu Busca em Largura!\n"); bfsSolver(cube,15); break;
                case 2: printf("Voce escolheu Busca em Profundidade!\n"); break;
                case 3: printf("Voce escolheu A*!\n"); break;
            }
        }
    }

    return 0;
}
