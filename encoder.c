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

#define MAXSIZE 40


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
	bool isLe = true;
	
	struct meditrik* medPtr = malloc(sizeof(struct meditrik));
	if( (medPtr->verIN = (int) checkLine(file, "Version: ")) != 1){
		fprintf(stdout, "Unsupported version Expected version 1\n");
	} 																		// actually set the version in a header
	if( (medPtr->seqIN = (int) checkLine(file, "Sequence: ")) < 1 ){
		fprintf(stdout, "Bad Sequence number\n");
	}
	if( (medPtr->srcIN = (int) checkLine(file, "From: ")) < 1 ){
		fprintf(stdout, "Bad From\n");
	}
	if ( (medPtr->dstIN = (int) checkLine(file, "To: ")) < 1 ){
		fprintf(stdout, "Bad To\n");
	}
	if(isLe){
			medPtr->seqIN = ntohs(medPtr->seqIN);
			medPtr->srcIN = ntohl(medPtr->srcIN);
			medPtr->dstIN = ntohl(medPtr->dstIN);
	}
	
//	printHeader(medPtr->srcUC, 4);

	char array [MAXSIZE];
	fgets(array, 4, file); 		
	
	if( !strcmp(array, "GET") || !strcmp(array, "Glu") || !strcmp(array, "Cap") 
			|| !strcmp(array, "Omo")){
		setCommand(array, file);
	} else if( !strcmp( array, "Mes")){
		fgets(array, 7, file);
		setMessage(array);					// need to pass file pointer to get message
	} else if( !strcmp( array, "Lat")){
		struct gps* gpsPtr = malloc(sizeof(struct gps));
		
		gpsPtr->latiDB = checkLine(file, "itude: ");
		gpsPtr->longDB = checkLine(file, "Longitude: ");
		gpsPtr->altiDB = ((float) checkLine(file, "Altitude: "))/6;
		
		printHeader(medPtr->srcUC, 4, "check.txt");
		printHeader(medPtr->dstUC, 4, "check.txt");
		printHeader(gpsPtr->longUC, 8, "check.txt");
		printHeader(gpsPtr->latiUC, 8, "check.txt");
		printHeader(gpsPtr->altiUC, 4, "check.txt");
		printf("%f\n", gpsPtr->altiDB);
		//setGps(file);
	} else if( !strcmp( array, "Bat")){
		setStatus(file);
	} else printf("Invalid input\n");
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
