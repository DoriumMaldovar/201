#include <stdio.h>
#include <pthread.h>
// #include <unistd.h>
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
	    printf( "%d=>", conductor->x );
	    conductor = conductor->next;
	    help++;
	}
	printf("END\n" );
	
}


/* This is our thread function.  It is like main(), but for a thread */
void *create(void *arg)
{	

	srand(time(NULL));
	threadParameters* threadParams = (threadParameters*) arg;
			
	while (turn > 0){
		if((turn == 1) && (pthread_mutex_trylock(&mutex))){
			printf("Create has Mutex Lock.\n");
			
			if(root == NULL){
				root = malloc( sizeof(struct node) );
		    	root->x = rand()%threadParams->max;
				conductor = root; 
				root->next = conductor; 
				conductor->next = 0; 
			}else{
				conductor = tail;
			} 
			while( listLength() < numTotalNodes )
			{
				/* Creates a node at the end of the list */
		        conductor->next = malloc( sizeof(struct node) );  
	
		        conductor = conductor->next; 
		        if ( conductor == 0 )
		        {
		            printf( "Out of memory" );
		            return 0;
		        }
		        conductor->next = 0;
		        conductor->x = rand()%threadParams->max;
			}
			turn = 2;
			printf("Create has released Mutex Lock.\n");
			pthread_mutex_unlock(&mutex);
		}
	}
	printf("~~~Create thread is kill.~~~\n");
	return NULL;
}

void *verify(void *arg)
{		
	threadParameters* threadParams = (threadParameters*) arg;
	while (turn > 0){
		if (turn == 2 && pthread_mutex_trylock(&mutex)){
			printf("Verify has Mutex Lock.\n");
			if(listLength() != 0){
				conductor = root;
				// int counter = 0;
				node *freeme;
				node *nextNode;
				while ( conductor->next != 0 ) {
	
				    if ( abs((conductor->x - conductor->next->x)) <= threadParams->distanceBetweenNumbers )
				    {
				    	// counter++;
				    	freeme = conductor->next;
				    	nextNode = freeme->next;
				    	free( freeme );
				   		conductor->next = nextNode;
				    }else{
				    conductor = conductor->next;
					}
				}
				if (listLength() == numTotalNodes){
					turn = 0;
					printf("###Final list created, required nodes created, all criterion met.###\n");
				}else{		
					turn = 1;
				}


		}else{
			turn = 1;
		}
		printf("Verify has released Mutex Lock.\n");
		pthread_mutex_unlock(&mutex);
		}
	}
	printf("~~~Verify thread is kill.~~~\n");
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

	pthread_create(&ath,NULL,create,threadParams);
	pthread_create(&vth,NULL,verify,threadParams);
	
	pthread_join(ath,NULL);
	pthread_join(vth,NULL);
	
	printList();
	
	return 0;
}