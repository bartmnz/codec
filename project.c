#include <stdio.h>
#include <stdlib.h>

#include "ethernetFrame.h"
#include "printHeader.h"
#include "ipV4.h"
#include "udp.h"

int getMessageType( unsigned char*, int);
int getSequenceID(unsigned char*, int);


int main(void){
	unsigned char buffer[40];
	FILE* file;
	file = fopen("hello.pcap", "rb");
	fread(buffer,sizeof(buffer), 1,file);
	printHeader(buffer, sizeof(buffer));

	struct ethernetFrame* frameName = malloc(14);
	setEthernetHeader(file, frameName);	

	unsigned char temp[1];
	fread(temp, sizeof(temp), 1, file);
	//printHeader(temp, sizeof(temp));	

	int sizeof_ip = getIpLen(temp, sizeof(temp));
	struct ipv4Header* ipH = malloc(sizeof_ip);
	setIpHeader(file, ipH, sizeof_ip, temp);

	struct udpHeader* udp;
	udp = malloc(8);
	if(ipH->nextProtocol[0] == 0x11){
		setUdpHeader(file, udp);
		//fread(udp, sizeof(udp), 1, file);
	//	printHeader(udp ,sizeof(udp));
	}else{
		printf("ERROR: not utilizing UDP protocol\n");
	}	
	
	unsigned char* meditrik;
	int sizeof_meditrik = (udp->length[0]*256 + udp->length[1])-8;
	meditrik = malloc(sizeof_meditrik);
	fread(meditrik, sizeof_meditrik, 1, file);
	printHeader(meditrik, sizeof_meditrik);
	printf("\nmessage type is %d\n", getMessageType(meditrik, sizeof_meditrik));
	printf("Sequence Id is : %d\n", getSequenceID(meditrik, sizeof_meditrik));



	free(udp);
	free(frameName);
	free(ipH);
	free(meditrik);
	printf("\n\n\n");
	fclose(file);
}

int getMessageType(unsigned char* bits, int size){
	if (size < 0 || bits == NULL){
		printf("ERROR: Invalid MESSAGE Type");
		return 0;
	}
	return bits[1] & 7;
}

int getSequenceID(unsigned char* bits, int size){
	if (size < 0 || bits == NULL){
		printf("ERROR: Invalid SEQUENCE ID\n");
		return 0;
	}
	unsigned char leftSide = bits[0] & 15;
	unsigned char rightSide = bits[1] >> 3;
	return (32* leftSide + rightSide);
}

