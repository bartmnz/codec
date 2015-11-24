#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include "meditrik.h"

//int getMessageType(struct meditrick* mdPtr){-
//}
//

void getMeditrikHeader(FILE* file, bool isLE){
	struct meditrik* medPtr = malloc(sizeof(struct meditrik));
	memset(medPtr, 0, sizeof(struct meditrik));
	unsigned char temp[2];
	fread(temp, 2, 1, file);
	medPtr->verIN = 0;
	medPtr->verUC[0] = temp[0] >> 4;
	medPtr->seqUC[0] = (temp[0] & 8) >> 3;
	medPtr->seqUC[1] = (temp[1] >> 3) | ( temp[0] << 5);
	medPtr->typeUC[0] = temp[1] & 7;
	fread(medPtr->lenUC, 2, 1, file);
	fread(medPtr->srcUC, 4, 1, file);
	fread(medPtr->dstUC, 4, 1, file);
	if(medPtr->verIN != 1){
		printf("ERROR: Unsupported version number!\n");
		free(medPtr);
		return;
	}
	printf("Version: %d\n", medPtr->verIN);
	if (isLE){
		medPtr->seqIN = ntohs(medPtr->seqIN);
		medPtr->srcIN = ntohl(medPtr->srcIN);
		medPtr->dstIN = ntohl(medPtr->dstIN);
	}
	printf("Sequence: %d\n",medPtr->seqIN);
	printf("From:  %u\n", medPtr->srcIN);
	printf("To:  %d\n", medPtr->dstIN);


	int szData = isLE ?  ntohs(medPtr->lenIN) : medPtr->lenIN;
	if(medPtr->typeIN == 0){
		getStatus(file);
	}else if(medPtr->typeIN == 1){
		bool hasParam = szData == 16;
		getCommand(file,hasParam);
	}else if(medPtr->typeIN == 2){
		getGps(file);
	}else if(medPtr->typeIN == 3){
		getMessage(file, szData);
	}else{
 		printf("ERROR: invalid message type");
		free(medPtr);
		return;
	}
	free(medPtr);

}

void getCommand(FILE* file, bool hasPara){
	struct command* cmdPtr = malloc(sizeof(struct command));
	memset(cmdPtr, 0, sizeof( struct command));
	fread(cmdPtr->comUC, 2, 1, file);
	if(hasPara)fread(cmdPtr->parUC, 2, 1, file);

	char* command[8] = {"GET_STATUS", "SET_GLUCOSE", "GET_GPS", 
			"SET_CAPSAICIN","RESERVED", "SET_OMORFINE",
			"RESERVED",  "REPEAT"};
	char* parameter[9] = {"","Glucose","", "Capsaicin", "",
				 "Omorfine","","SequenceID"};
	int index = ntohs(cmdPtr->comIN);
	printf("%s\n", command[index]);
	if(hasPara) printf("%s=%d", parameter[index], ntohs(cmdPtr->parIN));
	free(cmdPtr);
}

void getStatus(FILE* file){ 
	struct status* stsPtr = malloc(sizeof(struct status));
	memset(stsPtr, 0, sizeof( struct status));
	fread(stsPtr->batUC, 8, 1, file);
	fread(stsPtr->gluUC, 2, 1, file);
	fread(stsPtr->capUC, 2, 1, file);
	fread(stsPtr->omoUC, 2, 1, file);
	
	printf("Battery: %.2f%% \n", 100* stsPtr->batDB);
	printf("Glucose: %d\n", ntohs(stsPtr->gluIN));
	printf("Capsaicin: %d\n", ntohs(stsPtr->capIN));
	printf("Omorfine: %d\n", ntohs(stsPtr->omoIN));
	free(stsPtr);
}

void getGps(FILE* file){
	// need to find out how n / w are determined

	struct gps* gpsPtr = malloc(sizeof(struct gps));
	memset(gpsPtr, 0, sizeof(struct gps));
	fread(gpsPtr->longUC, 8, 1, file);
	fread(gpsPtr->latiUC, 8, 1, file);
	fread(gpsPtr->altiUC, 4, 1, file);


	printf("Latitude : %.9f\n", gpsPtr->latiDB);
	printf("Longitude: %.9f\n", gpsPtr->longDB);
	printf("Altitude : %.0f ft. \n", gpsPtr->altiDB * 6); // stored as fathoms 
	free(gpsPtr);
}

void getMessage(FILE* file, int size){
	// some error checkiing
	struct message* msgPtr = malloc(size+8);
	memset(msgPtr, 0, size);
	fread(msgPtr->message, size-1, 1, file);
	msgPtr->message[size-1] = '\n';

	printf("%s\n", msgPtr->message);
	free(msgPtr);
}

