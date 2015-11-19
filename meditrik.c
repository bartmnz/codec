#include <stdio.h>
#include "meditrik.h"

//int getMessageType(struct meditrick* mdPtr){-
//}
//

void setMeditrikHeader(FILE* file, struct meditrik* medPtr){
	unsigned char temp[2];
	fread(temp, 2, 1, file);
	medPtr->version[0] = temp[0] >> 4;
	medPtr->sequenceID[0] = (temp[0] & 8) >> 3;
	medPtr->sequenceID[1] = (temp[1] >> 3) & ( temp[0] << 5);
	medPtr->type[0] = temp[1] & 7;
	fread(medPtr->totalLength, 2, 1, file);
	fread(medPtr->sourceID, 4, 1, file);
	fread(medPtr->destinationID, 4, 1, file);
	//printHeader(medPtr->totalLength, 2);
}

void setCommand(FILE* file, struct command* cmdPtr, bool hasPara){
	fread(cmdPtr->comm, 2, 1, file);
	if(hasPara)fread(cmdPtr->para, 2, 1, file);
}

void setStatus(FILE* file, struct status* stsPtr){
	fread(stsPtr->batPow, 8, 1, file);
	fread(stsPtr->glucose, 2, 1, file);
	fread(stsPtr->capsaicin, 2, 1, file);
	fread(stsPtr->omorfine, 2, 1, file);
}

void setGps(FILE* file, struct gps* gpsPtr){
	fread(gpsPtr->longitute, 8, 1, file);
	fread(gpsPtr->latitude, 8, 1, file);
	fread(gpsPtr->altitude, 4, 1, file);
}

void setMessage(FILE* file, struct message* msgPtr, int size){
	// some error checking
	fread(msgPtr->message, size, 1, file);
}
