#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>


#include "meditrik.h"


int getMessageType( unsigned char*, int);
int getSequenceID(unsigned char*, int);
bool checkEndian(void);
int stripHeaders(FILE*, struct frame*);
void evaluatePcap(unsigned char*);
void stripGlobal(FILE*);


int main(int argc, const char* argv[]){
	FILE* file;
	if(argc != 2){ 
		fprintf(stderr,"Usage = project (FILENAME)\n");
		exit(0);
	};
	if(!(file = fopen(argv[1], "rb"))){
		fprintf(stderr,"ERROR: could not open file\n");
		exit(0);
	}
	stripGlobal(file);
	bool quit = false;
	long position = ftell(file); 
	char temp;
	struct frame* frmPtr = malloc(sizeof(struct frame));
	frmPtr->msgPtr = malloc(1477);
	while( ((temp = fgetc(file)) != EOF) && !quit){
		memset(frmPtr, 0, sizeof(struct frame)- sizeof(void*));
		memset(frmPtr->msgPtr, 0, 1477);
		fseek(file, position, SEEK_SET);
		quit = stripHeaders(file, frmPtr);
		getMeditrikHeader(file, frmPtr);
		fprintf(stdout,"\n");
		position = ftell(file);
	
	}
	free(frmPtr->msgPtr);
	free(frmPtr);
	fclose(file);
}

bool checkEndian(void){
	int n = 1;
	return (*(char *)&n == 1); 
}


int stripHeaders(FILE* file, struct frame* frmPtr){
	int size;
	unsigned char buffer[16]; // strip off local header ignore for now
	size = fread(buffer, 1, sizeof(buffer), file);
	if (size != 16){
		return 1;
	}
	
	setEthernetHeader(file, &(frmPtr->ethPtr));
	
	unsigned char temp[1];
	fread(temp, sizeof(temp), 1, file);
	frmPtr->ipPtr.nextProtocol[0] = 0;
	int sizeof_ip = getIpLen(temp, sizeof(temp));
	setIpHeader(file, &(frmPtr->ipPtr), sizeof_ip, temp);
	if(frmPtr->ipPtr.nextProtocol[0] != 0x11){
		fprintf(stderr,"ERROR1\n");
		return 2;
	}
	
	setUdpHeader(file, &(frmPtr->udpPtr));
	
	return 0;
}

void stripGlobal(FILE* file){
	unsigned char header[24];
	int size;
	size = fread(header, 1, sizeof(header),file);
	if (size != 24){
		fprintf(stderr,"ERROR: Invalid format\n");
		exit(0);
	}
	unsigned char magicNum[8];
	unsigned char numMagic[8];
	memcpy(magicNum, (unsigned char[]) { 0xd4, 0xc3, 0xb2, 0xa1, 0x02, 0x00, 0x04, 0x00 }, sizeof(magicNum));
	memcpy(numMagic, (unsigned char[]) { 0xa1, 0xb2, 0xc3, 0xd4, 0x00, 0x02, 0x00, 0x04 }, sizeof(numMagic));
	bool isLe = !(memcmp(magicNum, header, 8));
	bool isBe = !(memcmp(numMagic, header, 8));
	if(!(isLe ||  isBe)){
		fprintf(stderr,"ERROR: is not a valid PCAP file\n");
		exit(0);
	}
}

