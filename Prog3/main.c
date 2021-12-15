#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

int k, m, n;
int* toolNumbers;


sem_t* tools;
pthread_mutex_t mutex;
int Request(int toolNumber, int philIndex);

struct node_t {
	int data;
	struct node_t* next;
}typedef node_t;

void ReleaseAll(node_t* ordered);

struct Philosopher {
	char name[20];
	int orderCount;
	node_t* orderNode;
	node_t* ordered;
}typedef Phil;

Phil* phils;

void print_Node(node_t* head) {
	node_t* iter = head;
	while (iter != NULL) {
		printf("%d ", iter->data);
		iter = iter->next;
	}
	printf("\n");
}

void push_Node(node_t* node, int n) {

	node_t* iter = node;

	while (iter->next != NULL) {
		iter = iter->next;
	}

	iter->next = (node_t*)malloc(sizeof(node_t));
	iter = iter->next;
	iter->data = n;
}

node_t* delete_Node(node_t* head) {
	if (head->next != NULL) {
		return head->next;
	}
	else {
		return NULL;
	}
	
}

node_t* create_Node(int val) {
	node_t* newNode = malloc(sizeof(node_t));
	newNode->data = val;
	newNode->next = NULL;
	return newNode;
}



int start = 0;
int done = 0;
int counter = 0;

void* routine(void* arg) {
	int index = *(int*)arg;
	index--;
	while (counter<n) {
		pthread_mutex_lock(&mutex);
		int philIndex = counter;
		counter++;
		pthread_mutex_unlock(&mutex);
		Phil* temp = (phils+philIndex);
		printf("%s sits down at table %d\n", temp->name, index);
		while (!start);		
		time_t t;
		while (temp->orderCount > 0) {
			pthread_mutex_lock(&mutex);
			t = time(0);
			printf("%s requests: ", temp->name);
			if (t % 2) {
				printf("%d ", temp->orderNode->data);
				if (Request(temp->orderNode->data, philIndex) == 0) {
					printf("\n%s requests %d, granted", temp->name, temp->orderNode->data);
					if (temp->ordered == NULL) {
						temp->ordered = create_Node(temp->orderNode->data);
					}
					else {
						push_Node(temp->ordered, temp->orderNode->data);
					}
					temp->orderNode =delete_Node(temp->orderNode);
					temp->orderCount--;
				}
				else {
					printf("\n%s requests %d, denied", temp->name, temp->orderNode->data);
					push_Node(temp->orderNode, temp->orderNode->data);
					temp->orderNode = delete_Node(temp->orderNode);
				}
			}
			else {
				printf("%d ", temp->orderNode->data);
				if(temp->orderNode->next !=NULL)
					printf("% d ", temp->orderNode->next->data);
				
				if (Request(temp->orderNode->data, philIndex) == 0) {
					printf("\n%s requests %d, granted", temp->name, temp->orderNode->data);
					if (temp->ordered == NULL) {
						temp->ordered = create_Node(temp->orderNode->data);
					}
					else {
						push_Node(temp->ordered, temp->orderNode->data);
					}
					temp->orderNode = delete_Node(temp->orderNode);
					temp->orderCount--;
				}
				else {
					printf("\n%s requests %d, denied", temp->name, temp->orderNode->data);
					push_Node(temp->orderNode, temp->orderNode->data);
					temp->orderNode = delete_Node(temp->orderNode);
				}
				if (temp->orderNode != NULL) {
					
					if (Request(temp->orderNode->data, philIndex) == 0) {
						printf("\n%s requests %d, granted", temp->name, temp->orderNode->data);
						if (temp->ordered == NULL) {
							temp->ordered = create_Node(temp->orderNode->data);
						}
						else {
							push_Node(temp->ordered, temp->orderNode->data);
						}
						temp->orderNode = delete_Node(temp->orderNode);
						temp->orderCount--;
					}
					else {
						printf("\n%s requests %d, denied", temp->name, temp->orderNode->data);
						push_Node(temp->orderNode, temp->orderNode->data);
						temp->orderNode = delete_Node(temp->orderNode);
					}
				}
			}
			printf("\n");
			pthread_mutex_unlock(&mutex);
			
			struct timespec tim;
			tim.tv_sec = 1;
			tim.tv_nsec = (t % 2) * 1000000;
			nanosleep(&tim, NULL);
			
		}

		sleep(2);
		printf("\n%s finishes, releasing ", temp->name);
		print_Node(temp->ordered);
		ReleaseAll(temp->ordered);
		
	}
	done = 1;
}

int main(int argc, char** argv) {
	FILE* myfile;
	if (argc != 2) {
		printf("\nInvalid args\n");
	}
	if ((myfile=fopen(argv[1], "r")) == 0) {
		printf("\nfile did not opened\n");
	}

	//initialization starts from here

	fscanf(myfile, "%d %d %d", &k, &m, &n);


	phils = (Phil*) malloc(sizeof(Phil)*n);
	tools = (sem_t*)malloc(sizeof(sem_t)*k);
	toolNumbers = (int*) malloc(sizeof(int)*k);

	pthread_t threads[m];
	
	for (int i = 0; i < k; i++) {
		fscanf(myfile, "%d", &toolNumbers[i]);
		sem_init(&tools[i], i, toolNumbers[i]);
	}

	Phil* tempPhil;
	
	for (int i = 0; i < n; i++) {
		tempPhil = &phils[i];
		fscanf(myfile, "%s %d", tempPhil->name, &tempPhil->orderCount);
		
		int temp;
		for (int j = 0; j < tempPhil->orderCount; j++) {
			
			
			fscanf(myfile, "%d ", &temp);
			if (tempPhil->orderNode == NULL) {
				tempPhil->orderNode = create_Node(temp);
			}
			else {
				push_Node(tempPhil->orderNode, temp);
			}
		}
	}
	//initialization ends here ^^^^


	for (int i = 0; i < m; i++) {
		pthread_create(&threads[i], NULL, routine, &i);
	}

	
	start = 1;
	while (!done);

	printf("No one else is coming\n");
	for (int i = 0; i < m; i++) {
		pthread_join(threads[i], NULL);
	}


	for (int i = 0; i < k; i++) {
		sem_destroy(&tools[i]);
	}

	printf("All done\n");
}

int Request(int toolNumber, int pIndex) {
	int i;
	sem_getvalue(&tools[toolNumber], &i);
	if (i == 0) {
		return -1;
	}
	else {
		return sem_wait(&tools[toolNumber]);
	}
}
void ReleaseAll(node_t* ordered) {
	while (ordered != NULL) {
		sem_post(&tools[ordered->data]);
		ordered = ordered->next;

	}
}