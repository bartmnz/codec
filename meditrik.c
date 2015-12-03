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

void setIpHeader(FILE* file, struct ipv4Header* ipv4Header,int size,
						 unsigned char* first){
	
// change to if size > 20 fill out options
	if(size == 20){
		unsigned char temp[2];
		ipv4Header->verUC[0] = first[0] & 240;
		ipv4Header->hlenUC[0] = first[0] & 15;
		fread(ipv4Header->TOS, 1, 1, file);
		fread(ipv4Header->totalLength, 2, 1, file);
		fread(ipv4Header->identification, 2, 1, file);
		fread(temp, 2, 1, file);
		ipv4Header->flagUC[0] = temp[0] & 240;
		ipv4Header->foffUC[0] = temp[0] & 15;
		ipv4Header->foffUC[1] = temp[1];
		fread(ipv4Header->TTL, 1,1, file);
		fread(ipv4Header->nextProtocol, 1, 1, file);
		fread(ipv4Header->headerCheckSum, 2, 1, file);
		fread(ipv4Header->sourceAddress, 4, 1, file);
		fread(ipv4Header->destinationAddress, 4, 1, file);
	}
}
int getIpLen(unsigned char* bits, int size){
	if (size < 0 || bits == NULL){
		printf("ERROR: Invalid IP Length");
		return 0;
	}
	unsigned char leftSide = bits[0] >> 4;
	if (leftSide == 4){
		return 4 * (bits[0] & 15);
	}else{
		printf("ERROR: not IPV4\n");
	}
	return 0;
}

void setEthernetHeader(FILE* file, struct ethernetFrame* ethernetFrame){
        if (!file) return;
        fread(ethernetFrame->dstUC, 6, 1, file);
        fread(ethernetFrame->srcUC, 6, 1, file);
        fread(ethernetFrame->nxtUC, 2, 1, file);
        
}

int printHeader(unsigned char* buffer, int size){

	int i;
	int count=0;
	for(i = 0; i < size; i++){
	fprintf(stdout, "%02X ",buffer[i]);
	if (count == 15){
	printf("\n");
	count = -1 ;
	}
		count++;
	}
        return 0;
        
}

void setUdpHeader(FILE* file, struct udpHeader* udp){
	fread(udp->srcUC, 2, 1, file);
	fread(udp->dstUC, 2, 1, file);
	fread(udp->lenUC, 2, 1, file);
	fread(udp->chkUC, 2, 1, file);
}

void printMeditrik(struct frame* frmPtr, const char * fileName){
	FILE* file;
	if(! (file = fopen(fileName, "ab"))){
		exit(0);
	};
	fwrite( &(frmPtr->locPtr), 1, 16, file);
	//write ethernet header
	fwrite( &(frmPtr->ethPtr), 1, sizeof(struct ethernetFrame), file);
	printf("%d\n",(int) sizeof(struct udpHeader));
	//write ipv4 header
	frmPtr->ipPtr.verLen[0] = frmPtr->ipPtr.verUC[0] << 4;
	frmPtr->ipPtr.verLen[0] = frmPtr->ipPtr.verLen[0] | frmPtr->ipPtr.hlenUC[0];
	frmPtr->ipPtr.verLen[1] = frmPtr->ipPtr.TOS[0];
	frmPtr->ipPtr.ttlProt[0] = frmPtr->ipPtr.TTL[0];
	frmPtr->ipPtr.ttlProt[1] = frmPtr->ipPtr.nextProtocol[0];

	frmPtr->ipPtr.flagOff[0] = frmPtr->ipPtr.flagUC[0] << 5;
	frmPtr->ipPtr.flagOff[0] = frmPtr->ipPtr.flagOff[0] | frmPtr->ipPtr.foffUC[0];
	frmPtr->ipPtr.flagOff[1] = frmPtr->ipPtr.foffUC[1];
	
	fwrite( &(frmPtr->ipPtr), 1, 20, file);

	//write udp header

	fwrite( &(frmPtr->udpPtr), 1, sizeof(struct udpHeader), file);
	
	
	//write meditrick header
	unsigned char temp[2];
	frmPtr->medPtr.comboUC[0] = frmPtr->medPtr.comboUC[0] & 0;
	frmPtr->medPtr.comboUC[1] = frmPtr->medPtr.comboUC[1] & 0;
	frmPtr->medPtr.comboUC[1] = frmPtr->medPtr.typeUC[0] & 7;
	temp[1] = frmPtr->medPtr.seqUC[1] << 3;
	frmPtr->medPtr.comboUC[1] = frmPtr->medPtr.comboUC[1] | temp[1];
	temp[0] = frmPtr->medPtr.seqUC[1] >> 5;
	temp[1] = temp[1] & 0;
	temp[1] = frmPtr->medPtr.seqUC[0] << 3;
	temp[0] = temp[0] | temp[1];
	frmPtr->medPtr.comboUC[0] = frmPtr->medPtr.verUC[0] << 4;
	frmPtr->medPtr.comboUC[0] = frmPtr->medPtr.comboUC[0] | temp[0];
	

	fwrite( &(frmPtr->medPtr), 1, 12, file);

	if(frmPtr->medPtr.typeIN == 0){
		fwrite( &(frmPtr->stsPtr), 1, 14, file);
	} else if( frmPtr->medPtr.typeIN == 1){
		int size = ntohs(frmPtr->medPtr.lenIN);
	
		fwrite( &(frmPtr->cmdPtr), 1, size - 12, file);

	} else if( frmPtr->medPtr.typeIN == 2){
		fwrite( &(frmPtr->gpsPtr), 1, 20, file);
	} else if( frmPtr->medPtr.typeIN == 3){
		fwrite( frmPtr->msgPtr->message, 1, frmPtr->msgPtr->len, file);
	} else{
		fprintf(stderr, "ERROR: Invalid message type!");
		exit(0);
	}

	
	
	
	
	fclose(file);
}