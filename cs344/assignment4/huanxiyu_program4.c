#define  _POSIX_C_SOURCE 200809L


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define bufferSize 2000

int buffer1Size = 0;
char buffer1[bufferSize] = "";

// Initialize the mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Initialize the condition variables
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;


void getInput();
void line_separator(char* );
void plus_sign(char* , int);
void* inputProducer(void*);
void* lineSeparatorThread(void*);


void getInput()
{
	char temp[bufferSize];
	memset(temp, 0, bufferSize);
	fgets(temp, bufferSize, stdin);				
	/*if(getline(&temp, bufferSize, stdin) == -1)
	{
		if(getline(&temp, 0, stdin) == -1)
		{
			printf("No line\n");
		}
	}else{
		strcat(buffer1, temp);
	}*/
	strcat(buffer1,temp);
	buffer1Size = strlen(buffer1);
}

void line_separator(char* temp)
{

	for(int i = 0; i < strlen(buffer1); i++)
	{
		if(buffer1[i] == '\n')
		{
			buffer1[i] = ' ';
		}
		buffer1Size--;
	}
	strcat(temp, buffer1);
	memset(buffer1, 0, sizeof(buffer1));
}

/*void plus_sign(char* buffer2, int buffer2Size)
{
	int count = 0;
	int index = 0;
	int check = 0;

	for(int i = 0; i < buffer2Size; i++)
	{
		if(buffer2[i] == '+')
		{
			count = count + 1;
		}
		if(count == 1 && buffer2[i + 1] != '+')
		{
			count = 0;

		}
		if(count == 2)
		{
			count = 0; 
			buffer2[i] = '^';
			buffer2[i - 1] = '^';
		}
	}

	for(int i = 0 ; i < buffer2Size; i++)
	{
		if(buffer2[i] == '^')
		{
			check = check + 1;		
		}
		if(check == 2)
		{
			check = 0;
			for(int j = i; j <= buffer2Size; j++)
			{
				buffer2[j - 1] = buffer2[j];
			}
			buffer2[buffer2Size] = '\0';
			buffer2Size--;
			i--;
		}
	}	
}*/

void* inputProducer(void* args)
{
	char endMarker[] = "DONE";
	
	while(1)
	{
		// Lock the mutex before checking if the buffer has data      
      		pthread_mutex_lock(&mutex);

		while(buffer1Size > 80){
			pthread_cond_wait(&empty, &mutex);
		}
		getInput();
		pthread_cond_signal(&full);
		pthread_mutex_unlock(&mutex);	
		printf("the length %d and the input before is: %s", buffer1Size, buffer1);
		if(strstr(buffer1, endMarker) != NULL){
			exit(0);
		}
	}

	//printf("the input before is: %s", (char*) args);
}

void* lineSeparatorThread(void* args)
{
		char temp[bufferSize];
		pthread_mutex_lock(&mutex);
		//printf("the size of buffer in lineSep is %d\n", buffer1Size);
		while(buffer1Size == 0){
			pthread_cond_wait(&full, &mutex);
		}
		line_separator(temp);
		printf("********************************");
		printf("%s", temp);		
		pthread_cond_signal(&empty);
		pthread_mutex_unlock(&mutex);

		//printf("\n***********************\n");
}

int main()
{

	pthread_t input, lineSeparator;
	//int doWhile = 0;
	//int count = 0;
	//char test[] = "abc+++def++efg+++++bcg++uibDONE";
	/*while(doWhile == 0)
	{
		buffer1Size = getInput(buffer1);
		printf("the input before is: %s", buffer1);
		line_separator(buffer1, buffer1Size);
		printf("the input after  is: %s", buffer1);
		count = count + 1;
		if(count == 3){
			doWhile = 1;
		}
	}*/
	/*plus_sign(test, strlen(test));
	printf("the length of string is %d\n", strlen(test));
	printf("the test string after modify: %s\n", test);*/
	
	pthread_create(&input, NULL, inputProducer, NULL);
	sleep(1);
	pthread_create(&lineSeparator, NULL, lineSeparatorThread, NULL);

	pthread_join(input, NULL);
	pthread_join(lineSeparator, NULL);
	return 0;
}
