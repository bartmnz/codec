#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <math.h>

#include "meditrik.h"

void getMeditrikHeader(FILE* file, struct frame* frmPtr){
	memset(&(frmPtr->medPtr), 0, sizeof(struct meditrik));
	unsigned char temp[2];
	fread(temp, 2, 1, file);
	frmPtr->medPtr.verIN = 0;
	frmPtr->medPtr.verUC[0] = temp[0] >> 4;
	frmPtr->medPtr.seqUC[0] = (temp[0] & 8) >> 3;
	frmPtr->medPtr.seqUC[1] = (temp[1] >> 3) | ( temp[0] << 5);
	frmPtr->medPtr.typeUC[0] = temp[1] & 7;
	fread(frmPtr->medPtr.lenUC, 2, 1, file);
	fread(frmPtr->medPtr.srcUC, 4, 1, file);
	fread(frmPtr->medPtr.dstUC, 4, 1, file);
	if(frmPtr->medPtr.verIN != 1){
		fprintf(stdout,"ERROR: Unsupported version number!\n");
		return;
	}
	fprintf(stdout,"Version: %d\n", frmPtr->medPtr.verIN);
	frmPtr->medPtr.seqIN = ntohs(frmPtr->medPtr.seqIN);
	frmPtr->medPtr.srcIN = ntohl(frmPtr->medPtr.srcIN);
	frmPtr->medPtr.dstIN = ntohl(frmPtr->medPtr.dstIN);
	fprintf(stdout,"Sequence: %d\n",frmPtr->medPtr.seqIN);
	fprintf(stdout,"From: %u\n", frmPtr->medPtr.srcIN);
	fprintf(stdout,"To: %d\n", frmPtr->medPtr.dstIN);


	if(frmPtr->medPtr.typeIN == 0){
		getStatus(file, frmPtr);
	}else if(frmPtr->medPtr.typeIN == 1){
		getCommand(file, frmPtr);
	}else if(frmPtr->medPtr.typeIN == 2){
		getGps(file, frmPtr);
	}else if(frmPtr->medPtr.typeIN == 3){
		getMessage(file, frmPtr);
	}else{
 		fprintf(stderr,"ERROR: invalid message type");
		exit(0);
	}


}

void getCommand(FILE* file, struct frame* frmPtr){
	fread(frmPtr->cmdPtr.comUC, 2, 1, file);
	int szData = ntohs(frmPtr->medPtr.lenIN);	
	if(szData == 16)fread(frmPtr->cmdPtr.parUC, 2, 1, file);
	char* command[8] = {"GET STATUS", "Glucose", "GET GPS", 
			"Capsaicin","RESERVED", "Omorfine",
			"RESERVED",  "Sequence"};
	int index = (ntohs(frmPtr->cmdPtr.comIN)-1);
	fprintf(stdout,"%s", command[index]);
	if(szData == 16) fprintf(stdout,"=%d\n", ntohs(frmPtr->cmdPtr.parIN));
	else fprintf(stdout,"\n");

}

void getStatus(FILE* file, struct frame* frmPtr){ 
	fread(frmPtr->stsPtr.batUC, 8, 1, file);
	fread(frmPtr->stsPtr.gluUC, 2, 1, file);
	fread(frmPtr->stsPtr.capUC, 2, 1, file);
	fread(frmPtr->stsPtr.omoUC, 2, 1, file);
	
	fprintf(stdout, "Battery: %.2f%% \n", frmPtr->stsPtr.batDB);
	fprintf(stdout,"Glucose: %d\n", ntohs(frmPtr->stsPtr.gluIN));
	fprintf(stdout,"Capsaicin: %d\n", ntohs(frmPtr->stsPtr.capIN));
	fprintf(stdout,"Omorfine: %d\n", ntohs(frmPtr->stsPtr.omoIN));
}

void getGps(FILE* file, struct frame* frmPtr){
	fread(frmPtr->gpsPtr.longUC, 8, 1, file);
	fread(frmPtr->gpsPtr.latiUC, 8, 1, file);
	fread(frmPtr->gpsPtr.altiUC, 4, 1, file);

	char direction;
	frmPtr->gpsPtr.latiDB > 0 ? (direction = 'N') : (direction = 'S');
	fprintf(stdout, "Latitude: %.9f deg. %c\n", fabs(frmPtr->gpsPtr.latiDB), direction);
	frmPtr->gpsPtr.longDB > 0 ? (direction = 'W') : (direction = 'E');
	fprintf(stdout, "Longitude: %.9f deg. %c\n", fabs(frmPtr->gpsPtr.longDB), direction);
	fprintf(stdout, "Altitude: %.0f ft. \n", frmPtr->gpsPtr.altiDB * 6); // stored as fathoms 

}

void getMessage(FILE* file, struct frame* frmPtr){
	unsigned char zero = '\0';
	int size = ntohs(frmPtr->medPtr.lenIN);
	fread(frmPtr->msgPtr->message, size-12, 1, file);
	frmPtr->msgPtr->message[size-1] = '\n';
	fprintf(stdout,"Message: %s%c\n", frmPtr->msgPtr->message, zero);

}

void setIpHeader(FILE* file, struct ipv4Header* ipv4Header,int size,
						 unsigned char* first){
	
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
		fprintf(stderr, "ERROR: Invalid IP Length");
		return 0;
	}
	unsigned char leftSide = bits[0] >> 4;
	if (leftSide == 4){
		return 4 * (bits[0] & 15);
	}else{
		fprintf(stderr, "ERROR: not IPV4\n");
	}
	return 0;
}

void setEthernetHeader(FILE* file, struct ethernetFrame* ethernetFrame){
        if (!file){
        	return;
        } 
        fread(ethernetFrame->dstUC, 6, 1, file);
        fread(ethernetFrame->srcUC, 6, 1, file);
        fread(ethernetFrame->nxtUC, 2, 1, file);
        
}
// Below method used fo debugging
/* 
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
*/
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
	fwrite( &(frmPtr->ethPtr), 1, sizeof(struct ethernetFrame), file);

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