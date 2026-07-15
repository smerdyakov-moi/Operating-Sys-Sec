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
    printf("Configured Offset Allocation = %d bits\n", OFFSET_BIT);
    printf("Calculated Page/Frame Size   = %d bytes\n", PAGE_SIZE);
    printf("Configured Process Size      = %d bytes (Virtual Space)\n", PROCESS_SIZE);
    printf("Calculated Total Pages       = %d logical pages\n", NUM_PAGES);
    printf("Configured Main Memory Size  = %d bytes (Physical RAM)\n", MEM_SIZE);
    printf("Calculated Total Frames      = %d physical frames\n", NUM_FRAMES);
 
    
    // Physical RAM Allocation
    // Simulating our RAM by instantiating a physical main memory array containing our frame capacity (frame no.)
    
    int physical_frames[NUM_FRAMES];
    for (int i = 0 ; i < NUM_FRAMES ; i ++){
        physical_frames[i] = -1; // -1 means a completely empty frame slot
    }

    int requests_cpu[] = {4500,1051 ,2305, 4550, 6000,11000};
    int no_requests = sizeof(requests_cpu) / sizeof(requests_cpu[0]);

    // Primary Paging System
    for (int i = 0; i < no_requests ; i ++) {
        int virtual_address = requests_cpu[i]; 
        printf("CPU demanding access to virtual address: %d \n",virtual_address);

        // Checking whether the virtual address requested is within the boundary of process size or not
        if (virtual_address >= PROCESS_SIZE){
            printf("Address out of process bounds ! \n");
            continue;
        }

        // Bitwise Translation - Taught in class lectures
        // Process Size = 8192 (i.e. 2 ^ 13)
        // 13 = Page NO(bit) + Page Offset(bit)
        // We already know that Page Offset is 10 bits, so remaining 3 bit is page number
        
        // To calculate the page number and page offset
        
        // Calculating the page offset is just the modulo operation, whereas calculating page no. is
        // division operaation. For demonstration:
        // Process Size = 8 byte, Page size = 2 byte, No. of Pages = 4
        // If CPU requests 4. Following Bitwise Translation
        // 3 = Pg NO + Pg Off (Pg Off = 1 bit because 2^1 = 2 byte (page size)), Pg no = 4/2 -> 2
        // Pg Off = 4%2 = 0 (True)

        int page_no = virtual_address / PAGE_SIZE;

        int page_offset = virtual_address  % PAGE_SIZE ;

        printf("Page Number = %d, Page Offset = %d \n", page_no, page_offset);


    }
}