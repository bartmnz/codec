#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include "ethernetFrame.h"
#include "printHeader.h"
#include "ipV4.h"
#include "udp.h"
#include "meditrik.h"

int getMessageType( unsigned char*, int);
int getSequenceID(unsigned char*, int);


int main(int argc, const char* argv[]){

// chekc if machine is little or big endian see here  http://stackoverflow.com/questions/19275955/convert-little-endian-to-big-endian
	bool isLE = true;
	unsigned char buffer[40];
	FILE* file;
	if (argc <= 1){
		file = fopen("status.pcap", "rb");	
	}else {
		file = fopen(argv[1], "rb");
	}
	fread(buffer,sizeof(buffer), 1,file);


	struct ethernetFrame* frameName = malloc(14);
	setEthernetHeader(file, frameName);	

	unsigned char temp[1];
	fread(temp, sizeof(temp), 1, file);
	

	int sizeof_ip = getIpLen(temp, sizeof(temp));

	struct ipv4Header* ipH = malloc(sizeof_ip+2);
	setIpHeader(file, ipH, sizeof_ip, temp);

	struct udpHeader* udp; 
	udp = malloc(8);
	*udp = {0};
	if(ipH->nextProtocol[0] == 0x11){
		setUdpHeader(file, udp);
	}else{
		printf("ERROR: not utilizing UDP protocol\n");
	}		
	struct meditrik* medPtr = malloc(sizeof(struct meditrik));
	setMeditrikHeader(file, medPtr);


	printf("Version: %d\n", medPtr->verIN);
	if (isLE){
		medPtr->seqIN = ntohs(medPtr->seqIN);
		medPtr->srcIN = ntohl(medPtr->srcIN);
		medPtr->dstIN = ntohl(medPtr->dstIN);
	}
	printf("Sequence: %d\n",medPtr->seqIN);
	printf("From:  %u\n", medPtr->srcIN);	
	printf("To:  %d\n", medPtr->dstIN);

	struct gps *gpsPtr;
	struct command* cmdPtr;
	struct message* msgPtr;
	struct status* stsPtr;
	int type = medPtr->typeIN;

	if(type == 2){
		// need to determine how North and West are defined for GPS Coordinates!!!
		gpsPtr = malloc(sizeof(struct gps));
		setGps(file, gpsPtr);
		printf("Latitude : %.9f\n", gpsPtr->latiDB);
		printf("Longitude: %.9f\n", gpsPtr->longDB); 
		printf("Altitude : %.0f ft. \n", gpsPtr->altiDB * 6); // stored as fathoms 
	}else if(type == 3){
		int szData = ntohs(medPtr->lenIN) - 12;
		msgPtr = malloc(szData+1);
		setMessage(file, msgPtr, szData);
		printf("%s\n",  msgPtr->message);
	}else if(type == 1){
		cmdPtr = malloc(sizeof(struct command));
		bool hasParam = ntohs(medPtr->lenIN) == 16 ? true : false; 
		setCommand(file, cmdPtr, hasParam);
		char* command[8] = {"GET_STATUS", "SET_GLUCOSE", "GET_GPS",
					"SET_CAPSAICIN","RESERVED",
					"SET_OMORFINE","RESERVED",
					"REPEAT"};
		char* parameter[9] = {"","Glucose","", "Capsaicin","",
					 "Omorfine","","SequenceID"};
		int index = ntohs(cmdPtr->comIN);
		printf("%s\n", command[index]);
		if(hasParam) printf("%s=%d", parameter[index], ntohs(cmdPtr->parIN)); 
	}else if(type == 0){
		stsPtr = malloc(sizeof(struct status));
		setStatus(file, stsPtr);
		printf("Battery: %.2f%% \n", 100* stsPtr->batDB);
		printf("Glucose: %d\n", ntohs(stsPtr->gluIN));
		printf("Capsaicin: %d\n", ntohs(stsPtr->capIN));
		printf("Omorfine: %d\n", ntohs(stsPtr->omoIN));
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



















