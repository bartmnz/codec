#include <stdio.h>
#include "printHeader.h"
#ifndef MEDITRIK_H
	#define MEDITRIK_H


struct meditrik{
	unsigned char version;
	unsigned char sequenceID[2];
	unsigned char type;
	unsigned char totalLength[2];
	unsigned char sourceID[4];
	unsigned char destinationID[4];
};

//int getMessageType(struct meditrik*);
//int getSequenceID(struct meditrik*);
//int getVersion(struct meditrik*);
void setMeditrikHeader(FILE* file, struct meditrik*);
#endif
