#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<pthread.h>

/* FIFO from https://github.com/geekfactory/FIFO */
#include "../FIFO/FIFO.h"


pthread_mutex_t mutex;

int N, T;

/* Random Number Generator */
int generateNum(){
    return 1+ (rand()%40);
}

/* FIFO Q variable */
fifo_t myfifo;

/* Players Struct */
typedef struct players{
    char name[40];
    int points;
    pthread_t thread;
}players;

/* Function prints the whole FIFO Queue */
void fifo_print(fifo_t fifoo){
    int c;
    fifo_t tmpfifo = fifo_create(T, sizeof(int));
    memcpy(tmpfifo, fifoo, T*sizeof(int));
    printf("( ");
    while(!fifo_is_empty(tmpfifo)){
        fifo_get(tmpfifo, &c);
        printf("%d ", c);
    }
    printf(")\n");
}

players* plyrs;
int reached100 = 0;

/* Players routine */
void* playersRoutine(void* arg){
    int k = *(int*)arg;
    struct timespec sleepTime;
    sleepTime.tv_sec = 1;
    int x;
    while(!reached100){
        if(!fifo_is_empty(myfifo)){
           pthread_mutex_lock(&mutex);        
            /* Players grabs a number from queue */
            fifo_get(myfifo, &x);
            printf("\nThread %d pop %d from queue", k, x);
            sleepTime.tv_nsec = x;
            if(x<=N){
                printf(" (%d). Final bonus for object : no need to push back.\n\n",x);
            }
            else if(x%N==k || x%N == (k+1)%N){
                printf(" (%d). Matched. Need to push 2/5*x back on queue.\n\n",x*2/5);   
            }
            else{
                printf(". Not matched. Need to push x - 2 back on queue.\n\n"); 
            }
            fifo_print(myfifo);
            pthread_mutex_unlock(&mutex);
            
            nanosleep(&sleepTime, NULL);
            if(x<=N){
                plyrs[k].points += x;            
                x = -1;
            }
            else if(x%N==k || x%N == (k+1)%N){
                plyrs[k].points += 2*x/5;
                x -= 2*x/5;
            }
            else{
                x-=2;
            }

            if(x>0){
                pthread_mutex_lock(&mutex);
                printf("\nThread %d pushing %d to queue\n\n", k, x);
                /* Players inserts number to queue */
                fifo_add(myfifo, &x);
                fifo_print(myfifo);
                pthread_mutex_unlock(&mutex);
            }
            if(plyrs[k].points>=100){
                reached100 = 1;
            }
        }
    }
}

/* Dealers Routine */
void* dealersRoutine(void* arg){
    while(!reached100){
        int c = generateNum();
        pthread_mutex_lock(&mutex);
        printf("\nDealer pushing %d to queue.\n\n", c);
        /* Dealer inserts a random number */
        fifo_add(myfifo, &c);
        fifo_print(myfifo);
        pthread_mutex_unlock(&mutex);
        pthread_mutex_unlock(&mutex);
        while(myfifo->storedbytes/sizeof(int) > N && !reached100);
    }
}

int main(int argc, char *argv[]){
    int totalPlayers;

    /* Opens the input file on read mode */
    FILE* myfile;
    if((myfile = fopen(argv[1], "r")) == NULL){
        printf("File did not opened.\n");
        return 1;
    }
    fscanf(myfile, "%d %d %d", &N, &T, &totalPlayers);

    printf("Number of threads : %d   |   Number of objects : %d\n\n",N,T);
    
    srand(time(0)); // declaring seed for rand(), it depends to time

    pthread_t dealert;

    /* Initializes players according to input file */
    plyrs = (players*)malloc(N*sizeof(players));
    for(int i=0;i<N;i++){
        plyrs[i].points = 0;
        fscanf(myfile, "%s", plyrs[i].name);
    }

    myfifo = fifo_create((N*2)+1, sizeof(int));

    int playersWon = 0;
    
    /* Creates games till there is no more player left */
    while(playersWon < totalPlayers){
        reached100=0;
        
        /* dealer's thread */
        pthread_mutex_init(&mutex, NULL);
        /* players' thread */
        for(int i =0; i<N; i++){
            int* a = malloc(sizeof(int));
            *a=i;
            if(strcmp(plyrs[i].name, "null") != 0){
                if(pthread_create(&plyrs[i].thread, NULL, playersRoutine, a) != 0){
                    printf("Thread create failed\n");
                }
            }
        }
        if(pthread_create(&dealert, NULL, dealersRoutine, 0) != 0 ) {
            printf("Thread create failed.\n");
        }

        if(pthread_join(dealert, NULL) != 0){
            printf("Thread join failed.\n");
        }
        for(int i=0;i<N;i++){
            if(strcmp(plyrs[i].name, "null") != 0){
                if(pthread_join(plyrs[i].thread, NULL) != 0){
                    printf("Thread join failed\n");
                }
            }
        }

        pthread_mutex_destroy(&mutex);
        /* this for loop finds the winner */
        for(int i=0;i<N;i++){
            if(plyrs[i].points>100){
                playersWon++;
                printf("Player %s left with %d points.\n", plyrs[i].name, plyrs[i].points);
                if(!feof(myfile)){
                    fscanf(myfile, "%s", plyrs[i].name);
                    printf("New player: %s joined.\n", plyrs[i].name);
                    plyrs[i].points = 0;
                }
                else{
                    strcpy(plyrs[i].name, "null");
                    plyrs[i].points = 0;
                }
            }
        }
    }
    
    
    printf("=========================\n");
    printf("Game is ended.\n");
    fclose(myfile);
    return 1;
}
