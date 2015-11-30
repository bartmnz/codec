#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>


#include "meditrik.h"
#include "ethernetFrame.h"
#include "ipV4.h"
#include "udp.h"
#include "printHeader.h"

#define MAXSIZE 20


void setHeader(FILE*);

void setMessage(char *);
double checkLine(FILE*, const char*);
void setGps(FILE*);
void setCommand(char *, FILE*);
void setStatus(FILE*);


int main( void){
	FILE* file;
	if(!(file = fopen("test.txt", "r"))) exit(0);
	
	setHeader(file);
	char array [MAXSIZE];
	fgets(array, 4, file); 		
	
	if( !strcmp(array, "GET") || !strcmp(array, "Glu") || !strcmp(array, "Cap") 
			|| !strcmp(array, "Omo")){
		setCommand(array, file);
	} else if( !strcmp( array, "Mes")){
		fgets(array, 7, file);
		setMessage(array);					// need to pass file pointer to get message
	} else if( !strcmp( array, "Lat")){
		setGps(file);
	} else if( !strcmp( array, "Bat")){
		setStatus(file);
	} else printf("Invalid input\n");
	//fclose(file);
}
/*
void makeHeader(FILE* file){

}
*/
double checkLine(FILE* file, const char * text){
	char temp[MAXSIZE], *end, array[MAXSIZE], *num;
	fgets(array, sizeof(array), file);
	if (!((long int)array == (long int)strstr(array, text))){
		printf("Invalid line: expected %s \n", text);
		exit(0);
	}
	// need to implement more error checking to ensure that strings are not evaluated as a number, eg. allow for a zero value that is not abc
	
	strcpy(temp, array);
	strtok(array, ":");
	if(!strcmp(array, temp)){
			num = strtok(temp, "=");
	}
	num = strtok(NULL, " ");
	return strtod(num, &end);
}

void setHeader(FILE* file){
	
	if( checkLine(file, "Version: ") != 1){
		fprintf(stdout, "Unsupported version Expected version 1\n");
	} 																		// actually set the version in a header

	if( checkLine(file, "Sequence: ") < 1 ){
		fprintf(stdout, "Bad Sequence number\n");
	}
	if( checkLine(file, "From: ") < 1 ){
		fprintf(stdout, "Bad From\n");
	}
	if ( checkLine(file, "To: ") < 1 ){
		fprintf(stdout, "Bad To\n");
	}

	printf("valid line : doing stuff\n");
}



void setMessage(char * array){
	if(!(long int) &*array == (long int)strstr(array, "sage: ")){
		printf("Invalid Line: expected 		Message:\n");
		return;
	}
	printf("setting message\n");		//update to set message
}

void setGps(FILE* file){
	printf("Latitude = %f\n", checkLine(file, "itude: "));
	checkLine(file, "Longitude: ");
	checkLine(file, "Altitude: ");
	/*
	if(!(long int) &*array == (long int)strstr(array, "itude: ")){
		printf("Invalid Line: expected 		Latitude \n");
		return;
	}
	char theLine[MAXSIZE];
	fgets(theLine, sizeof(theLine), file);
	if(!((long int) theLine == (long int)strstr(theLine, "Longitude: "))){
		printf("Invalid line: expected 		Longitude \n");
		return;
	}
	fgets(theLine, sizeof(theLine), file);
	if(!((long int) theLine == (long int)strstr(theLine, "Altitude: "))){
		printf("Invalid line: expected 		Altitude \n");
		return;
	}
	*/
	printf("GPS Successful\n");
}

void setCommand(char * array, FILE* file){
	if( !strcmp(array, "GET")){
		printf("have a GET command\n");
	} else if( !strcmp( array, "Glu")){
		printf("Glucose setting is %ld\n", (long) checkLine(file, "cose="));
	} else printf("Invalid Line expected GET or SET\n");
	fclose(file);
}

void setStatus(FILE* file){
	checkLine(file, "tery: ");
	
	checkLine(file, "Glucose: ");
	
	checkLine(file, "Capsaicin: ");
	
	printf("Status Successful\n");
}
