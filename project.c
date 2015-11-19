#include <stdio.h>
#include <stdlib.h>

#include "ethernetFrame.h"
#include "printHeader.h"
#include "ipV4.h"
#include "udp.h"
#include "meditrik.h"

int getMessageType( unsigned char*, int);
int getSequenceID(unsigned char*, int);


int main(void){
	unsigned char buffer[40];
	FILE* file;
	file = fopen("gps.pcap", "rb");
	fread(buffer,sizeof(buffer), 1,file);
	printHeader(buffer, sizeof(buffer));

	struct ethernetFrame* frameName = malloc(14);
	setEthernetHeader(file, frameName);	

	unsigned char temp[1];
	fread(temp, sizeof(temp), 1, file);
	printHeader(temp, sizeof(temp));	

	int sizeof_ip = getIpLen(temp, sizeof(temp));
	printf("%d\n",sizeof_ip);
	struct ipv4Header* ipH = malloc(sizeof_ip+2);
	setIpHeader(file, ipH, sizeof_ip, temp);

	struct udpHeader* udp;
	udp = malloc(8);
	if(ipH->nextProtocol[0] == 0x11){
		setUdpHeader(file, udp);
		//fread(udp, sizeof(udp), 1, file);

	}else{
		printf("ERROR: not utilizing UDP protocol\n");
	}	
	
	struct meditrik* medPtr = malloc(sizeof(struct meditrik));




	setMeditrikHeader(file, medPtr);
	

	struct gps* gpsPtr;
	bool wasGPS = false;
	if(medPtr->type[0] == 0X10){
		gpsPtr = malloc(20);
		setGps(file, gpsPtr);
		wasGPS = true;
	}
	

	if(wasGPS) free(gpsPtr);
	free(medPtr);
	free(udp);
	free(frameName);
	free(ipH);

	printf("\n\n\n");
	fclose(file);
}



















