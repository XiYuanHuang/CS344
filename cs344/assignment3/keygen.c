/**********************************************************************
** * Program: keygen
** * Author: XiYuan Huang
** * Date: August 1, 2020
** * Description: This program generates the key file and the length 
** *		  of the file depends on the user
** * Input: The user's input decides the length of the file
** * Output: the key file 
**********************************************************************/

/**********************************************************************
** * Included Libraries
**********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


/*********************************************************************************
** * Function: main
** * Description: The function uses the random number to create a unique key file
** * Parameters: int argc, char const* argv[]
** * Pre-Conditions: the user need to follow the format of command line
** * Post-Conditions: the unique key file
**********************************************************************************/
int main(int argc, char* argv[])
{
	//initialize the variables	
	srand(time(0));
	char* cipherArray; 
	int arrayLength;
	char letter[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
	int randNum;
	

	if(argc == 2){
		//get the length of key file
		arrayLength = atoi(argv[1]);
		//initialize the key array
		cipherArray = (char *)malloc(arrayLength + 1 * sizeof(char));
		//create the key array by random number	
		for(int i = 0; i < arrayLength; i++){
			randNum = rand()% 27;
			cipherArray[i] = letter[randNum];
		}
		
		//add the null terminator at the end of key array
		cipherArray[arrayLength + 1] = '\0';
		//output the key array
		printf("%s\n", cipherArray);
	
		free(cipherArray);		

	}else{
		
		fprintf(stderr, "The format of command is wrong, please enter it again\n");
		exit(0);
	}
	

	return 0;
}
