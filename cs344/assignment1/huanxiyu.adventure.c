/*************************************************************************************************************
** * Program: huanxiyu.adventure
** * Author: XiYuan Huang
** * Date: July 6, 2020
** * Description: create an adventure game that allows user to find a path from the start room to the end room
** * Input: the room files which provide the info of different rooms
** * Output: the user finds the path between the beginning room and the end room and prints the step of history
****************************************************************************************************************/


/**********************************************************************
** * Included the library for building the room files
**********************************************************************/
#define  _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>


/**********************************************************************
** * Set up the structure for the room variable
**********************************************************************/
typedef struct Room {
	char roomName[10];
	char roomType;
	int connection[6];
	int numConnect;
}Room;



/**********************************************************************
** * Prototype of function
**********************************************************************/
char* openDirectory();
Room* createGraph(char*, Room* );
int checkRoom(Room*, char*);
int startRoom(Room* );
int checkConnect(Room* , char* , int );
void userHistory(Room* , int* , int );
void printConnection(Room* , int );


/***********************************************************************************
** * Function: openDiectory
** * Description: find out the directory that is opened last time
** * Parameters: No parameters are needed for this function
** * Pre-Conditions: none
** * Post-Conditions: find out the current directory
************************************************************************************/
char* openDirectory()
{
	struct dirent* folder;
	struct stat dirStatus;
	long currentTime = 0;
	char* currentDir;

	DIR* dir = opendir(".");
	
	while((folder = readdir(dir)) != NULL)
	{
		stat(folder->d_name, &dirStatus);
		if(folder->d_name[14] == '.' && dirStatus.st_mtime >= currentTime)
		{
			currentTime = dirStatus.st_mtime;
			currentDir = folder->d_name;
		}
	}
	closedir(dir);

	return currentDir;
}

/***********************************************************************************
** * Function: createGraph
** * Description: open the directory and get the information of each room
** * Parameters: char* fileDir, Room* allRoom
** * Pre-Conditions: the name of directory and the dynamic array of room
** * Post-Conditions: get the infomation of all rooms and store it into a room array
************************************************************************************/
Room* createGraph(char*  fileDir, Room* allRoom)
{
	//initialize the file variables
	struct dirent* folder;
	char dirPath[50];
	char filePath[50];
	int maxSize;
	char filebuffer[1000];
	char temp[50];
	int index = 0;
	FILE* openFile; 
	sprintf(dirPath, "./%s/", fileDir); //get the path of directory
	DIR* dir = opendir(dirPath); //open the directory
	
	while((folder = readdir(dir)) != NULL)
	{
		//looking for the room file
		if(folder->d_name[0] != '.')
		{
			sprintf(filePath, "%s%s", dirPath, folder->d_name); //create the file name
			openFile = fopen(filePath, "r");  //open the file
		
			while(fgets(filebuffer,1000,openFile) != NULL)
			{
				sscanf(filebuffer, "%*[^:]: %[^]%*[^\n]", temp); //take out the unnecessary information
			
				//put the information of room type and name into the room array
				if(filebuffer[5] == 'N')
				{
					allRoom[index].numConnect = 0;
					strcpy(allRoom[index].roomName, temp);
				}else if(filebuffer[5] == 'T'){
					allRoom[index].roomType = temp[0];
				}
			
			
			}
			fclose(openFile); //close the file
			index++;

		}


	}
	closedir(dir); //close the directory

	index = 0;
	dir = opendir(dirPath); //open the directory again
	
	while((folder = readdir(dir)) != NULL)
	{
		if(folder->d_name[0] != '.')
		{
			sprintf(filePath, "%s%s", dirPath, folder->d_name);
			openFile = fopen(filePath, "r");
			while(fgets(filebuffer,1000,openFile) != NULL)
			{
				sscanf(filebuffer, "%*[^:]: %[^]%*[^\n]", temp);
				
				//put the information(number of connection and connections) into the room array
				if(filebuffer[0] == 'C')
				{
					allRoom[index].connection[allRoom[index].numConnect] = checkRoom(allRoom,temp);
					allRoom[index].numConnect = allRoom[index].numConnect + 1;
				}
			}
			fclose(openFile); //close the file
			index++;
		}
	}
	closedir(dir); //close the directory
	
}


/***********************************************************************************
** * Function: checkRoom
** * Description: check whether the user input can match one of the rooms in the room array.
** * Parameters: char* temp, Room* allRoom
** * Pre-Conditions: the user input and the dynamic array of room
** * Post-Conditions: return the position of room or false
************************************************************************************/
int checkRoom(Room* allRoom, char* temp)
{
	for(int i = 0; i < 7; i++)
	{
		if(strcmp(allRoom[i].roomName, temp) == 0)
			return i; //the position of room
	}
	return 10; //false
}

/***********************************************************************************
** * Function: startRoom
** * Description: find out the start room
** * Parameters: Room* allRoom
** * Pre-Conditions: the dynamic array of room
** * Post-Conditions: return the position of room or false
************************************************************************************/
int startRoom(Room* allRoom)
{
	for(int i = 0; i < 7; i++)
	{
		if(allRoom[i].roomType == 'S')
			return i; //the position of the start room

	}
	return 10; //false	
}


/***********************************************************************************
** * Function: checkConnect
** * Description: check whether the current room has a connection with the user input
** * Parameters: Room* allRoom, char* temp, int currentRoom
** * Pre-Conditions: the dynamic array of room, the position of current room, the user input
** * Post-Conditions: return true or false
************************************************************************************/
int checkConnect(Room* allRoom, char* temp, int currentRoom)
{
	int goalRoom = checkRoom(allRoom, temp);
	for(int i = 0; i < allRoom[currentRoom].numConnect; i++)
	{
		if(allRoom[currentRoom].connection[i] == goalRoom)
			return 1;
	}
	return 10;
}


/***********************************************************************************
** * Function: game
** * Description: display the game menu and have interaction with the user
** * Parameters: none
** * Pre-Conditions: none
** * Post-Conditions: game menu
************************************************************************************/
void game()
{
	char* currentdir;
	int maxSize = 7;
	int currentRoom = 0;
	char dirPath[50];
	int winner = 0;
	int doAgain = 0;
	int userRecord[100];
	int count = 0;
	char buffer[100];


	currentdir = openDirectory(); //open the room file directory
	Room* allRoom = malloc(maxSize * sizeof(Room)); //initialize the room array
	createGraph(currentdir, allRoom);  //load the information of each room into the array
	

	currentRoom = startRoom(allRoom); //find out the start room
	while(winner == 0)
	{
		while(doAgain == 0){	
			//print out the current room and possible connections
			printf("CURRENT LOCATION: %s \n", allRoom[currentRoom].roomName);
			printf("POSSIBLE CONNECTIONS: ");
			printConnection(allRoom,currentRoom);

			//get the user input
			printf("WHERE TO? >");
      			scanf("%s", buffer);

			//check whether the user input is correct(sensitive case, possible connection)
			if(checkRoom(allRoom,buffer) == 10 || checkConnect(allRoom, buffer, currentRoom) == 10)
			{
				printf("HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n");

			}else{
				doAgain = 1;
				currentRoom = checkRoom(allRoom, buffer); //get the position of current room
				userRecord[count] = currentRoom; //add the current room into the history of user path
				count = count + 1;

			}
			printf("\n");
		}

		//if the user reaches the end room, break the loop
		if(allRoom[currentRoom].roomType == 'E')
		{
			//print out the winner message and the history of user path
			printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
			printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS: \n", count);
			userHistory(allRoom, userRecord,count);
			winner = 1;
			
		}else{
			doAgain = 0;
		}
	}
	free(allRoom); //free the memory of room array

}


/***********************************************************************************
** * Function: userHistory
** * Description: print out the history of user path
** * Parameters: Room* allRoom, int* path, int count
** * Pre-Conditions: the dynamic array of room, the int array of history, the numer of step
** * Post-Conditions: output the hsitory of user path in room name format
************************************************************************************/
void userHistory(Room* allRoom, int* path, int count)
{
	for(int i=0; i < count; i++)
	{
		printf("%s\n",allRoom[path[i]].roomName);
	}
}

/***********************************************************************************
** * Function: printConnection
** * Description: print out the possible connections of current room
** * Parameters: Room* allRoom, int currentRoom
** * Pre-Conditions: the dynmaic array of room. the position of current room
** * Post-Conditions: output the possible connection of current room
************************************************************************************/
void printConnection(Room* allRoom, int currentRoom)
{

	for(int i = 0; i < allRoom[currentRoom].numConnect; i++)
	{
		if(i == (allRoom[currentRoom].numConnect - 1))
		{
			printf("%s. ", allRoom[allRoom[currentRoom].connection[i]].roomName);
			
		}else{
			printf("%s, ", allRoom[allRoom[currentRoom].connection[i]].roomName);
		}
	}
	printf("\n");
}

/***************************************
** * main function
***************************************/
int main()
{
	game();

	return 0;
}
