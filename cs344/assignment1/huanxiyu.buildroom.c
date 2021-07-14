/********************************************************************************************************
** * Program: huanxiyu.buildroom
** * Author: XiYuan Huang
** * Date: July 6, 2020
** * Description:Create multiple room files and store in a directory for the adventure game 
** * Input: none
** * Output: multiple room files in a room directory
**********************************************************************************************************/



/**********************************************************************
** * Included the library for building the room files
**********************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>


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
Room* createRoom();
void roomName(Room*, int*);
void roomType(Room*);
void randomSelect(int*);
void swap(int*,int*);
int IsGraphFull(Room*);
int checkAddConnect(Room);
int checkConnectExists(Room, int);
void roomConnection(Room*);
void createDirectory(Room*);
void writeInDirectory(Room*, char*);





/***********************************************************************************
** * Function: createRoom
** * Description: creates different room files and stores in a directory for the game
** * Parameters: No parameters are needed for this function
** * Pre-Conditions: none
** * Post-Conditions: outputs multiples room files in a room directory
************************************************************************************/
Room* createRoom()
{
	srand(time(0));
	int maxRoom = 7;
	Room* allRoom = malloc(maxRoom * sizeof(Room)); // allocate the memory for all rooms
	int* selectName = malloc(maxRoom *sizeof(int)); //allocate the memory for selecting the name of rooms
 

	//initialize the array of room name
	for(int i = 0; i < 7; i++)
	{
		selectName[i] = i;

	}
	
	//assign the random number to decide the name of room
	randomSelect(selectName);


	//initialize the number of room and connection variables to avoid the garbage data
	for(int i = 0; i < 7; i++)
	{
		
		allRoom[i].numConnect = 0;
		for(int j = 0; j < 7; j++)
		{
			allRoom[i].connection[j] = 0;
		}
	}

	roomName(allRoom, selectName); //assign the name of room
	roomType(allRoom); 	//assign the type of room
	roomConnection(allRoom);  //build the connection between different rooms
	createDirectory(allRoom); //store the roomfile in a directory


	//free the memory of array 
	free(allRoom);  
	free(selectName);
}




/***********************************************************************************
** * Function: createDiectory
** * Description: creates a directory for storing the room files
** * Parameters: Room* allRoom
** * Pre-Conditions: require the dynamic array of room
** * Post-Conditions: output an empty directory
************************************************************************************/
void createDirectory(Room* allRoom)
{
	srand(time(0));
	char dirName[100] = "huanxiyu.rooms."; //create the name of directory
	int randID = rand()%100001; //set up the directory ID
	char randNum[30]; 
	
	sprintf(randNum, "%d", randID); //transfer the directory ID to char
	strcat(dirName, randNum); //put the name and the directory ID together
	int check = mkdir(dirName,0777); //create a directory
	
	//check whether the directory is created
	if(!check)
	{
		printf("Directory created\n");
	}else{
		printf("unable to create directory\n");
	}
	
	writeInDirectory(allRoom, dirName); //write the room files into the directory
}



/***********************************************************************************
** * Function: writeInDiectory
** * Description: writes the room files into an empty directory
** * Parameters: Room* allRoom, char* dirName
** * Pre-Conditions: require the dynamic array of room and the directory name
** * Post-Conditions: output seven room files into a empty directory
************************************************************************************/
void writeInDirectory(Room* allRoom, char* dirName)
{
	FILE* roomFiles; //create the file variable	
	char fileName[50];  //create the file name variable	
	
	DIR* dir = opendir(dirName); //open the directory



	for(int i = 0; i < 7; i++)
	{
		sprintf(fileName, "./%s/%s_room", dirName, allRoom[i].roomName); //set up the name for each room file
		roomFiles = fopen(fileName, "w+"); //open the room file

		//start to write in the information of room
		fprintf(roomFiles, "ROOM NAME: %s\n", allRoom[i].roomName); 
		for(int j = 0; j < allRoom[i].numConnect; j++)
		{
			fprintf(roomFiles, "CONNECTION %d: %s\n",j+1, allRoom[allRoom[i].connection[j]].roomName);
			
		}
		
		
		if(allRoom[i].roomType == 's')
		{
			fprintf(roomFiles, "ROOM TYPE: START_ROOM\n");
		}else if(allRoom[i].roomType == 'e'){
			fprintf(roomFiles, "ROOM TYPE: END_ROOM\n");
		}else if(allRoom[i].roomType == 'm'){
			fprintf(roomFiles, "ROOM TYPE: MID_ROOM\n");
		
		}

		fclose(roomFiles); //close the file

	}
	
	closedir(dir); //close the directory


}

/***********************************************************************************
** * Function: IsGraphFull
** * Description: check whether the number of connections is enough
** * Parameters: Room* allRoom
** * Pre-Conditions: require the dynamic array of room
** * Post-Conditions: output the true or false 
************************************************************************************/
int IsGraphFull(Room* allRoom)
{
	for(int i = 0; i < 7; i++)
	{
		if(allRoom[i].numConnect < 3)
		{
			return 0;
		}
	}
	return 1;
}

/***********************************************************************************
** * Function: checkConnectExists
** * Description: check whether the connected room is the same
** * Parameters: Room* allRoom, int randNum
** * Pre-Conditions: require the dynamic array of room and the connected room
** * Post-Conditions: output the true or false 
************************************************************************************/
int checkConnectExists(Room allRoom, int randNum)
{
	for(int i = 0; i < allRoom.numConnect; i++)
	{
		if(allRoom.connection[i] == randNum)
		{
			return 1;
		}
	}
	return 0;
}


/***********************************************************************************
** * Function: checkAddConnect
** * Description: check whether the number of connection reaches the max 
** * Parameters: Room* allRoom
** * Pre-Conditions: require the dynamic array of room
** * Post-Conditions: output the true or false 
************************************************************************************/
int checkAddConnect(Room allRoom)
{
	if(allRoom.numConnect>= 6)
	{
		return 1;
	}
	return 0;
}




/***********************************************************************************
** * Function: roomConnection
** * Description: create the connection between different rooms
** * Parameters: Room* allRoom
** * Pre-Conditions: require the dynamic array of room
** * Post-Conditions: each room will have three to six connections
************************************************************************************/
void roomConnection(Room* allRoom)
{
	int randNum = 0;
	while(IsGraphFull(allRoom) == 0)
	{
		for(int i = 0; i < 7; i++){
			randNum = rand()%7; //decide the connected room
			while(checkConnectExists(allRoom[i],randNum) == 1 || randNum == i || checkAddConnect(allRoom[randNum]) == 1)
			{
				//if the number of connection reaches the max,break the loop
				if(allRoom[i].numConnect >= 6) 
				{
					break;
				}
				randNum = rand()%7; //pick up another connected room	
			}
			
			//build up the connection between the original room and selected room
			allRoom[i].connection[allRoom[i].numConnect] = randNum;	
			allRoom[randNum].connection[allRoom[randNum].numConnect] = i;
			allRoom[i].numConnect += 1;
			allRoom[randNum].numConnect += 1;
			
			//if all room have enough connection, break the loop
			if(IsGraphFull(allRoom) == 1) 
			{
				break;
			}
		}	
		
	}
}


/***********************************************************************************
** * Function: roomType
** * Description: assign the room type to each room
** * Parameters: Room* allRoom
** * Pre-Conditions: require the dynamic array of room
** * Post-Conditions: each room will have its own type
************************************************************************************/
void roomType(Room* allRoom)
{

	int randNum1 = 0;
	int randNum2 = 0;
	int count = 0;
	int temp1 = 0;
	int temp2 = 0;
	
	//select the start room
	randNum1 = rand()%6; 
	allRoom[randNum1].roomType = 's';
	temp1 = randNum1;
	
	//select the end room
	randNum2 = rand()%6;
	while(randNum2 == temp1)
	{
		randNum2 = rand()%6;

	}
	allRoom[randNum2].roomType = 'e';


	//the rest of rooms will become middle room
	for(int i = 0; i < 7; i++)
	{
		if(allRoom[i].roomType != 's' && allRoom[i].roomType != 'e')
		{
			allRoom[i].roomType = 'm';
		}
	}



}

/***********************************************************************************
** * Function: swap
** * Description: switch the value of two variables
** * Parameters: int *a, int* b
** * Pre-Conditions: need the value of two variables
** * Post-Conditions: switch the value of two variables
************************************************************************************/
void swap(int *a, int *b)  
{  
    int temp = *a;  
    *a = *b;  
    *b = temp;  
}  


/***********************************************************************************
** * Function: randomSelect
** * Description: use the random number to choose the name of room
** * Parameters: int *selectName
** * Pre-Conditions: the integer array
** * Post-Conditions: output an int array with random number
************************************************************************************/
void randomSelect(int* selectName)
{
	int select = 0;
	int flag = 0;	
	int count = 0;

	for(int i = 6; i > 0; i--)
	{
		select = rand()%(i+1);
		swap(&selectName[i],&selectName[select]);
		
	}
	
	
}

/***********************************************************************************
** * Function: randomName
** * Description: assign the name to each room
** * Parameters: Room* allRoom, int* selectName
** * Pre-Conditions: the dynamic array of all room and the int array of random number
** * Post-Conditions: each room will have an unique name
************************************************************************************/
void roomName(Room* allRoom,int* selectName)
{
	for(int i = 0; i < 7; i++){		
		switch(selectName[i])
		{
			case 0:
			strcpy(allRoom[i].roomName, "twisty");
			break;
			case 1: 
			strcpy(allRoom[i].roomName, "PLOVER");
			break;
			case 2:
			strcpy(allRoom[i].roomName, "ZORK");
			break;
			case 3:
			strcpy(allRoom[i].roomName, "XYZZY");
			break;
			case 4: 
			strcpy(allRoom[i].roomName, "PLUGH");
			break;
			case 5: 
			strcpy(allRoom[i].roomName, "CROWTHER");
			break;
			case 6: 
			strcpy(allRoom[i].roomName, "DUNGEON");
	 		break;
			default:
			strcpy(allRoom[i].roomName, "NULL"); 
		}
	}
}

/*********************************************
** * main function
**********************************************/
int main()
{
	srand(time(0)); 
	createRoom();
	return 0;
	
}
