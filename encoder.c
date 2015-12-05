#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>


#include "meditrik.h"
#define MAXSIZE 40


int setHeader(FILE*, struct frame*);
int setMessage(FILE*, struct frame*);
double checkLine(FILE*, const char*);
int setGps(FILE*, struct frame*);
int setCommand( FILE*, struct frame*);
int setStatus(FILE*, struct frame*);
void setDefaults(struct frame*);
void setLens(struct frame*, int);
void setLocal(struct frame*);
void setGlobal(const char*);

int main( int argc, const char* argv[]){
	FILE* file;
	if(argc != 3){ // set errno goto problems
		fprintf(stderr,"Usage = encoder (FILE TO READ) (FILE TO WRITE)\n");
		exit(0);
	};
	if(!(file = fopen(argv[1], "r"))){
		fprintf(stderr,"ERROR: could not open file\n");
		exit(0);
	}
	
	long position = ftell(file); 
	char temp;
	setGlobal(argv[2]);
	
	struct frame* frmPtr = malloc(sizeof(struct frame));
	frmPtr->msgPtr = malloc(1478);
	
	while( (temp = fgetc(file)) != EOF){
		memset(frmPtr, 0, sizeof(struct frame)- sizeof(void*));
		memset(frmPtr->msgPtr, 0, 1478);
		if (temp != '\n'){
			fseek(file, position, SEEK_SET);
		} 
		if(!setHeader(file, frmPtr)){
			printMeditrik(frmPtr, argv[2]);
			position = ftell(file);
		} else break;
	}
	free(frmPtr->msgPtr);
	free(frmPtr);
	fclose(file);
}
/*	function compares the first line of file to string text. if a match is 
	found it will return the number following the match, else -1 or -2 will be returned as errors.
*/

double checkLine(FILE* file, const char * text){
	char temp[MAXSIZE], *end, array[MAXSIZE], *num;
	if(fgets(array, sizeof(array), file) == NULL){
		fprintf(stderr, "ERROR: nothing to read\n");
		return -1;
	}
	if (!((long int)array == (long int)strstr(array, text))){
		fprintf(stdout,"Invalid line: expected %s HAVE %s \n", text, array);
		return -2;
	}

	strcpy(temp, array);
	strtok(array, ":");
	if(!strcmp(array, temp)){
			num = strtok(temp, "=");
	}
	num = strtok(NULL, " ");
	end = num;
	double value = strtod(num, &end);
	if( end == num || !(*end == '\n' || *end == '%' || *end == ' ' || *end == 0)){
		fprintf(stderr, "ERROR: must input a number HAVE: %s\n", temp);
		return -1;
	}
	if( !strcmp(text, "itude: ")){ // if it is latitude or longitude
		strtok(temp, ".");
		strtok(NULL, ".");
		num = strtok(NULL, " ");
		if('S' == toupper(num[0])){
			value = 0 - value;
		}else if ('N' != toupper(num[0])){
			fprintf(stderr, "ERROR: Latitude must be North or South\n");
			return -1;
		}
	} else if (!strcmp(text, "Longitude: ")){
		strtok(temp, ".");
		strtok(NULL, ".");
		num =strtok(NULL, " ");
		if('E' == toupper(num[0])){
			value = 0 - value;
		}else if ('W' != toupper(num[0])){
			fprintf(stderr, "ERROR: Longitude must be East or West\n");
			return -1;
		}
	}
	return value;
}


int setHeader(FILE* file, struct frame* frmPtr){

	int temp;
	temp = (int) checkLine(file, "Version: ");
	if( temp != 1){
		fprintf(stdout, "ERROR: Unsupported version Expected version 1\n");
		return -1;
	} else frmPtr->medPtr.verIN = temp;	 // actually set the version in a header
	temp = (int) checkLine(file, "Sequence: ");
	if( temp < 1 || temp > 511){
		fprintf(stdout, "ERROR: Bad Sequence number\n");
		return -2;
	} else frmPtr->medPtr.seqIN = temp;
	temp = (int) checkLine(file, "From: ");
	if( temp < 1 || temp > 65535){
		fprintf(stdout, "ERROR: Bad From number\n");
		return -3;
	} else frmPtr->medPtr.srcIN = temp;
	temp = (int) checkLine(file, "To: ");
	if ( temp < 1 || temp > 65535){
		fprintf(stdout, "ERROR: Bad To number\n");
		return-4;
	} else frmPtr->medPtr.dstIN = temp;

	frmPtr->medPtr.seqIN = htons(frmPtr->medPtr.seqIN);
	frmPtr->medPtr.srcIN = htonl(frmPtr->medPtr.srcIN);
	frmPtr->medPtr.dstIN = htonl(frmPtr->medPtr.dstIN);


	char array [MAXSIZE];
	fgets(array, 4, file); 		
	
	if( !strcmp(array, "GET") || !strcmp(array, "Glu") || !strcmp(array, "Cap") 
			|| !strcmp(array, "Omo") || !strcmp(array, "Seq")){
		if(setCommand( file, frmPtr)){
			return -1;
		}
	} else if( !strcmp( array, "Mes")){
		if (setMessage(file, frmPtr)){
			return -1;
		}
	} else if( !strcmp( array, "Lat")){
		if (setGps(file, frmPtr)){
			return -1;
		}
	} else if( !strcmp( array, "Bat")){
		if(setStatus(file, frmPtr)){
			return -1;
		}
	} else {
		fprintf(stderr, "ERROR: %s is not valid input\n", array);
		return -5;
	}
	setLocal(frmPtr);
	return 0;

}



int setMessage(FILE* file, struct frame* frmPtr){
	char array[7];
	
	fgets(array, sizeof(array), file);
	if(!((long int) &*array == (long int)strstr(array, "sage: "))){
		fprintf(stderr, "ERROR: Invalid Line: expected Message: \n Have: %s\n", array);
		return -1;
	}
	long position = ftell(file);
	int temp, count = 0;
	while( (temp = fgetc(file)) != '\0' && temp != EOF){ 
		count++;
		if( count == 1478){
			fprintf(stderr, "ERROR: message is too large, max size is 1477\n");
			return -1;
		}
	}
	
	fseek(file, position, SEEK_SET);
	
	frmPtr->msgPtr->len = count;
	fread(frmPtr->msgPtr->message, 1, count, file);
	
	fgets(array, sizeof(array), file); // clear out anything left inthe line
	frmPtr->medPtr.typeIN = 3;
	setLens(frmPtr, 12 + frmPtr->msgPtr->len);
	
	setDefaults(frmPtr);
	return 0;
}

int setGps(FILE* file, struct frame* frmPtr){
	double temp;
	temp = checkLine(file, "itude: ");
	if (temp < -90 || temp > 90){
		fprintf(stderr, "ERROR: invalid latitude\n");
		return -1;
	}else frmPtr->gpsPtr.latiDB = temp;
	temp = checkLine(file, "Longitude: ");
	if (temp < -180 || temp > 180){
		fprintf(stderr, "ERROR: invalid longitude\n");
		return -1;
	}else frmPtr->gpsPtr.longDB = temp;
	float alt = ((float) checkLine(file, "Altitude: "))/6;
	if (alt < -6705 || alt > 4839){ // ma is heiht of mt everest min is deepest depth every achieved
		fprintf(stderr, "ERROR: invalid altitude\n");
		return -1;
	}else frmPtr->gpsPtr.altiDB = alt;
	frmPtr->medPtr.typeIN = 2;
	setLens(frmPtr, 32);
	setDefaults(frmPtr);
	return 0;
}

void setLens( struct frame* frmPtr, int len){

	frmPtr->medPtr.lenIN = htons(len); // assign other lengths here as well
	frmPtr->ipPtr.lenIN = htons(len + 28);		//need this here
	frmPtr->udpPtr.lenSH = htons(len + 8);
}

int setCommand( FILE* file, struct frame* frmPtr){
	char array [MAXSIZE];
	fgets(array, 4, file);
	bool hasPara = false;
	if (!strcmp(array, " ST")){ // GET STATUS
		frmPtr->cmdPtr.comIN = htons(0);
	} else	if(!strcmp(array, "cos")){ // Glucose
		frmPtr->cmdPtr.comIN = htons(1);
		int size = checkLine(file, "e=");
		if(size < 0 || size > 65535){
			fprintf(stderr, "ERROR: invalid Glucose setting\n");
			return -1;
		} 
		frmPtr->cmdPtr.parIN = htons((unsigned short)size);
		hasPara = true;
	} else if (!strcmp(array, " GP")){ // GET GPS
		frmPtr->cmdPtr.comIN = htons(2);
	} else if(!strcmp(array, "sai")){ // Capsaicin
		frmPtr->cmdPtr.comIN = htons(3);
		int size = checkLine(file, "cin=");
		if(size < 0 || size > 65535){
			fprintf(stderr, "ERROR: invalid Capsaicin setting\n");
			return -1;
		} 
		frmPtr->cmdPtr.parIN = htons((unsigned short)size);
		hasPara = true;
	} // reserved for future use
	/*else if (!strcmp(array, " ")){ // 
		frmPtr->cmdPtr.comIN = htons(4);
	}*/ 
	else if(!strcmp(array, "rfi")){ // Omorfine
		frmPtr->cmdPtr.comIN = htons(5);
		int size = checkLine(file, "ne=");
		if(size < 0 || size > 65535){
			fprintf(stderr, "ERROR: invalid Omorfine setting\n");
			return -1;
		} 
		frmPtr->cmdPtr.parIN = htons((unsigned short)size);
		hasPara = true;
	} // reserved for future use
	/*else if (!strcmp(array, " ")){ // 
		frmPtr->cmdPtr.comIN = htons(6);
	}*/ 
	else if(!strcmp(array, "uen")){ // REPEAT
		frmPtr->cmdPtr.comIN = htons(7);
		int size = checkLine(file, "ce=");
		if(size < 0 || size > 65535){
			fprintf(stderr, "ERROR: invalid Sequence\n");
			return -1;
		} 
		frmPtr->cmdPtr.parIN = htons((unsigned short)size);
		hasPara = true;
	}
	fgets(array, MAXSIZE, file); 			//Move file pointer to end of line
	frmPtr->medPtr.typeIN = 1; // need this here
	if (hasPara){
		setLens(frmPtr, 16);
	} else {
		setLens(frmPtr, 14);
	}
	setDefaults(frmPtr);
	return 0;
}

int setStatus(FILE* file, struct frame* frmPtr){
	double temp;
	temp = checkLine(file, "tery: ");
	if (temp < 0){
		fprintf(stderr, "ERROR: Battery cannot be a negative value\n");
		return 1;
	} else frmPtr->stsPtr.batDB = temp/100;
	temp = checkLine(file, "Glucose: ");
	if (temp < 0){
		fprintf(stderr, "ERROR: Glucose cannot be a negative value\n");
		return 1;
	} else frmPtr->stsPtr.gluIN = htons(temp);
	temp = checkLine(file, "Capsaicin: ");
	if (temp < 0){
		fprintf(stderr, "ERROR: Capsaicin cannot be a negative value\n");
		return 1;
	} else frmPtr->stsPtr.capIN = htons(temp);
	temp = checkLine(file, "Omorfine: ");
	if (temp < 0){
		fprintf(stderr, "ERROR: Omorfine cannot be a negative value\n");
		return 1;
	} else frmPtr->stsPtr.omoIN = htons(temp);
	setLens(frmPtr, 28);
	setDefaults(frmPtr);
	return 0;
	
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
	frmPtr->locPtr.nxtLenIN = frmPtr->locPtr.lenIN;
}

void setGlobal( const char* fileName){
	FILE* file;
	if(! (file = fopen(fileName, "wb"))){
		exit(0);
	};
	struct globalHeader global = {.magicIN = 2712847316, .magVerSH = 2, .minVerSH = 4};
	fwrite( &global, 1, 24, file );
	fclose(file);
}