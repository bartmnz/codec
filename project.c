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
	struct command* cmdPtr;
	struct message* msgPtr;
	struct status* stsPtr;
	int type = (int) medPtr->type[0];
	printf("int is %d", type);
	if(type == 2){
		gpsPtr = malloc(sizeof(struct gps));
		setGps(file, gpsPtr);
		printf("longitude is %f\n", gpsPtr->d);
	}else if(type == 3){
		msgPtr = malloc(sizeof(struct message));
		// need to set size of message from size of packet
		setMessage(file, msgPtr, 10);
	}else if(type == 1){
		cmdPtr = malloc(sizeof(struct command));
		//need to set true / false from size of packet
		setCommand(file, cmdPtr, false);
	}else if(type == 0){
		stsPtr = malloc(sizeof(struct status));
		setStatus(file, stsPtr);
	}else{
		printf("ERROR: invalid message type");
	}
	
	if(type == 0) free(stsPtr);
	if(type == 1) free(cmdPtr);
	if(type == 2) free(gpsPtr);
	if(type == 3) free(msgPtr);
	free(medPtr);
	free(udp);
	free(frameName);
	free(ipH);

	printf("\n\n\n");
	fclose(file);
}



















