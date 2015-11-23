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
void problems(void);
void stripHeaders(FILE*);
void evaluatePcap(unsigned char*);

int main(int argc, const char* argv[]){
	bool isLE = checkEndian();
	FILE* file;
	if(argc != 2){ // set errno goto problems
	};
	if(!(file = fopen(argv[1], "rb"))) problems();
	stripHeaders(file);
	getMeditrikHeader(file, isLE);
	printf("\n\n\n");
	fclose(file);
}

bool checkEndian(void){
	int n = 1;
	return (*(char *)&n == 1); 
}

void stripHeaders(FILE* file){
	unsigned char buffer[40];
	fread(buffer,sizeof(buffer), 1,file);
	evaluatePcap(buffer);
	struct ethernetFrame* frameName = malloc(14);
	setEthernetHeader(file, frameName);
	unsigned char temp[1];
	fread(temp, sizeof(temp), 1, file);
	int sizeof_ip = getIpLen(temp, sizeof(temp));
	struct ipv4Header* ipH = malloc(sizeof_ip+2);
	setIpHeader(file, ipH, sizeof_ip, temp);
	struct udpHeader* udp;
	udp = malloc(8);
	memset(udp, 0, sizeof(struct udpHeader));

	if(ipH->nextProtocol[0] == 0x11){
		setUdpHeader(file, udp);
	}else{
		errno = EPROTONOSUPPORT;
		problems();
	}	

	free(udp);
	free(frameName);
	free(ipH);

}

void evaluatePcap(unsigned char* header){
	unsigned char magicNum[4];
	unsigned char numMagic[4];
	memcpy(magicNum, (unsigned char[]) { 0xd4, 0xc3, 0xb2, 0xa1 }, sizeof(magicNum));
	memcpy(numMagic, (unsigned char[]) { 0xa1, 0xb2, 0xc3, 0xd4 }, sizeof(numMagic));
	bool isLe = !(memcmp(magicNum, header, 4));
	bool isBe = !(memcmp(numMagic, header, 4));
	if(!(isLe ||  isBe)){
		printf("%d\n", memcmp(numMagic, header, 4));
		errno = ENOTSUP;
		problems();
	}
	unsigned char version[4];
	unsigned char noisrev[4];
	memcpy(version, (unsigned char[]) { 0x02, 0x00, 0x04, 0x00 }, sizeof(version));
	memcpy(noisrev, (unsigned char[]) { 0x00, 0x02, 0x00, 0x04 }, sizeof(noisrev));
	if((isLe && memcmp(version, &header[4], 4)) ||
		(isBe && memcmp(noisrev, &header[4], 4))){
		errno = EPROTONOSUPPORT;
		problems();
	}

	unsigned char linkHeader[4];
	unsigned char headerLink[4];
	memcpy(linkHeader, (unsigned char[]) {0x01, 0x00, 0x00, 0x00}, sizeof(linkHeader));
	memcpy(headerLink, (unsigned char[]) {0x00, 0x00, 0x00, 0x01}, sizeof(headerLink));

	if(( isLe && memcmp(linkHeader, &header[20], 4)) ||
		(isBe && memcmp(headerLink, &header[20], 4))){
		errno = EPROTONOSUPPORT;
		problems();
	}

}
void problems(void){
	printf("ERROR: %s\n", strerror(errno));
	exit(errno);
}

