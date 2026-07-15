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

    int requests_cpu[] = {1023,2028,3039,4050,5600,6700,7800,8900,10002};
    int no_requests = sizeof(requests_cpu) / sizeof(requests_cpu[0]);

    int pg_faults = 0; // Counter variable for page faults
    int pg_hits = 0; // Counter variable for page hits

    // Primary Paging System
    for (int k = 0; k < no_requests ; k ++) {
        int virtual_address = requests_cpu[k]; 
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

        // RAM HIT/MISS Evaluation before Page Replacement Algorithm

        bool hit = false;
        int frame_located = -1;

        for (int i = 0 ; i < NUM_FRAMES; i ++){
            if (physical_frames[i] == page_no){
                hit = true;
                frame_located = i;
                break;
            }
        }

        if (hit){
            pg_hits+=1;
            // Calculating the exact physical address
            int phys_addr = (frame_located*FRAME_SIZE) + page_offset; // note that page_offset equals frame_offset
            printf("||PAGE HIT|| Target resides in Frame No: %d \n",frame_located);
            printf("Physical Memory Mapping Location: %d \n", phys_addr);

        }else{
            pg_faults+=1;
            printf("||PAGE MISS|| Target page not found in RAM! \n");

            // Locating an empty frame slot
            bool found = false;
            for (int j = 0; j < NUM_FRAMES ; j++){
                if (physical_frames[j] == -1){
                    physical_frames[j] = page_no;
                    found = true;
                    int phys_addr = (j*FRAME_SIZE) + page_offset;

                    printf("Free slot found! Loaded Page No. %d into Frame No. %d \n", page_no,j);
                    printf("Physical Memory Mapping Location: %d \n", phys_addr);
                    break;
                }
            }

            if (!found){
                printf("RAM isn't empty!\n");
            }
        }
        printf("\n");
    }

    printf("Page Hit Ratio: %.2f%% \n", ((double)pg_hits / no_requests) * 100);
    printf("Page Fault Ratio: %.2f%% \n", ((double)pg_faults / no_requests) * 100);
}