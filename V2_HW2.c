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
pthread_mutex_t mutex;
int numTotalNodes;
int turn = 1; // 1 Denotes it is create time, 2 Denotes it is verify time, 0 Denotes end of program

int listLength()
{	
	node *conductor;
	printf("checking length of list\n");
	if (root != NULL){
		conductor = root;
	  	int help = 0;

		while ( conductor->next != 0 ) {
		    conductor = conductor->next;
		    help++;
		}
		// printf("LIST LENGTH IS %d\n",help );
		// pthread_mutex_unlock(&mutex);
		return help;
	
	}else{
		// printf("LIST LENGTH IS %d \n",0 );
		// pthread_mutex_unlock(&mutex);
		return 0;
	}
}

void printList(){
	node *conductor;
	conductor = root;
	int help = 0;
	while ( conductor->next != 0 ) {
	    printf( "%d, \t", conductor->x );
	    conductor = conductor->next;
	    help++;
	}
	printf("NUM ITEMS: %d\n",help );
	
}


/* This is our thread function.  It is like main(), but for a thread */
void *create(void *arg)
{	
	node *conductor;

	printf("create thread made\n");

	srand(time(NULL));
	threadParameters* threadParams = (threadParameters*) arg;

	pthread_mutex_lock(&mutex);
	printf("create has lock, generating root\n");
	root = malloc( sizeof(struct node) );
	root->x = rand()%threadParams->max;
	conductor = root; 
	root->next = conductor;
	conductor->x =  rand()%threadParams->max;
	conductor->next = 0; 
	printf("root generated, releasing lock\n");
	pthread_mutex_unlock(&mutex);
			
	while (turn != 0){
			printf("Create: turn = %d\n", turn);
			while(!pthread_mutex_lock(&mutex) && (listLength() < numTotalNodes))
			{
				/* Creates a node at the end of the list */
				// pthread_mutex_unlock(&mutex);
				printf("yay\n");
				// pthread_mutex_lock(&mutex);
				conductor = root;

				while(conductor->next != 0){
					// printf("%d=>",conductor->x );
					conductor = conductor->next;
				}
				// printf("\n");
				// sleep(1);
		        conductor->next = malloc( sizeof(struct node) );  
	
		        conductor = conductor->next; 
		        if ( conductor == 0 )
		        {
		            printf( "Out of memory" );
		            return 0;
		        }
		        conductor->next = 0;
		        conductor->x = rand()%threadParams->max;
		        printf("node added\n");
		        
		        // printf("released\n");
		        pthread_mutex_unlock(&mutex);
			}
			if (pthread_mutex_trylock(&mutex))
			{
				
			pthread_mutex_unlock(&mutex)		;	
			}
	}
	printf("~~~Create thread is kill.~~~\n");
	return NULL;
}

void *verify(void *arg)
{		
	node *conductor;
	node *prev;

	threadParameters* threadParams = (threadParameters*) arg;
	while (turn != 0){
		// printf("hi from verify\n");
		

		if(!pthread_mutex_lock(&mutex) && (listLength() != 0)){
			// printf("hi\n");
			// !pthread_mutex_lock(&mutex);
			conductor = root;
			// while(conductor->next != 0){
			// 	prev = conductor;
			// 	conductor = conductor->next;
			// }
			// printf("got them\n");
			// !pthread_mutex_unlock(&mutex);
			node *freeme;
			node *nextNode;
			while ( /*!pthread_mutex_lock(&mutex) && */(conductor->next != 0)  /*&& !pthread_mutex_unlock(&mutex) */) {
				// printf("verifing\n");
				// pthread_mutex_lock(&mutex);
			    if ( abs((conductor->x - conductor->next->x)) <= threadParams->distanceBetweenNumbers )
			    {
			    	// counter++;
			    	freeme = conductor->next;
			    	nextNode = freeme->next;
			    	free( freeme );
			   		conductor->next = nextNode;
			   		// printf("node deleted\n");
			    }else{
			    conductor = conductor->next;
				}
				
			}
			
		
		}
		pthread_mutex_unlock(&mutex);
		if(!pthread_mutex_lock(&mutex) && (listLength() == 100)){
			// printf("error\n");
			turn = 0;
		}
		pthread_mutex_unlock(&mutex);
	
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
//	printf("Distance between nodes has been set to: %d\n", threadParams->distanceBetweenNumbers);
	

	numTotalNodes = threadParams->numNodes;

	
	pthread_t ath,vth;	// this is our thread identifier
	int i = 0;

	


	pthread_create(&ath,NULL,&create,threadParams);
	pthread_create(&vth,NULL,&verify,threadParams);
	
	pthread_join(ath,NULL);
	pthread_join(vth,NULL);
	
	printList();
	
	return 0;
}