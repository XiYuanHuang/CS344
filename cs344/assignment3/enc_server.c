
/**********************************************************************
** * Program: enc_server
** * Author: XiYuan Huang
** * Date: August 1, 2020
** * Description: The enc_server encrypts the message from the enc_client 
** * 		  and returns the encrypted message back to the client
** * Input: the server receives the message from the enc_client
** * Output: the server returns the encrypted message back to the client
**********************************************************************/


/**********************************************************************
** * Included Libraries
**********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>



/**********************************************************************
** * Prototypes
**********************************************************************/
void setupAddressStruct(struct sockaddr_in*, int);
char* encryptText(char* , char*);




/*******************************************************************************************************
** * Function: main
** * Description: This function performs all function of enc_server, which receives and encrypts the text
** * Parameters: int argc, char* argv[]
** * Pre-Conditions: The format of running command must be correct, and the plaintext
** *		     and key files have to be created before the enc_server run
** * Post-Conditions: The message will be encrypted and send back to the client
**********************************************************************************************************/
int main(int argc, char* argv[])
{

	//Initialize all the variables
	int connectionSocket = 0;
	int listenSocket = 0;
	int port = 0;
	int charsRead = 0;
	int plaintextLength = 0;
	int keyLength = 0;
	int status = 0;
	char buffer[100000] = "";
	struct sockaddr_in serverAddress;
	struct sockaddr_in clientAddress;
	socklen_t sizeOfClientInfo;
	int spawnPID = 0;	

	// Check usage & args
	if(argc != 2){
		fprintf(stderr, "The command is wrong!(USAGE: %s port) \n", argv[0]);
		exit(1);
	}

  	// Set up the address struct for the server socket
	setupAddressStruct(&serverAddress, atoi(argv[1]));

	// Create the socket that will listen for connections
	listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(listenSocket < 0){
		fprintf(stderr, "ERROR opening socket\n");
		exit(1);
	}
	

	// Associate the socket to the port
	if(bind(listenSocket, (struct sockaddr* )&serverAddress, sizeof(serverAddress)) < 0){
    		fprintf(stderr, "ERROR on binding\n");
		exit(1);
  	}

  	// Start listening for connetions. Allow up to 5 connections to queue up
 	listen(listenSocket, 5); 
  
  	// Accept a connection, blocking if one is not available until one connects
  	while(1){
 		char plaintext[100000] = "";
		char key[100000] = "";
		
    		sizeOfClientInfo = sizeof(clientAddress);
		// Accept the connection request which creates a connection socket
    		connectionSocket = accept(listenSocket, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); 
		if (connectionSocket < 0){
     			fprintf(stderr, "ERROR on accept\n");
			exit(1);
    		}
		
		//Create the child process
		spawnPID = fork();

		switch(spawnPID)
		{
			case -1:
			{
				fprintf(stderr, "fork() fail\n");
				exit(1);
				break;
			}

			case 0:
			{
				//clear the buffer array
				memset(buffer, '\0', 100000);
	    			// Read the client's message from the socket
	    			charsRead = recv(connectionSocket, buffer, 99999, 0); 
    				if (charsRead < 0){
      					fprintf(stderr, "ERROR reading from socket\n");
    					exit(1);
				}	

				//move the length of plaintext to the variable
				plaintextLength = atoi(buffer);
    				// Send a Success message back to the client
    				charsRead = send(connectionSocket, "encServer", 10, 0); 	
				if (charsRead < 0){
      					fprintf(stderr, "ERROR writing to socket\n");
					exit(1);
    				}

			

				//clear the buffer array
				memset(buffer, '\0', 100000);
    				// Read the client's message from the socket
    				charsRead = recv(connectionSocket, buffer, 99999, 0); 
    				if (charsRead < 0){
   	   				fprintf(stderr, "ERROR reading from socket\n");
    					exit(1);
				}
				//move the length of key to the variable
				keyLength = atoi(buffer);
    				//Send a Success message back to the client
    				charsRead = send(connectionSocket, "I am the server, and I got your keyLength", 39, 0); 			
    				if (charsRead < 0){
      					fprintf(stderr, "ERROR writing to socket\n");
					exit(1);
    					}
			


				//clear the buffer array
				memset(buffer, '\0', 100000);
    				// Read the client's message from the socket
    				charsRead = recv(connectionSocket, buffer, 99999, 0); 
    				if (charsRead < 0){
      					fprintf(stderr, "ERROR reading from socket\n");
    					exit(1);
				}
				//get the content of plaintext
				strcat(plaintext,buffer);
				//check whether all content of plaintext is sent
				plaintextLength = plaintextLength - strlen(buffer);
				//If the content of plaintext is not sent at once, then loop until the server obtain all the content
				while(plaintextLength != 0)
				{
					if(strlen(buffer) == 0){
						break;
					}else{
						memset(buffer, '\0', 100000);
						charsRead = recv(connectionSocket, buffer, 99999, 0);
					
						if(charsRead < 0){
		     					fprintf(stderr, "ERROR reading from socket\n");
    							exit(1);
						}
	
						strcat(plaintext, buffer);
						plaintextLength = plaintextLength - strlen(buffer);
					}
					
				}			
    				// Send a Success message back to the client
    				charsRead = send(connectionSocket, "I am the server, and I got your message", 39, 0); 			
    				if(charsRead < 0)
				{
      					fprintf(stderr, "ERROR writing to socket\n");
					exit(1);
    				}



				//clear the buffer array
				memset(buffer, '\0', 100000);
    				// Read the client's message from the socket
    				charsRead = recv(connectionSocket, buffer, 99999, 0); 
    				if (charsRead < 0){
      					fprintf(stderr, "ERROR reading from socket\n");
    					exit(1);
				}
				//get the content of key file
				strcat(key,buffer);
				//check whether all content of key file is sent
				keyLength = keyLength - strlen(buffer);
				//If the content of key file is not sent at once, then loop until the server obtain all the content
				while(keyLength != 0)
				{
					if(strlen(buffer) == 0){
						break;
					}else{
						memset(buffer, '\0', 100000);
						charsRead = recv(connectionSocket, buffer, 99999, 0);
					
						if(charsRead < 0){
		     					fprintf(stderr, "ERROR reading from socket\n");
    							exit(1);
						}

						strcat(key, buffer);
						keyLength = keyLength - strlen(buffer);
					}	
				
				}				
    				// Send a Success message back to the client
    				charsRead = send(connectionSocket, "I am the server, and I got your key", 39, 0); 			
    				if(charsRead < 0)
				{
      					fprintf(stderr, "ERROR writing to socket\n");
					exit(1);
    				}


	
				//Send the encrypted message to the client
				charsRead = send(connectionSocket, encryptText(plaintext, key), strlen(plaintext), 0); 			
    				if(charsRead < 0)
				{
      					fprintf(stderr, "ERROR writing to socket\n");
					exit(1);
    				}
				if(charsRead < strlen(plaintext))
				{
					printf("SERVER: Not all the data written to the socket!\n");
				}
			
			}
			default:
			{
				break;
			}

		}
		//close the connection
		close(connectionSocket);
		

	}
	//close the socket
	close(listenSocket);
	return 0;

}


/**********************************************************************
** * Function: encryptText
** * Description: encrypt the message
** * Parameters: char* message, char* key
** * Pre-Conditions: none
** * Post-Conditions: return the encrypted message
**********************************************************************/
char* encryptText(char* message, char* key)
{
	//Initialize the array variable
	char* letter = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
	char* encryptText = malloc(strlen(message) * sizeof(char));
	
	//compare the length of key and message
	if(strlen(key) < strlen(message)){
		fprintf(stderr, "the key is too short\n");
		exit(1);
	}

	//check whether the message contains the bad character
	for(int i = 0; i < strlen(message) - 1; i++){
		if((message[i] < 65 || message[i] > 90) && message[i] != ' '){
			fprintf(stderr, "error: input has bad character\n");
			exit(1);
		}
	}	

	//encrypt the content of message
	for(int i = 0; i < strlen(message) -1 ; i++){
			
		if(message[i] == ' ' && key[i] == ' '){
			encryptText[i] = ("%c", letter[((message[i] - 6) + (key[i] - 6)) % 27]);
		}else if(message[i] == ' ' && key[i] != ' '){
			encryptText[i] = ("%c", letter[((message[i] - 6) + (key[i] - 65)) % 27]);	
		}else if(message[i] != ' ' && key[i] == ' '){
			encryptText[i] = ("%c", letter[((message[i] - 65) + (key[i] - 6)) % 27]);
		}else{
			encryptText[i] = ("%c", letter[((message[i] - 65) + (key[i] - 65)) % 27]);	
		}	
	}

	//add the newline and null at the end of encrypted text
	encryptText[strlen(message) - 1] = '\n';	
	encryptText[strlen(message)] = '\0';
	
	return encryptText;
}


/**********************************************************************
** * Function: setupAddressStruct
** * Description: Set up the address struct for the server socket
** * Parameters: struct sockaddr_in* address, int portNumber
** * Pre-Conditions: none
** * Post-Conditions: the address struct is initialized 
**********************************************************************/
void setupAddressStruct(struct sockaddr_in* address, int portNumber)
{
 
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);
  // Allow a client at any address to connect to this server
  address->sin_addr.s_addr = INADDR_ANY;
}

