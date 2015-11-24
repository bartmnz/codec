#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include "ethernetFrame.h"
#include "printHeader.h"
#include "ipV4.h"
#include "udp.h"
#include "meditrik.h"


int getMessageType( unsigned char*, int);
int getSequenceID(unsigned char*, int);
bool checkEndian(void);
void problems(FILE* file);
void stripHeaders(FILE*, bool);
void evaluatePcap(unsigned char*);
void stripGlobal(FILE*);


int main(int argc, const char* argv[]){
	bool isLE = checkEndian();
	FILE* file;
	if(argc != 2){ // set errno goto problems
	};
	if(!(file = fopen(argv[1], "rb"))) problems(file);
	stripGlobal(file);
	bool hasRun = false;
	while (true){
		stripHeaders(file, hasRun);
//		if(errno) problems(file);
		getMeditrikHeader(file, isLE);
		printf("\n");
		hasRun = true;
	}
}

bool checkEndian(void){
	int n = 1;
	return (*(char *)&n == 1); 
}
/*
void stripGlobal(FILE* file){
	unsigned char buffer[24];
	int size;
	size = fread(buffer,sizeof(buffer), 1,file);
	if (size != 1) problems(file);
	evaluatePcap(buffer);
	if(errno) problems(file);
}
*/
void stripHeaders(FILE* file, bool hasRun){
	int size;
	unsigned char buffer[16]; // strip off local header ignore for now
	size = fread(buffer, sizeof(buffer), 1, file);
	if (size != 1 && hasRun){
		// handle exit better need to check 
		problems(file);

	}
	struct ethernetFrame* frameName = malloc(14);
	setEthernetHeader(file, frameName);
	unsigned char temp[1];
	fread(temp, sizeof(temp), 1, file);
	free(frameName);

	int sizeof_ip = getIpLen(temp, sizeof(temp));
	struct ipv4Header* ipH = malloc(sizeof_ip+2);
	setIpHeader(file, ipH, sizeof_ip, temp);
	if(ipH->nextProtocol[0] != 0x11){
		free(ipH);
		errno = EPROTONOSUPPORT;
		return;
	}
	free(ipH);


	struct udpHeader* udp = malloc(8);
	memset(udp, 0, sizeof(struct udpHeader));	
	setUdpHeader(file, udp);
	free(udp);
	

	
}

void stripGlobal(FILE* file){
	unsigned char header[24];
	int size;
	size = fread(header,sizeof(header), 1,file);
	if (size != 1){
		errno = EIO;
		problems(file);
	}
	unsigned char magicNum[8];
	unsigned char numMagic[8];
	memcpy(magicNum, (unsigned char[]) { 0xd4, 0xc3, 0xb2, 0xa1, 0x02, 0x00, 0x04, 0x00 }, sizeof(magicNum));
	memcpy(numMagic, (unsigned char[]) { 0xa1, 0xb2, 0xc3, 0xd4, 0x00, 0x02, 0x00, 0x04 }, sizeof(numMagic));
	bool isLe = !(memcmp(magicNum, header, 8));
	bool isBe = !(memcmp(numMagic, header, 8));
	if(!(isLe ||  isBe)){
		errno = ENOTSUP;
		problems(file);
	}	
	unsigned char linkHeader[4];
	unsigned char headerLink[4];
	memcpy(linkHeader, (unsigned char[]) {0x01, 0x00, 0x00, 0x00}, sizeof(linkHeader));
	memcpy(headerLink, (unsigned char[]) {0x00, 0x00, 0x00, 0x01}, sizeof(headerLink));

	if(( isLe && memcmp(linkHeader, &header[20], 4)) ||
		(isBe && memcmp(headerLink, &header[20], 4))){
		errno = EPROTONOSUPPORT;
		problems(file);
	}

}

void problems(FILE* file){
	file = file;
	//fclose(file);
	printf("ERROR: %s\n", strerror(errno));
	exit(errno);
}

