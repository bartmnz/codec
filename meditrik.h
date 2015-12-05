#include <stdio.h>
#include <stdbool.h>

#ifndef MEDITRIK_H
	#define MEDITRIK_H

struct globalHeader{
	union{
		unsigned char magicNum[4];
		unsigned int magicIN;
	};
	union{
		unsigned char magVerUC[2];
		short magVerSH;
	};
	union{
		unsigned char minVerUC[2];
		short minVerSH;
	};
	unsigned char zeros[16];
};

struct localHeader{
	unsigned char zeros[8];
	union{
		unsigned char length[4];
		int lenIN;
	};
	union{
		unsigned char nxtLen[4];
		int nxtLenIN;
	};
};

struct ethernetFrame {
	union{
		unsigned char srcUC[6];
		short srcIN[3];
	};
	union{
		unsigned char dstUC[6];
		short dstIN[3];
	};
	union{
		unsigned char nxtUC[2];
		unsigned short nxtIN;
	};
};

struct ipv4Header{
	unsigned char verLen[2];
	union{
		unsigned char totalLength[2];
		unsigned short lenIN;
	};
	union{
		unsigned char identification[2];
		unsigned short idIN;	
	};
	unsigned char flagOff[2];
	unsigned char ttlProt[2];
	union{
		unsigned char headerCheckSum[2];
		unsigned short chkIN;
	};
	union{
		unsigned char sourceAddress[4];
		unsigned int srcLN;
	};
	union{
		unsigned char destinationAddress[4];
		unsigned int dstLN;
	};
	union{
		unsigned char verUC[1];
		int verSH;
	};
	union{
		unsigned char hlenUC[1];
		short hlenSH;
	};
	union{
		unsigned char TOS[1];
		short tosSH;
	};
	union{
		unsigned char flagUC[1];
		short flagSH;
	};
	union{
		unsigned char foffUC[2];
		int foffIN;
	};
	union{
		unsigned char TTL[1];
		short ttlSH;
	};
	union{
		unsigned char nextProtocol[1];
		short nxpSH;
	};
	
	
};

struct udpHeader{
	union{
		unsigned char srcUC[2];
		unsigned short srcSH;
	};
	union{
		unsigned char dstUC[2];
		unsigned short dstSH;
	};
	union{
		unsigned char lenUC[2];
		unsigned short lenSH;
	};
	union{
		unsigned char chkUC[2];
		unsigned short chkSH;
	};
};

struct meditrik{
	unsigned char comboUC[2];
	union{
		unsigned char lenUC[2];
		unsigned short lenIN;
	};
	union{
		unsigned char srcUC[4];
		unsigned int srcIN;
	};
	union{
		unsigned char dstUC[4];
		int dstIN;
	};
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
};

struct status{
	union{
		unsigned char batUC[8];
		double batDB;
	};
	union{
		unsigned char gluUC[2];
		unsigned short gluIN;
	};
	union{
		unsigned char capUC[2];
		unsigned short capIN;
	};
	union{
		unsigned char omoUC[2];
		unsigned short omoIN;
	};
};

struct command{
	union{
		unsigned char comUC[2];
		unsigned short comIN;
	};
	union{
		unsigned char parUC[2];
		unsigned short parIN;
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
	int len;
	unsigned char message[];
};

struct frame{
	struct localHeader locPtr;
	struct ethernetFrame ethPtr;
	struct ipv4Header ipPtr;
	struct udpHeader udpPtr;
	struct meditrik medPtr;
	union{
		struct command cmdPtr;
		struct gps gpsPtr;
		struct status stsPtr;
	};
	struct message* msgPtr;
};

void getMeditrikHeader(FILE*, struct frame*);
void getCommand(FILE*, struct frame*);
void getStatus(FILE*, struct frame*); 
void getGps(FILE*, struct frame*);
void getMessage(FILE*, struct frame*); 
void setEthernetHeader(FILE*, struct ethernetFrame*);
void setIpHeader(FILE*, struct ipv4Header*, int, unsigned char*);
void printMeditrik(struct frame*, const char *);
int getIpLen(unsigned char*, int);
void setUdpHeader(FILE*, struct udpHeader*);
int printHeader(unsigned char*, int);

#endif
