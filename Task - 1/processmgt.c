#include<stdio.h>
#include<unistd.h>
#include<pthread.h> // Necessary library for working with threads in C
#include<stdbool.h> // Necessary library for true/false boolean logics in C
#include <sys/wait.h> // Necessary library for wait() {specifically used for parent process to wait for 
                        // the termination of its child process}


void *airplane_landing(void *arg){
    sleep(3);
    printf("Successfully terminating..\n");
    return NULL;
}

int main(){

    printf("||Parent Process||");

    // fork() clones existing process (parent process) inside where I will be running my concurrent threads.
    pid_t pid = fork(); 

    if(pid == 0){ // Successfull process creation. PID of 0 denotes the child process
        printf("||Child Process|| Launching airplanes [threads]....");

        pthread_t flights[3]; // Creates three distinct thread objects (airplanes) in memory, with each
                                // of the three threads being in 'New' state theoretically.
        
        int flight_ids[3] = {5,11,19}; // For tracking the airplanes

        //Threads are in 'Runnable' state
        for(int i = 0; i< 3; i++){
            
            // &flights[i] -> address where the thread id will be stored
            // NULL -> no specific features
            // airplane_landing ->  the function the thread will be running (In this case, the task is for airplane
            // to be landing on the runway)
            // &flight_ids[i] -> argument passed to 'airplane_landing' so as to track down the plane 

            pthread_create(&flights[i],NULL,airplane_landing,&flight_ids[i]);
        }

        // The child process only terminates when all of its threads successfully terminate
        for (int i = 0; i < 3; i++) {
            pthread_join(flights[i], NULL);
        }

        printf("||CHILD PROCESS|| Terminated.");
        return 0; // Termination of the child process
    }
    else if (pid >0){ // PID > 0 denotes parent process
        wait(NULL);
        printf("||PARENT PROCESS|| Terminated.");
    }else{ // PID <0 indicates that there was some issue/error with creating the child process
        printf("Fork failed its execution!");
    }

    return 0;
}