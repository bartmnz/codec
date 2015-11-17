#include <stdio.h>
#include "meditrik.h"

//int getMessageType(struct meditrick* mdPtr){-
//}
//

void setMeditrikHeader(FILE* file, struct meditrik* medPtr){
	unsigned char temp[2];
	fread(temp, 2, 1, file);
	medPtr->version = temp[0] >> 4;
	medPtr->sequenceID[0] = (temp[0] & 8) >> 3;
	medPtr->sequenceID[1] = (temp[1] >> 3) & ( temp[0] << 5);
	medPtr->type = temp[2] & 7;
	fread(medPtr->totalLength, 2, 1, file);
	fread(medPtr->sourceID, 4, 1, file);
	fread(medPtr->destinationID, 4, 1, file);
	//printHeader(medPtr->totalLength, 2);
}
