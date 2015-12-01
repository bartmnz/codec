#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>


#include "meditrik.h"
//#include "ethernetFrame.h"
//#include "ipV4.h"
//#include "udp.h"
//#include "printHeader.h"

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
	
	fclose(file);
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
	
	struct frame* frmPtr = malloc(sizeof(struct frame));
	if( (frmPtr->medPtr.verIN = (int) checkLine(file, "Version: ")) != 1){
		fprintf(stdout, "Unsupported version Expected version 1\n");
	} 																		// actually set the version in a header
	if( (frmPtr->medPtr.seqIN = (int) checkLine(file, "Sequence: ")) < 1 ){
		fprintf(stdout, "Bad Sequence number\n");
	}
	if( (frmPtr->medPtr.srcIN = (int) checkLine(file, "From: ")) < 1 ){
		fprintf(stdout, "Bad From\n");
	}
	if ( (frmPtr->medPtr.dstIN = (int) checkLine(file, "To: ")) < 1 ){
		fprintf(stdout, "Bad To\n");
	}
	if(isLe){
			frmPtr->medPtr.seqIN = ntohs(frmPtr->medPtr.seqIN);
			frmPtr->medPtr.srcIN = ntohl(frmPtr->medPtr.srcIN);
			frmPtr->medPtr.dstIN = ntohl(frmPtr->medPtr.dstIN);
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
		
		frmPtr->gpsPtr.latiDB = checkLine(file, "itude: ");
		frmPtr->gpsPtr.longDB = checkLine(file, "Longitude: ");
		frmPtr->gpsPtr.altiDB = ((float) checkLine(file, "Altitude: "))/6;
		frmPtr->medPtr.typeIN = 2; // need this here
		frmPtr->medPtr.lenIN = htonl(32); // assign other lengths here as well
		frmPtr->ipPtr.lenIN = 60;		//need this here
		frmPtr->ipPtr.nxpSH = 17;
		frmPtr->ipPtr.verSH = 4;
		frmPtr->ipPtr.hlenSH = 5;
		frmPtr->ipPtr.tosSH = 170;
		frmPtr->ipPtr.idIN = 56797;		//dddd
		frmPtr->ipPtr.flagSH = 1;
		frmPtr->ipPtr.foffIN = htons(1);
		frmPtr->ipPtr.ttlSH = 255;		//ff
		frmPtr->ipPtr.chkIN = 52428; 	// cccc
		frmPtr->ipPtr.srcLN = htonl(3735928559); 	//DEAD BEEF
		frmPtr->ipPtr.dstLN = htonl(48879); 	//BEEF
		frmPtr->udpPtr.srcSH = 4369;
		frmPtr->udpPtr.dstSH = 8738;
		frmPtr->udpPtr.lenSH = 40;		// need this here
		frmPtr->udpPtr.chkSH = 17476;
		unsigned short temp[3] = {43690,43690,43690};
		memcpy( frmPtr->ethPtr.srcIN, temp, 6);
		unsigned short to[3] = {65535,65535,65535};
		memcpy( frmPtr->ethPtr.dstIN, to, 6);
		frmPtr->ethPtr.nxtIN = 52428;
		
		//frmPtr->ethPtr.srcIN = 733007751850;
		
		
		printMeditrik(frmPtr, "check.txt");

		//setGps(file);
	} else if( !strcmp( array, "Bat")){
		setStatus(file);
	} else printf("Invalid input\n");
	printf("valid line : doing stuff\n");
	free(frmPtr);
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
