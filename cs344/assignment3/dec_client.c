
/**********************************************************************
** * Program: dec_client 
** * Author: XiYuan Huang
** * Date: August 1, 2020
** * Description: The dec_client sent the content of cipherext and 
** * 		  key file to the dec_server and gain the decrypted file.
** * Input: the ciphertext and key file from the command line
** * Output: the decrpted message from the dec_server
**********************************************************************/


/**********************************************************************
** * Included Libraries
**********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


/**********************************************************************
** * Prototypes
**********************************************************************/
void error(const char*);
void setupAddressStruct(struct sockaddr_in*, int);




/**********************************************************************
** * Function: error
** * Description: Prints out an error message
** * Parameters: const char* msg
** * Pre-Conditions: none
** * Post-Conditions: output the error message and set the status
** *		      of exit to 1
**********************************************************************/
void error(const char *msg) { 
	perror(msg); 
	exit(1); 
} 


/**********************************************************************
** * Function: setupAddressStruct
** * Description: Set up the address struct for the client socket
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
	
	// Get the DNS entry for this host name
	struct hostent* hostInfo = gethostbyname("localhost"); 
	if (hostInfo == NULL) { 
		fprintf(stderr, "CLIENT: ERROR, no such host\n"); 
		exit(1); 
	}
  
	// Copy the first IP address from the DNS entry to sin_addr.s_addr
	memcpy((char*) &address->sin_addr.s_addr, hostInfo->h_addr_list[0], hostInfo->h_length);
}


/*******************************************************************************************************
** * Function: main
** * Description: This function performs all function of dec_client, which obtains the content of ciphertext
**		  and receives the decrypted message from the dec_server
** * Parameters: int argc, char* argv[]
** * Pre-Conditions: The format of running command must be correct, and the ciphertext
** *		     and key files have to be created before the enc_server run
** * Post-Conditions: Output the decrypted message
**********************************************************************************************************/
int main(int argc, char *argv[])
{
	//Initialize all the variables
	int socketFD, portNumber, charsWritten, charsRead;
	long keyLength, plaintextLength;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[100000];
	char keyLen[10000];
	char plaintextLen[10000];
	char plaintext[100000];
	char key[100000];
	FILE* keyFile;
	FILE* plaintextFile;
	FILE* keySize;
	FILE* plaintextSize;
	
	// Check usage & args
	if (argc != 4) { 
		fprintf(stderr,"USAGE: %s plaintext key port\n", argv[0]); 
  		exit(1); 
	} 



	// Set up the server address struct
  	setupAddressStruct(&serverAddress, atoi(argv[3]));
	
  	// Create a socket
  	socketFD = socket(AF_INET, SOCK_STREAM, 0); 
  	if (socketFD < 0){
    		error("CLIENT: ERROR opening socket\n");
  	}

  	// Connect to server
  	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
    		error("CLIENT: ERROR connecting\n");
  	}

	//open the ciphertext file
	plaintextFile = fopen(argv[1], "r+");
	if(plaintextFile == NULL){
		error("Failed to open the plaintext file\n");
	}
	//get the content of ciphertext
	fgets(plaintext, 100000, plaintextFile);
	fclose(plaintextFile);
	
	//open the key file
	keyFile = fopen(argv[2], "r+");
	if(keyFile == NULL){
		error("Failed to open the key file\n");
	}
	//get the content of key file
	fgets(key, 100000, keyFile);
	fclose(keyFile);

	
	//open the ciphertext file
	plaintextSize = fopen(argv[1], "r+");
	//locate the end of the ciphertext file
	fseek(plaintextSize, 0, SEEK_END);
	//get the length of ciphertext
	plaintextLength = ftell(plaintextSize);
	//close the ciphertext file
	fclose(plaintextSize);

	//open the key file
	keySize = fopen(argv[2], "r+");
	//locate the end of the key file
	fseek(keySize, 0, SEEK_END);
	//get the length of key file
	keyLength = ftell(keySize);
	//close the key file
	fclose(keySize);

	//convert the data type of length of ciphertext and key file
	sprintf(plaintextLen, "%d", plaintextLength);
	sprintf(keyLen, "%d", keyLength);
	
	//send the length of ciphertext to the dec_server
  	charsWritten = send(socketFD, plaintextLen, strlen(plaintextLen), 0); 
  	if (charsWritten < 0){
   		 error("CLIENT: ERROR writing to socket\n");
  	}
  	if (charsWritten < strlen(plaintextLen)){
		printf("CLIENT: WARNING: Not all data written to socket!\n");
  	}
  	memset(buffer, '\0', 100000);
	//get the infomation from the dec_server
  	charsRead = recv(socketFD, buffer, 99999, 0);
  	if (charsRead < 0){
    		error("CLIENT: ERROR reading from socket\n");
  	}
	//check whether the dec_client connects to the right server
	if(strcmp(buffer, "decServer") != 0){
		fprintf(stderr, "Error: the server is not correct\n");
		exit(2);
	}



	//send the length of key file to the dec_server
  	charsWritten = send(socketFD, keyLen, strlen(keyLen), 0); 
  	if (charsWritten < 0){
   		 error("CLIENT: ERROR writing to socket\n");
  	}
  	if (charsWritten < strlen(keyLen)){
		printf("CLIENT: WARNING: Not all data written to socket!\n");
  	}
  	memset(buffer, '\0', 100000);
  	charsRead = recv(socketFD, buffer, 99999, 0); 
  	if (charsRead < 0){
    		error("CLIENT: ERROR reading from socket\n");
  	}



	//send the content of ciphertext to the dec_server
  	charsWritten = send(socketFD, plaintext, strlen(plaintext), 0); 
  	if (charsWritten < 0){
   		 error("CLIENT: ERROR writing to socket\n");
  	}
  	if (charsWritten < strlen(plaintext)){
		printf("CLIENT: WARNING: Not all data written to socket!\n");
  	}
  	memset(buffer, '\0', 100000);
  	charsRead = recv(socketFD, buffer, 99999, 0); 
  	if (charsRead < 0){
    		error("CLIENT: ERROR reading from socket\n");
  	}



	//send the content of key file to the dec_server
  	charsWritten = send(socketFD, key, strlen(key), 0); 
  	if (charsWritten < 0){
   		 error("CLIENT: ERROR writing to socket\n");
  	}
  	if (charsWritten < strlen(key)){
		printf("CLIENT: WARNING: Not all data written to socket!\n");
  	}
  	memset(buffer, '\0', 100000);
  	charsRead = recv(socketFD, buffer, 99999, 0); 
  	if (charsRead < 0){
    		error("CLIENT: ERROR reading from socket\n");
  	}



  	memset(buffer, '\0', 100000);
	//get the decrypted message from the dec_server
  	charsRead = recv(socketFD, buffer, 99999, 0); 
  	if (charsRead < 0){
    		error("CLIENT: ERROR reading from socket\n");
  	}
  	printf("%s", buffer);
	//check whether all decrypted message is received
	plaintextLength = plaintextLength - strlen(buffer);
	//If the decrypted message is not sent at once, then loop until the client obtain all the decrypted message
	while(plaintextLength != 0)
	{
		if(strlen(buffer) == 0){
			break;
		}else{
			memset(buffer, '\0', 100000);
			charsRead = recv(socketFD, buffer, 99999, 0);		
			if(charsRead < 0){
			  	fprintf(stderr, "ERROR reading from socket\n");
    				exit(1);
			}

			plaintextLength = plaintextLength - strlen(buffer);
			printf("%s", buffer);
		}
				
	}	
	// Close the socket
  	close(socketFD); 
  	return 0;
}
