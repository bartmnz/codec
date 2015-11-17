#include <stdio.h>
#include <stdlib.h>

#include "ethernetFrame.h"
#include "printHeader.h"
#include "ipV4.h"

//int getIpLen(unsigned char*, int);
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
	free(frameName);



	unsigned char temp[1];
	fread(temp, sizeof(temp), 1, file);
	printHeader(temp, sizeof(temp));
	

	int sizeof_ip = getIpLen(temp, sizeof(temp));
	struct ipv4Header* ipH = malloc(sizeof_ip);
	setIpHeader(file, ipH, sizeof_ip, temp);
	free(ipH);	
/*


	unsigned char* ipV4;
	ipV4 = malloc(sizeof_ipV4);
	ipV4[0] = temp[0];
	fread(&ipV4[1],sizeof_ipV4-1, 1, file);
	printHeader(ipV4, sizeof_ipV4);
*/
	unsigned char udp[8];
//	if(ipV4[9] == 0x11){
	if(1){ //temp for now see above line
		fread(udp, sizeof(udp), 1, file);
		printHeader(udp ,sizeof(udp));
	}else{
		printf("ERROR: not utilizing UDP protocol\n");
	}	
	
	unsigned char* meditrik;
	int sizeof_meditrik = (udp[4]*256 + udp[5])-sizeof(udp);
	meditrik = malloc(sizeof_meditrik);
	fread(meditrik, sizeof_meditrik, 1, file);
	printHeader(meditrik, sizeof_meditrik);
	printf("\nmessage type is %d\n", getMessageType(meditrik, sizeof_meditrik));
	printf("Sequence Id is : %d\n", getSequenceID(meditrik, sizeof_meditrik));
	
//	free(ipV4);
	free(meditrik);
	printf("\n\n\n");
	fclose(file);
}

/*
int getIpLen(unsigned char* bits, int size){
	unsigned char rightSide = bits[0] & 15;
	if (size < 0 || bits == NULL){
		printf("ERROR: Invalid IP Length");
		return 0;
	}
	return 4 * rightSide;
	
}
*/
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

