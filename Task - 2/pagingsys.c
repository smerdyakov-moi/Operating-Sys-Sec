#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

int main(){

    // Configurable parts (Page size, Process Size, Memory Size)

    #define PROCESS_SIZE 8192 // Virtual Memory allocated to the process is 8 KB (8192 Bytes)
    #define MEM_SIZE 4096 // Physical Memory (RAM) is 4 KB (4096 Bytes)
    #define OFFSET_BIT 10 // 2^10 = 1024 Bytes (1 KB) per process page or main memory frame

    // Calculation of page,frame sizes and no. of pages and frames 

    int NUM_PAGES = PROCESS_SIZE / (pow(2,OFFSET_BIT));
    int NUM_FRAMES = MEM_SIZE / (pow(2,OFFSET_BIT));

    int FRAME_SIZE = pow(2,OFFSET_BIT);
    int PAGE_SIZE = FRAME_SIZE; // In paging, frame size is always equal to page size.

    // Calculated Hardware Metrics
    printf("Configured Offset Allocation : %d bits\n", OFFSET_BIT);
    printf("Calculated Page/Frame Size   : %d bytes\n", PAGE_SIZE);
    printf("Configured Process Size      : %d bytes (Virtual Space)\n", PROCESS_SIZE);
    printf("Calculated Total Pages       : %d logical pages\n", NUM_PAGES);
    printf("Configured Main Memory Size  : %d bytes (Physical RAM)\n", MEM_SIZE);
    printf("Calculated Total Frames      : %d physical frames\n", NUM_FRAMES);
 
    
    // Physical RAM Allocation
    // Simulating our RAM by instantiating a physical main memory array containing our frame capacity (frame no.)
    
    int physical_frames[NUM_FRAMES];
    for (int i = 0 ; i < NUM_FRAMES ; i ++){
        physical_frames[i] = -1; // -1 means a completely empty frame slot
    }

    int requests_cpu[] = {4500,1051 ,2305, 4550, 6000,11000};
    int no_requests = sizeof(requests_cpu) / sizeof(requests_cpu[0]);

}