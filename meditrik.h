#include <stdio.h>
#include <stdbool.h>

#ifndef MEDITRIK_H
	#define MEDITRIK_H




struct ethernetFrame {
	unsigned char sourceIP[6];
	unsigned char destinationIP[6];
	unsigned char nextProtocol[2];
};

struct ipv4Header{
	unsigned char version[1];
	unsigned char headerLength[1];
	unsigned char TOS[1];
	unsigned char totalLength[2];
	unsigned char identification[2];
	unsigned char flags[1];
	unsigned char fragmentOffset[2];
	unsigned char TTL[1];
	unsigned char nextProtocol[1];
	unsigned char headerCheckSum[2];
	unsigned char sourceAddress[4];
	unsigned char destinationAddress[4];
	//assign dynamically	unsigned char options[40];
	
};


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
	size_t len;
	unsigned char message[];
};

struct udpHeader{
	unsigned char sourcePort[2];
	unsigned char destinationPort[2];
	unsigned char length[2];
	unsigned char checksum[2];
};

struct frame{
	struct ethernetFrame ethPtr;
	struct ipv4Header ipPtr;
	struct udpHeader udpPtr;
	struct meditrik medPtr;
	union{
		struct command cmdPtr;
		struct gps gpsPtr;
	//	struct message msgPtr;
		struct status stsPtr;
	};
};

//int getMessageType(struct meditrik*);
//int getSequenceID(struct meditrik*);
//int getVersion(struct meditrik*);
void getMeditrikHeader(FILE*, bool);
void getCommand(FILE*, bool);
void getStatus(FILE*); 
void getGps(FILE*);
void getMessage(FILE*, int); 
void setEthernetHeader(FILE*, struct ethernetFrame*);

void setIpHeader(FILE*, struct ipv4Header*, int, unsigned char*);

int getIpLen(unsigned char*, int);
void setUdpHeader(FILE*, struct udpHeader*);
int printHeader(unsigned char*, int, const char*);

#endif
