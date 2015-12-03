#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>


#include "meditrik.h"
#define MAXSIZE 40


void setHeader(FILE*);
void setMessage(FILE*, struct frame*);
double checkLine(FILE*, const char*);
void setGps(FILE*, struct frame*);
void setCommand( FILE*, struct frame*);
void setStatus(FILE*, struct frame*);
void setDefaults(struct frame*);
void setLens(struct frame*, int);
void setLocal(struct frame*);
void setGlobal(const char*);

int main( void){
	FILE* file;
	if(!(file = fopen("test.txt", "r"))) exit(0);
	
	long position = ftell(file); 
	char temp;
	setGlobal("check.txt");
	while( (temp = fgetc(file)) != EOF){
		fseek(file, position, SEEK_SET);
		setHeader(file);
		position = ftell(file);
	}
	fclose(file);
}
/*
void makeHeader(FILE* file){

}
*/
double checkLine(FILE* file, const char * text){
	char temp[MAXSIZE], *end, array[MAXSIZE], *num;
	if(fgets(array, sizeof(array), file) == NULL){
		printf("ERROR: nothing to read!!");
		exit(0);
	}
	if (!((long int)array == (long int)strstr(array, text))){
		printf("Invalid line: expected %s HAVE %s \n", text, array);
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
	

	char array [MAXSIZE];
	fgets(array, 4, file); 		
	
	if( !strcmp(array, "GET") || !strcmp(array, "Glu") || !strcmp(array, "Cap") 
			|| !strcmp(array, "Omo")){
		setCommand( file, frmPtr);
	} else if( !strcmp( array, "Mes")){
		setMessage(file, frmPtr);					// need to pass file pointer to get message
	} else if( !strcmp( array, "Lat")){
		setGps(file, frmPtr);
	} else if( !strcmp( array, "Bat")){
		setStatus(file, frmPtr);
	} else {
		printf("Invalid input\n");
		exit(0);
	}
	printf("valid line : doing stuff\n");
	setLocal(frmPtr);
	printMeditrik(frmPtr, "check.txt");
	free(frmPtr);
}



void setMessage(FILE* file, struct frame* frmPtr){
	char array[7];
	frmPtr->msgPtr = malloc(1478);
	fgets(array, sizeof(array), file);
	if(!((long int) &*array == (long int)strstr(array, "sage: "))){
		printf("Invalid Line: expected Message: \n Have: %s\n", array);
		return;
	}
	long position = ftell(file);
	int temp, count = 0;
	while( (temp = fgetc(file)) != '\0' && temp != EOF){ 
		count++;
		if( count == 1478){
			fprintf(stderr, "ERROR: message is too large, max size is 1477");
			exit(0);
		}
	}
	fseek(file, position, SEEK_SET);
	frmPtr->msgPtr->len = count;
	fread(frmPtr->msgPtr->message, 1, count, file);
	fgets(array, sizeof(array), file); // clear out anything left inthe line
	printf("%d\n", frmPtr->msgPtr->len);
	frmPtr->medPtr.typeIN = 3;
	setLens(frmPtr, 12 + frmPtr->msgPtr->len);
	setDefaults(frmPtr);
}

void setGps(FILE* file, struct frame* frmPtr){
	frmPtr->gpsPtr.latiDB = checkLine(file, "itude: ");
	frmPtr->gpsPtr.longDB = checkLine(file, "Longitude: ");
	frmPtr->gpsPtr.altiDB = ((float) checkLine(file, "Altitude: "))/6;
	frmPtr->medPtr.typeIN = 2;
	setLens(frmPtr, 32);
	setDefaults(frmPtr);
}

void setLens( struct frame* frmPtr, int len){

	frmPtr->medPtr.lenIN = htons(len); // assign other lengths here as well
	frmPtr->ipPtr.lenIN = htons(len + 28);		//need this here
	frmPtr->udpPtr.lenSH = htons(len + 8);
}

void setCommand( FILE* file, struct frame* frmPtr){
	char array [MAXSIZE];
	fgets(array, 4, file);
	bool hasPara = false;
	if (!strcmp(array, " ST")){ // GET STATUS
		frmPtr->cmdPtr.comIN = htons(1);
		fgets(array, MAXSIZE, file);
	} else	if(!strcmp(array, "cos")){ // Glucose
		frmPtr->cmdPtr.comIN = htons(2);
		printf("%s\n", array);
		frmPtr->cmdPtr.parIN = htons(checkLine(file, "e="));
		hasPara = true;
	} else if (!strcmp(array, " GP")){ // GET GPS
		frmPtr->cmdPtr.comIN = htons(3);
	} else if(!strcmp(array, "sai")){ // Capsaicin
		frmPtr->cmdPtr.comIN = htons(4);
		frmPtr->cmdPtr.parIN = htons(checkLine(file, "cin="));
		hasPara = true;
	} // reserved for future use
	/*else if (!strcmp(array, " ")){ // 
		frmPtr->cmdPtr.comIN = htons(4);
	}*/ 
	else if(!strcmp(array, "rfi")){ // Omorfine
		frmPtr->cmdPtr.comIN = htons(5);
		frmPtr->cmdPtr.parIN = checkLine(file, "ne=");
		hasPara = true;
	} // reserved for future use
	/*else if (!strcmp(array, " ")){ // 
		frmPtr->cmdPtr.comIN = htons(6);
	}*/ 
	else if(!strcmp(array, "EAT")){ // REPEAT
		frmPtr->cmdPtr.comIN = htons(7);
		frmPtr->cmdPtr.parIN = checkLine(file, "=");
		hasPara = true;
	}
	
	frmPtr->medPtr.typeIN = 1; // need this here
	if (hasPara){
		setLens(frmPtr, 16);
	} else {
		setLens(frmPtr, 14);
	}
	setDefaults(frmPtr);
}

void setStatus(FILE* file, struct frame* frmPtr){
	frmPtr->stsPtr.batDB = checkLine(file, "tery: ");
	printf("%f\n", frmPtr->stsPtr.batDB );
	frmPtr->stsPtr.gluIN = htons(checkLine(file, "Glucose: "));
	
	frmPtr->stsPtr.capIN = htons(checkLine(file, "Capsaicin: "));
	
	frmPtr->stsPtr.omoIN = htons(checkLine(file, "Omorfine: "));
	printHeader(frmPtr->stsPtr.batUC, 8);
	setLens(frmPtr, 28);
	setDefaults(frmPtr);
	
	printf("Status Successful\n");
}

void setDefaults(struct frame* frmPtr){
	
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
	frmPtr->udpPtr.chkSH = 17476;
	unsigned short temp[3] = {43690,43690,43690};
	memcpy( frmPtr->ethPtr.srcIN, temp, 6);
	unsigned short to[3] = {65535,65535,65535};
	memcpy( frmPtr->ethPtr.dstIN, to, 6);
	frmPtr->ethPtr.nxtIN = 52428;
	
}

void setLocal( struct frame* frmPtr){
	frmPtr->locPtr.lenIN = ntohs(frmPtr->ipPtr.lenIN) + 14;
//	printf("%d\n", frmPtr->locPtr.lenIN);
//	printf("%d\n", ntohs(frmPtr->ipPtr.lenIN));
	frmPtr->locPtr.nxtLenIN = frmPtr->locPtr.lenIN;
}

void setGlobal( const char* fileName){
	FILE* file;
	if(! (file = fopen(fileName, "ab"))){
		exit(0);
	};
	struct globalHeader global = {.magicIN = 2712847316, .magVerSH = 2, .minVerSH = 4};
	fwrite( &global, 1, 24, file );
	fclose(file);
}