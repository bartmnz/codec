#include <stdio.h>
#include <stdbool.h>
#include "printHeader.h"
#ifndef MEDITRIK_H
	#define MEDITRIK_H


struct meditrik{
	union{
		unsigned char verUC[1];
		int verIN;
	};
	union{
		unsigned char seqUC[2];
		int seqIN;
	};
	union{
		unsigned char typeUC[1];
		int typeIN;
	};
	union{
		unsigned char lenUC[2];
		int lenIN;
	};
	union{
		unsigned char srcUC[4];
		unsigned int srcIN;
	};
	union{
		unsigned char dstUC[4];
		int dstIN;
	};
};

struct status{
	union{
		unsigned char batUC[8];
		double batDB;
	};
	union{
		unsigned char gluUC[2];
		int gluIN;
	};
	union{
		unsigned char capUC[2];
		int capIN;
	};
	union{
		unsigned char omoUC[2];
		int omoIN;
	};
};

struct command{
	union{
		unsigned char comUC[2];
		int comIN;
	};
	union{
		unsigned char parUC[2];
		int parIN;
	};
};

struct gps{
	union{
		unsigned char longUC[8];
		double longDB;
	};
	union{
		unsigned char latiUC[8];
		double latiDB;
	};
	union{
		unsigned char altiUC[4];
		float altiDB;
	};
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
