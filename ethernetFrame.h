#ifndef ethernetFrame_H
	#define	ethernetFrame_H
	
#include <stdio.h>
	

struct ethernetFrame {
	unsigned char sourceIP[6];
	unsigned char destinationIP[6];
	unsigned char nextProtocol[2];
};

/*dst must be of size 14 
 *
 */

void setEthernetHeader(FILE*, struct ethernetFrame*);
#endif
