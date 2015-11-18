#include <stdio.h>
#include <stdlib.h>

#include "ethernetFrame.h"
#include "printHeader.h"
#include "ipV4.h"
#include "udp.h"
#include "meditrik.h"





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
	
	struct meditrik* medPtr = malloc(14);
	setMeditrikHeader(file, medPtr);

	struct statusPayload* stPtr;
	struct commandPayload* cmdPtr;
	struct gpsPayload* gpsPtr;
	

	if(medPtr->type == 0){
		#define StatusMessage
		stPtr = malloc(14);
		setStatusPayload(file, stPtr);
	}else if(medPtr->type == 1){
		#define CommandInstruction
		cmdPtr = malloc(4);
		// set bool to  equal accurate value -- if has options params
		setCommandPayload(file, cmdPtr, false);
	}else if(medPtr->type == 2){
		#define GpsInstruction
		gpsPtr = malloc(20);
		setGpsPayload(file, gpsPtr);
	}else if(medPtr->type == 3){
		#define MessageInstruction
	//	struct messagePayload* msgPtr = malloc(//figure out size of message)
	//	setmessagePayload(file, msgPtr);

	}else{
		// do something here because you have invalid input.
	}



	#ifdef StatusMessage 
		free(stPtr);
	#elif defined CommandInstruction
		free(cmdPtr);
	#elif defined GpsInstruction
		free(gpsPtr);
	#elif defined MessageInstruction
		free(msgPtr);
	#else 
		//bad juju happened or some  suff
	#endif
	free(medPtr);
	free(udp);
	free(frameName);
	free(ipH);
	printf("\n\n\n");
	fclose(file);
}



