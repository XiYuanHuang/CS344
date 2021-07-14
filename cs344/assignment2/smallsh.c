  
/**********************************************************************
** * Program: smallsh
** * Author: XiYuan Huang
** * Date: July 21, 2020
** * Description: Create a low-level shell in c 
** * Input: Prompt the user to enter the commands
** * Output: Results depend on the user input
**********************************************************************/





#define _XOPEN_SOURCE 700 

/**********************************************************************
** * Included Libraries
**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>



/**********************************************************************
** * Prototypes
**********************************************************************/

void handle_SIGINT(int);
void handle_SIGTSTP(int);
char* addPID(char* , int);
void moveDirectory(char* [2048], int);
void showStatus(int );
int tokenInput(char* , char* [200] , char* [200], char* [2048], int);


/**********************************************************************
** * Global Variables
**********************************************************************/

int foreground = 1;


/**********************************************************************
** * Function: handle_SIGINT
** * Description:implement the ^C functionality
** * Parameters: int signal
** * Pre-Conditions: the signal has to be defined
** * Post-Conditions: catches all of the ^C signal and ignore it
**********************************************************************/
void handle_SIGINT(int signal) {				
	if(foreground == 1){
		printf("\n");
		fflush(stdout);
	}else{
		printf("\n");
	}
}


/***************************************************************************************
** * Function: handle_SIGTSTP
** * Description: implement the ^Z functionality to enter or exit the foreground-only mode
** * Parameters: int signal
** * Pre-Conditions: The signal has to be defined
** * Post-Conditions: Catches all of the ^Z signal and switch the foreground-only mode
****************************************************************************************/
void handle_SIGTSTP(int signal) {				
	if(foreground == 1) {
		foreground = 0;
		char* message = "\nEntering foreground-only mode (& is now ignored)\n";
		write(STDOUT_FILENO, message, 50);
		fflush(stdout);
	}
	else {
		foreground = 1;
		char* message = "\nExiting foreground-only mode\n";
		write(STDOUT_FILENO, message, 30);
		fflush(stdout);
	}
}


/********************************************************************************************************************
** * Function: tokenInput
** * Description: Break the user input to different types of data and assign them to the specific arrays
** * Parameters: char* tokenArray, char* inputFileName[200], char* outputFileName[200]
** *             char* commands[2048], int count
** * Pre-Conditions: all the parameters have to be defined
** * Post-Conditions: the user's input is separated and put into different arrays and return the number of commands
********************************************************************************************************************/
int tokenInput(char* tokenArray, char* inputFileName[200], char* outputFileName[200], char* commands[2048], int count)
{
	char temp[300];
	
	while(tokenArray != NULL)
		{
			if(strcmp(tokenArray, "<") == 0)
			{
				//capture and store the name of input file
				tokenArray = strtok(NULL, " ");
				sscanf(tokenArray, "%s", inputFileName);
				tokenArray = strtok(NULL, " ");

			}else if(strstr(tokenArray, "$$") != NULL){
				
				//transfer the "$$" into pid number
				sscanf(tokenArray, "%s", temp);
				commands[count] = strdup(addPID(temp, getpid()));
				count = count + 1;
				tokenArray = strtok(NULL, " ");

			}else if(strcmp(tokenArray, ">") == 0){
			
				//capture and store the name of output file
				tokenArray = strtok(NULL, " ");
				sscanf(tokenArray, "%s", outputFileName);
				tokenArray = strtok(NULL, " ");

			}else{
			
				//capture and store the commands of shell
				sscanf(tokenArray, "%s", temp);
				commands[count] = strdup(temp);
				count = count + 1;
				tokenArray = strtok(NULL, " ");

			}
		}
		return count;	

}


/**********************************************************************
** * Function: main
** * Description: performs the basic operations of the shell
** * Parameters: none
** * Pre-Conditions: none
** * Post-Conditions: the feedback depends on the user's command list
**********************************************************************/
int main()
{
	//Initialize the variables
	char input[2048];
	char* commands[2048];
	char* tokenArray;
	int doWhile = 1;
	int count  = 0;
	int background = 0;
	int status = 0;
	int userInput = 0;
	int userOutput = 0;
	pid_t spawnPID = -5;	

	char* inputFileName[200];
	char* outputFileName[200];
	char inputFile[200];
	char outputFile[200];

	//Initialize the variables of signal handlers
	struct sigaction signalAction1 = {0};
	struct sigaction signalAction2 = {0};

	// Fill out the SIGINT_action struct
 	// Register handle_SIGINT as the signal handler
	signalAction1.sa_handler = handle_SIGINT;		
	// No flags set
	signalAction1.sa_flags = 0;
	// Block all catchable signals while handle_SIGINT is running				
	sigfillset(&(signalAction1.sa_mask));
	// Install our signal handler
	sigaction(SIGINT, &signalAction1, NULL);
	

	// Fill out the SIGTSTP_action struct
 	// Register handle_SIGTSTP as the signal handler
	signalAction2.sa_handler = handle_SIGTSTP;
	// No flags set
	signalAction2.sa_flags = 0;
	// Block all catchable signals while handle_SIGTSTP is running	
	sigfillset(&(signalAction2.sa_mask));
	// Install our signal handler
	sigaction(SIGTSTP, &signalAction2, NULL);



	while(doWhile == 1)
	{
		printf(": ");
		fflush(stdout);
		fgets(input, 2048, stdin); //get the user's input
		tokenArray = strtok(input, " "); //start to break the input		

		count = 0;
		for(int i = 0; i < 200; i++){
			inputFileName[i] = 0;
			outputFileName[i] = 0;		
		}
		
		//separate the user input and store it into different array
		count = tokenInput(tokenArray, inputFileName, outputFileName, commands, count);
	
		//if user enters nothing, there is no command
		if(input[0] == '\n'){
			commands[0] = NULL;
		}	
		
		//check whether the command is a comment or nothing
		if(commands[0] != NULL && (*commands[0]) != '#'){

			//check whether the command needs to be run in the background mode
			if(strcmp(commands[count-1], "&") == 0){
				commands[count - 1] = NULL;
				background = 1;
			}else{
				commands[count++] = NULL;
				background = 0;
			}
			
			//check whether the command is the build-in commands
			if(strcmp(commands[0],"exit") == 0){
				doWhile = 0;
				printf("\n");
				exit(0);	
			}else if(strcmp(commands[0], "cd") == 0){
				moveDirectory(commands, count);
			}else if(strcmp(commands[0], "status") == 0){
				showStatus(status);
			}else{
				
				spawnPID= fork(); //parent process forks a child process
				switch(spawnPID)
				{
					case -1:
					{
						//if the child process is failed to build, print out the error message
						perror("fork() fail\n"); 
						exit(1);
							break;
					}
					case 0:
					{
						//set the SIGINT to the default
						if(background == 0 || foreground == 0){
							signalAction1.sa_handler = SIG_DFL;			
							sigaction(SIGINT, &signalAction1, NULL);

						}
						
						//check whether the input file array is NULL
						if(inputFileName[0] != 0){
							
							snprintf(inputFile, sizeof(inputFile), "%s", inputFileName);
							//establish the connection between a file and a file descriptor
							//and set the permission
							userInput = open(inputFile, O_RDONLY);
						
							//if the file cannot open, return error message
							if(userInput == -1)
							{
								printf("cannot open the file %s\n", inputFileName);
								fflush(stdout);
								_Exit(1);
							}
							
							//if the file does not exit, return error message
							if(dup2(userInput, 0) == -1)
							{
								perror("The input file name is not valid\n");
								fflush(stdout);
								_Exit(1);
							}
							close(userInput); //close the file
						}
						
						//check whether the input file array is NULL
						if(outputFileName[0] != 0){
							snprintf(outputFile, sizeof(outputFile), "%s", outputFileName);
							//establish the connection between a file and a file descriptor
							//and set the permission
							userOutput = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0777);
							
							//if the file cannot open, return error message
							if(userOutput == -1)
							{
								printf("cannot open the file %s\n", outputFileName);
								fflush(stdout);
								_Exit(1);
							}

							//if the file does not exit, return error message
							if(dup2(userOutput, 1) == -1)
							{
								perror("The output file name is not valid\n");
								fflush(stdout);
								_Exit(1);
							}
							close(userOutput); //close the file
						}
					
						//Use execvp() to perform other functionality
						if(execvp(commands[0],commands) == -1)
						{
							printf("The command %s is not valid\n", commands[0]);
							fflush(stdout);
							_Exit(1);
						}
						break;
					}
					default:
					{
						
						if(background == 0 || foreground == 0){
							waitpid(spawnPID, &status, 0); //the parent process has to wait the child process
						}else if(background == 1){
							//if the command run in the background mode, then print out the 
							//pid of background process
							printf("background pid is %d\n", spawnPID);
							fflush(stdout);
						}
						break;
					}
				}
			}
			
			//check if any zombie children process exist
			spawnPID = waitpid(-1, &status, WNOHANG);

			//shows the background process ID and status when the child process is terminated
			while(spawnPID > 0){
				printf("background process, %i, is done: ", spawnPID);
				fflush(stdout);
				showStatus(status);
				spawnPID = waitpid(-1, &status, WNOHANG);
			}			
		}	
			
		
	}
	
	return 0;
}



/**********************************************************************
** * Function: showStatus
** * Description: displays the status of the last foreground process
** * Parameters: int status
** * Pre-Conditions: the status defines and equals to zero
** * Post-Conditions: return the status of the last foreground process
**********************************************************************/
void showStatus(int status)
{

	if(WIFEXITED(status)){
		printf("exit value %i\n",WEXITSTATUS(status));
		fflush(stdout);	
	}else{
		printf("terminated by signal %i\n",status);
		fflush(stdout);
	}
}


/**********************************************************************
** * Function: moveDirectory
** * Description: depends on the user input to change the directory
** * Parameters: char* commands[2048], int count
** * Pre-Conditions: the user's input already store in the command array 
** *                 and the number of commands is provided
** * Post-Conditions: move to the directory that user request
**********************************************************************/
void moveDirectory(char* commands[2048], int count)
{
	if(count - 1 == 1)
	{
		chdir(getenv("HOME"));

	}else{
		chdir(commands[1]);
	}
}




/**********************************************************************
** * Function: addPID
** * Description: change the "$$" to the pid number
** * Parameters: char* temp, int pid
** * Pre-Conditions: a string with "$$" token
** * Post-Conditions: returns a string with the pid number added on the end
**********************************************************************/
char* addPID(char* temp, int pid)
{
	char pidArray[200];
	sprintf(pidArray, "%d", pid);
	int count = 0;
	int index = 0;
	int pidLength = strlen(pidArray);
	int i;	

	//check the "$$" token
	for(i = 0; temp[i] != '\0'; i++)
	{
		if(strstr(&temp[i], "$$") == &temp[i]){
			count = count + 1;
			i = i + 1;
		}
	}

	//allocate a dynamic array to reconstruct the string
	char* resultPid = malloc(i + count * (pidLength) + 1);

	
	//build the string again and replace the "$$" with pid number
	while(*temp)
	{
		if(strstr(temp, "$$") == temp){
			strcpy(&resultPid[index], pidArray);
			index = index + pidLength;
			temp = temp + 2;
		}else{
			resultPid[index++] = *temp++;
		}
	}	
	
	resultPid[i] = '\0';
	return resultPid;		
}
