#ifndef ip_Header
	#define ip_Header
#include <stdio.h>
#include <stdlib.h>


struct ipv4Header{
	unsigned char version[1];
	unsigned char headerLength[1];
	unsigned char TOS[1];
	unsigned char totalLength[2];
	unsigned char identification[2];
	unsigned char flags[1];
	unsigned char fragmentOffset[2];
	unsigned char TTL[1];
	unsigned char nextProtocol[1];
	unsigned char headerCheckSum[2];
	unsigned char sourceAddress[4];
	unsigned char destinationAddress[4];
	//assign dynamically	unsigned char options[40];
	
};

void setIpHeader(FILE*, struct ipv4Header*, int, unsigned char*);

int getIpLen(unsigned char*, int);
#endif
