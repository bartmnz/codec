#include <stdio.h>




int printHeader(unsigned char* buffer, int size){
        printf("\n\n");
        int i;
        int count=0;
        for(i = 0; i < size; i++){
                printf("%02X ",buffer[i]);
                if (count == 15){
                        printf("\n");
                        count = -1 ;
                }
                count++;
        }
        return 0;
}

