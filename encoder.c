#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>


#include "meditrik.h"
#include "ethernetFrame.h"
#include "ipV4.h"
#include "udp.h"
#include "printHeader.h"
static const char Version[] = "Version";
static const char Sequence[] = "Sequence:";

void setVersion(char *);
void setSequence(char *);

int main( void){
	FILE* file;
	if(!(file = fopen("test.txt", "r"))) exit(0);
	char array[20];
	fgets(array, sizeof(array), file);


	printf("%ld\n", (long int)strstr(array, Version));
	printf("%ld\n", (long int)&array);
	printf("%s\n", array);


	setVersion(array);
	
	fgets(array, sizeof(array), file);
	setSequence(array);
//	if ((long int)&array == (long int)strstr(array, Version)){
//		printf("valid packet\n");
//	}
	
//	printf("%ld\n", (long int)strstr(array, Sequence));
//	printf("%ld\n", (long int)&array);
//	printf("%s\n", array);
	fclose(file);
}
/*
void makeHeader(FILE* file){

}
*/
void setVersion(char * array){
	printf("%ld\n", (long int)&*array);
	if ((long int)&*array == (long int)strstr(array, Version)){
                printf("valid packet\n");
        }else printf("Not supported\n");


	printf("%ld\n", (long int)strstr(array, Version));
        printf("%ld\n", (long int)&array);
        printf("%s\n", array);
}

void setSequence(char * array){
	if ((long int) &*array == (long int)strstr(array, Sequence)){
		printf("valid packet\n");
	}else printf("Invalid line: expected     Sequence:\n");
}
