#include <stdio.h>
#include "meditrik.h"

//int getMessageType(struct meditrick* mdPtr){-
//}
//

void setMeditrikHeader(FILE* file, struct meditrik* medPtr){
	unsigned char temp[2];
	fread(temp, 2, 1, file);
	medPtr->verIN = 0;
	medPtr->verUC[0] = temp[0] >> 4;

	medPtr->seqUC[0] = (temp[0] & 8) >> 3;
	medPtr->seqUC[1] = (temp[1] >> 3) | ( temp[0] << 5);
	medPtr->typeIN = 0;	
	medPtr->typeUC[0] = temp[1] & 7;
	medPtr->lenIN = 0;
	fread(medPtr->lenUC, 2, 1, file);
	fread(medPtr->srcUC, 4, 1, file);
	fread(medPtr->dstUC, 4, 1, file);
	//printHeader(medPtr->totalLength, 2);
}

void setCommand(FILE* file, struct command* cmdPtr, bool hasPara){
	fread(cmdPtr->comUC, 2, 1, file);
	if(hasPara)fread(cmdPtr->parUC, 2, 1, file);
}

void setStatus(FILE* file, struct status* stsPtr){
	fread(stsPtr->batUC, 8, 1, file);
	fread(stsPtr->gluUC, 2, 1, file);
	fread(stsPtr->capUC, 2, 1, file);
	fread(stsPtr->omoUC, 2, 1, file);
}

void setGps(FILE* file, struct gps* gpsPtr){
	fread(gpsPtr->longUC, 8, 1, file);
	fread(gpsPtr->latiUC, 8, 1, file);
	fread(gpsPtr->altiUC, 4, 1, file);
}

void setMessage(FILE* file, struct message* msgPtr, int size){
	// some error checking
	if(size)
	fread(msgPtr->message, size, 1, file);
//	msgPtr->message[size] = '\0';
}
