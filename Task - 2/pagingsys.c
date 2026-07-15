#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(){

    // Configurable parts (Page size, Process Size, Memory Size)

    #define PROCESS_SIZE 8192 // Virtual Memory allocated to the process is 8 KB (8192 Bytes)
    #define MEM_SIZE 4096 // Physical Memory (RAM) is 4 KB (4096 Bytes)
    #define OFFSET_BIT 10 // 2^10 = 1024 Bytes (1 KB) per process page or main memory frame
}