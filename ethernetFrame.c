#include <stdio.h>
#include <stdlib.h>
#include "ethernetFrame.h"
#include "printHeader.h"





	




void setEthernetHeader(FILE* file, struct ethernetFrame* ethernetFrame){
        if (!file) return;
//	struct ethernetFrame* ethernetFrame = malloc(14);
        fread(ethernetFrame->destinationIP, 6, 1, file);
        fread(ethernetFrame->sourceIP, 6, 1, file);
        fread(ethernetFrame->nextProtocol, 2, 1, file);
        printHeader(ethernetFrame->destinationIP, 6);
//        free(ethernetFrame);
        //unsigned char ethernet[14];
        //        //fread(ethernet, sizeof(ethernet), 1, file);
        //                //printHeader(ethernet, 14);
        //                        //return 0;
}
