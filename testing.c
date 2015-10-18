#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
// #include <stdbool.h>


typedef int bool; 
#define true 1
#define false 0

typedef struct node {
  int x;
  struct node *next;
} node ;

typedef struct threadParameters
{
	int max;
	int numNodes;
	node* start;
	int distanceBetweenNumbers;
} threadParameters;

node *root;
node *conductor;
node *tail;
pthread_mutex_t mutex;
int numTotalNodes;
int turn = 1; // 1 Denotes it is create time, 2 Denotes it is verify time, 0 Denotes end of program

int listLength()
{	
	if (root != NULL){
		conductor = root;
	  	int help = 0;

		while ( conductor->next != 0 ) {
		    conductor = conductor->next;
		    help++;
		}
		tail = conductor;
		return help;
	
	}else{
		return 0;
	}
}

void printList(){
	conductor = root;
	int help = 0;
	while ( conductor->next != 0 ) {
	    printf( "%d, \t", conductor->x );
	    conductor = conductor->next;
	    help++;
	}
	printf("NUM ITEMS: %d\n",help );
	
}




void *verify(void *arg)
{		
	threadParameters* threadParams = (threadParameters*) arg;
	// if(pthread_mutex_trylock(&mutex) == 0){
	// 	printf("lock acquired\n");
	// }
	printf("%d\n",pthread_mutex_trylock(&mutex) );
	printf("%d\n",pthread_mutex_unlock(&mutex) );
	if( !pthread_mutex_trylock(&mutex) && 0<20 && !pthread_mutex_unlock(&mutex)){
		printf("lock acquired and releaed\n");
	}else{
		printf("lock not acquired\n");
	}

	return NULL;
}




int main(int argc, char * argv [] )
{

	if (argc != 3 && argc != 4)
	{	//Check to see if we were given 2 or 3 parameters, if not, exit.
		printf("ERROR: Please include three (3) integer paramters.\n");
		printf("1) The number of random numbers you want generated\n2) The maximum number any of them could be\n3) The disntance between the numbers you want (OPTIONAL, default = 8)\n");
		exit(0);
	}	
	// MAKE THESE PUT INTO 
	threadParameters* threadParams;
	threadParams = malloc( sizeof(threadParameters));
	threadParams->numNodes = atoi(argv[1]);
	threadParams->max = atoi(argv[2]);
	threadParams->start = root;

	if (argc == 4)
	{
		/* For fun, if you give it a fourth parameter, you can specify the distance between the numbers */
		threadParams->distanceBetweenNumbers = atoi(argv[3]);
	}else{
		threadParams->distanceBetweenNumbers = 8;
		
	}

	if(threadParams->distanceBetweenNumbers >= threadParams->max){
			printf("Error, cannot create nodes. Please ensure your distance is less than your maximum.\n");
			exit(0);
	}
	printf("Distance between nodes has been set to: %d\n", threadParams->distanceBetweenNumbers);
	

	numTotalNodes = threadParams->numNodes;

	
	pthread_t ath,vth;	// this is our thread identifier
	int i = 0;

	


	// pthread_create(&ath,NULL,create,threadParams);
	pthread_create(&vth,NULL,verify,threadParams);
	
	// pthread_join(ath,NULL);
	pthread_join(vth,NULL);
	
	// printList();
	
	return 0;
}