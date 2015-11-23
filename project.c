#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "ethernetFrame.h"
#include "printHeader.h"
#include "ipV4.h"
#include "udp.h"
#include "meditrik.h"

int getMessageType( unsigned char*, int);
int getSequenceID(unsigned char*, int);
bool checkEndian(void);

int main(int argc, const char* argv[]){

// chekc if machine is little or big endian see here  http://stackoverflow.com/questions/19275955/convert-little-endian-to-big-endian
	bool isLE = checkEndian();
	unsigned char buffer[40];
	FILE* file;
	if (argc <= 1){
		file = fopen("status.pcap", "rb");	
	}else if (!(access(argv[1], F_OK & R_OK))){
		file = fopen(argv[1], "rb");
	} else{
		printf("ERROR: File does not exist or you do not have write permission\n");	
		return 1;
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
//	memset(udp, 0, sizeof(struct udpHeader));
	//*udp = {0};
	if(ipH->nextProtocol[0] == 0x11){
		setUdpHeader(file, udp);
	}else{
		printf("ERROR: not utilizing UDP protocol\n");
	}		

	getMeditrikHeader(file, isLE);

	free(udp);
	free(frameName);
	free(ipH);
	printf("\n\n\n");
	fclose(file);
}



bool checkEndian(void){
	int n = 1;
	return (*(char *)&n == 1); 
}















