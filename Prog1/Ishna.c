/**
 * A pthread program illustrating how to
 * create a simple thread and some of the pthread API
 * This program implements the prime number checking function where
 * the prime number check operation is run as a separate thread.
 *
 *
 */

#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

void *runner(void *param); /* the thread */
bool *p;      /* this data is shared by the thread(s) */
int n, t;

//int main(int argc, char *argv[])
int main()
{
    int v;
    printf("Enter values for N and T, where T should be greater than square root of N : ");
    scanf("%d  %d ", &n, &t);
    v = (int)(sqrt(n));
    if(t < v){
        printf("Enter values for N and T, such that T should be greater than square root of N. In this case N = %d :then T >= %d", n, v);
        return 0;
    }
/*
    do {
        printf("Enter values for N and T, where T should be greater than square root of N : ");
        scanf("%d  %d ", &n, &t);
        //int n = atoi(argv[1]);
        //int t = atoi(argv[2]);
        printf("n : %d  t : %d \n", n, t);
        v = (int)(sqrt(n));
    }while(t < v);*/


    printf("n : %d  t : %d \n", n, t);
    p = (bool *)malloc(sizeof(bool) * n);
    pthread_t tid[t]; /* the thread identifier */
    pthread_attr_t attr; /* set of attributes for the thread */

    for(int i = 1; i <= n; i++) {
        p[i] = true;
    }

    /* get the default attributes */
    pthread_attr_init(&attr);

    /* create the thread */
    for (int i = 2; i < t+2; i++)
    {
        int *x = (int *) malloc(sizeof(int));
        *x = i;
        pthread_create(&(tid[i]), &attr, runner, (void *) x);
        sleep(3);

    }

    /* now wait for the thread to exit */
    for (int i = 1; i <= n; i++) {
        pthread_join(tid[i], NULL);
    }

    /* Print prime numbers from 2 to N, since 1 is not prime */
    for (int i = 2; i <= n; i++) {
        if(p[i] == 1){
            printf(" %d \n ", i);
        }
    }
    free(p);
}

/**
 * The thread will begin control in this function
 */
void *runner(void *param)
{
    int  q =  *((int *) param);
    int a;
    /*Check if p[q] is a prime number */
    if(p[q] == true && q <= n/2){
        /*Loop from 2 to n/q, since for j=n/q,  the value of a = q*j = q*n/q = n values */
        for (int j = 2; j <= n / q; j++) {
            a = q * j;
            /*Change all the multiples of prime numbers to false */
            p[a] = false;
        }
        q++;
    }
    pthread_exit(0);
}
