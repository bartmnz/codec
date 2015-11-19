#include <stdio.h>
#include <stdbool.h>
#include "printHeader.h"
#ifndef MEDITRIK_H
	#define MEDITRIK_H


struct meditrik{
	unsigned char version[1];
	unsigned char sequenceID[2];
	unsigned char type[1];
	unsigned char totalLength[2];
	unsigned char sourceID[4];
	unsigned char destinationID[4];
};

struct status{
	unsigned char batPow[8];
	unsigned char glucose[2];
	unsigned char capsaicin[2];
	unsigned char omorfine[2];
};

struct command{
	unsigned char comm[2];
	unsigned char para[2];
};

struct gps{
	union{
		unsigned char uc[8];
		double d;
	};
	unsigned char latitude[8];
	unsigned char altitude[4];
};

struct message{
	unsigned char* message;
};

//int getMessageType(struct meditrik*);
//int getSequenceID(struct meditrik*);
//int getVersion(struct meditrik*);
void setMeditrikHeader(FILE*, struct meditrik*);
void setCommand(FILE*, struct command*, bool);
void setStatus(FILE*, struct status*);
void setGps(FILE*, struct gps*);
void setMessage(FILE*, struct message*, int); 






#endif
