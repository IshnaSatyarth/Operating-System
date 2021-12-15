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
    int points;
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

/* Players routine */
void* playersRoutine(void* arg){
    int k = *(int*)arg;
    int x;
    struct timespec sleepTime;
    sleepTime.tv_sec = 1;
    
    while(!fifo_is_empty(myfifo)){
        pthread_mutex_lock(&mutex);        
        /* Players grabs a number from queue */
        fifo_get(myfifo, &x);
        sleepTime.tv_nsec = x;
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
    }
}

/* Dealers Routine */
void* dealersRoutine(void* arg){
    int z=0;
    while(z<T){
        int c = generateNum();
        pthread_mutex_lock(&mutex);
        printf("\nDealer pushing %d to queue.\n\n", c);
        /* Dealer inserts a random number */
        fifo_add(myfifo, &c);
        fifo_print(myfifo);
        pthread_mutex_unlock(&mutex);
        z++;
        while(myfifo->storedbytes/sizeof(int)>N && z<T);
    }
}

int main(int argc, char *argv[]){

    srand(time(0)); // declaring seed for rand(), it depends to time

    N = atoi(argv[1]);
    T = atoi(argv[2]);

    printf("Number of threads : %d   |   Number of objects : %d\n\n",N,T);

    /* declaring threads */
    pthread_t playerst[N];
    pthread_t dealert;

    /* Initializes players  */
    plyrs = (players*)malloc(N*sizeof(players));
    for(int i=0;i<N;i++){
        plyrs[i].points = 0;
    }

    myfifo = fifo_create(T, sizeof(int));
    
    pthread_mutex_init(&mutex, NULL);

    /* dealer's thread */
    if(pthread_create(&dealert, NULL, dealersRoutine, 0) != 0 ) {
        printf("Thread create failed.\n");
    }
    /* players' thread */
    for(int i=0; i<N; i++){
        int* a = malloc(sizeof(int));
        *a=i;
        if(pthread_create(&playerst[i], NULL, playersRoutine, a) != 0){
            printf("Thread create failed\n");
        }
        printf("Thread %d started\n", i);
    }

    if(pthread_join(dealert, NULL) != 0){
        printf("Thread join failed.\n");
    }
    for(int i=0;i<N;i++){
        if(pthread_join(playerst[i], NULL) != 0){
            printf("Thread join failed\n");
        }
    }

    pthread_mutex_destroy(&mutex);


    int biggestPoints = 0;
    int winnerIndex;
    /* this for loop finds the winner and prints all players and their scores */
    for(int i=0;i<N;i++){
        if(plyrs[i].points>biggestPoints){
            winnerIndex = i;
            biggestPoints = plyrs[i].points;
        }
        printf("Player %d finished with %d points.\n",i,  plyrs[i].points);
    }
    printf("--------------------\n");
    printf("Winner: Player %d with %d points.\n", winnerIndex, plyrs[winnerIndex].points);
    return 1;
}
