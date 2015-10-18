#include <pthread.h>
#include <stdio.h>

typedef struct node {
    int val;
    struct node * next;
} node_t;

/* This is our thread function.  It is like main(), but for a thread*/
void *threadFunc(void *arg)
{
	char *str;
	int i = 0;

	str=(char*)arg;

	while(i < 3)
	{
		printf("threadFunc says: %s\n",str);
		++i;
	}

	return NULL;
}

int main(void)
{
	pthread_t linked, sub;	// this is our thread identifier
	int i = 0;

	pthread_create(&linked,NULL,threadFunc,"foo");
	
	// while(i < 5)
	// {
		
		printf("main is running...\n");
		++i;
	// }

	printf("main waiting for thread to terminate...\n");
	pthread_join(linked,NULL);

	return 0;
}