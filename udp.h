#include <stdio.h>
#ifndef UDP_HEADER
	#define UDP_HEADER
struct udpHeader{
	unsigned char sourcePort[2];
	unsigned char destinationPort[2];
	unsigned char length[2];
	unsigned char checksum[2];
};

void setUdpHeader(FILE*, struct udpHeader*);
#endif
