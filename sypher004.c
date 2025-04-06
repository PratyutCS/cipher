#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#define R 5
int main() {
    struct timespec start, end1;
    clock_gettime(CLOCK_REALTIME, &start);
    int sBox[16] = {6 , 4 , 12 , 5 , 0 , 7 , 2 , 14 , 1 , 15 , 3 , 13 , 8 , 10 , 9 , 11};
    int permut[16] = {0 , 4 , 8 , 12 , 1 , 5 , 9 , 13 , 2 , 6 , 10 , 14 , 3 , 7 , 11 , 15};
    uint16_t key = 0;
    uint16_t msg = 0;
    do{
        uint16_t rkey[R+1];
        uint16_t temp;
        uint16_t key1 = key;
        for(int i = 0; i < R+1; i++){
            temp = (sBox[(key1 & 0xF)] & 0xF) | ((sBox[(key1 >> 4) & 0xF] & 0xF) << 4) | ((sBox[(key1 >> 8) & 0xF] & 0xF) << 8) | ((sBox[(key1 >> 12) & 0xF] & 0xF) << 12);
            uint16_t ret = 0;
            for (int i = 0; i < 16; i++) {
                if ((temp >> permut[i]) & 1)
                    ret |= (1 << i);
            }
            key1 =  ret & 0xffff;
            rkey[i] = key1;
        }
        do{
            uint16_t msg_c = msg;
            for(int i=0; i<R-1 ;i++){
                msg_c = msg_c ^ rkey[i];
                msg_c = (sBox[(msg_c & 0xF)] & 0xF) | ((sBox[(msg_c >> 4) & 0xF] & 0xF) << 4) | ((sBox[(msg_c >> 8) & 0xF] & 0xF) << 8) | ((sBox[(msg_c >> 12) & 0xF] & 0xF) << 12);
                uint16_t ret = 0;
                for (int i = 0; i < 16; i++) {
                    if ((msg_c >> permut[i]) & 1)
                        ret |= (1 << i);
                }
                msg_c = ret;
            }
            msg_c = msg_c ^ rkey[4];
            msg_c = (sBox[(msg_c & 0xF)] & 0xF) | ((sBox[(msg_c >> 4) & 0xF] & 0xF) << 4) | ((sBox[(msg_c >> 8) & 0xF] & 0xF) << 8) | ((sBox[(msg_c >> 12) & 0xF] & 0xF) << 12);
            msg_c = msg_c ^ rkey[5];
            // if(key == 0x0011 && msg == 0x0011){
            //     for(int i=0;i<R+1;i++){
            //         printf("rkey %d - %x\n",i , rkey[i]);
            //     }
            //     printf("key %u - PT %u - CT %x\n",key, msg, msg_c);
            // }
            msg++;
        }while(msg != 0);
        key++;
    }while(key != 0);
    clock_gettime(CLOCK_REALTIME, &end1);
    printf("[MAIN] executed in time: %f seconds\n", (end1.tv_sec - start.tv_sec) + (end1.tv_nsec - start.tv_nsec) / 1.0e9);
}