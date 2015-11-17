#include <stdio.h>
#include <stdlib.h>
#include "ethernetFrame.h"
#include "printHeader.h"

void setEthernetHeader(FILE* file, struct ethernetFrame* ethernetFrame){
        if (!file) return;
        fread(ethernetFrame->destinationIP, 6, 1, file);
        fread(ethernetFrame->sourceIP, 6, 1, file);
        fread(ethernetFrame->nextProtocol, 2, 1, file);
        
}
