#include <stdio.h>
#include <stdlib.h>
#include "ipV4.h"
#include "printHeader.h"



void setIpHeader(FILE* file, struct ipv4Header* ipv4Header,int size,
						 unsigned char* first){
	
// change to if size > 20 fill out options
	if(size == 20){
		unsigned char temp[2];
		ipv4Header->version[0] = first[0] & 240;
		ipv4Header->headerLength[0] = first[0] & 15;
		fread(ipv4Header->TOS, 1, 1, file);
		fread(ipv4Header->totalLength, 2, 1, file);
		fread(ipv4Header->identification, 2, 1, file);
		fread(temp, 2, 1, file);
		ipv4Header->flags[0] = temp[0] & 240;
		ipv4Header->fragmentOffset[0] = temp[0] & 15;
		ipv4Header->fragmentOffset[1] = temp[1];
		fread(ipv4Header->TTL, 1,1, file);
		fread(ipv4Header->nextProtocol, 1, 1, file);
		fread(ipv4Header->headerCheckSum, 2, 1, file);
		fread(ipv4Header->sourceAddress, 4, 1, file);
//		printHeader(ipv4Header->sourceAddress, 4);
		fread(ipv4Header->destinationAddress, 4, 1, file);
//		printHeader(ipv4Header->destinationAddress, 4);
	}
//	printf("here is %d\n", size);
}
int getIpLen(unsigned char* bits, int size){
	if (size < 0 || bits == NULL){
		printf("ERROR: Invalid IP Length");
		return 0;
	}
	unsigned char leftSide = bits[0] >> 4;
	if (leftSide == 4){
		return 4 * (bits[0] & 15);
	}else{
		printf("ERROR: not IPV4\n");
	}
	return 0;
}
