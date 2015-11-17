#include <stdio.h>
#include <stdlib.h>
#include "udp.h"

void setUdpHeader(FILE* file, struct udpHeader* udp){
	fread(udp->sourcePort, 2, 1, file);
	fread(udp->destinationPort, 2, 1, file);
	fread(udp->length, 2, 1, file);
	fread(udp->checksum, 2, 1, file);
}
