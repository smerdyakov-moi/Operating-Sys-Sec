#include<stdio.h>
#include<unistd.h>
#include<pthread.h> // Necessary library for working with threads in C
#include<stdbool.h> // Necessary library for true/false boolean logics in C
#include <sys/wait.h> // Necessary library for wait() {specifically used for parent process to wait for 
                        // the termination of its child process}


bool runway_occupied = false; // shared state                        

// Helper function (Thread logic) for plane landing without synchronization (MUTEX)                    
void *airplane_landing(void *arg){
    
    int flight_id = *((int*)arg); // extracting the flight id of the current thread/airplane

    printf("||CHILD PROCESS|| Flight %d nearing runway. Reading runway_occupied = %d... \n",flight_id,runway_occupied);

    if (!runway_occupied){
        printf("||CHILD PROCESS|| Flight %d reads runway clear. Landing...\n",flight_id);

        // To simulate a race condition here, I run a sleep() meaning the CPU will switch its context to another
        // thread. The other thread still reads the runway_occupied as false, causing a collision.

        sleep(3); // Simulates 3 seconds of idle time

        runway_occupied = true;
        printf("Collision!! Flight %d collided on an active runway! \n",flight_id);

        sleep(2); // Simulating actively landing on the airway and then leaving the runway 

        runway_occupied = false; // Meaning the plane has successfully left the runway after landing on it.

    }else{
        printf("||CHILD PROCESS|| Runway Occupied. Flight %d on halt flying around",flight_id);
    }

    return NULL;

}

int main(){

    printf("||Parent Process||\n");

    // fork() clones existing process (parent process) inside where I will be running my concurrent threads.
    pid_t pid = fork(); 

    if(pid == 0){ // Successfull process creation. PID of 0 denotes the child process
        printf("||Child Process|| Launching airplanes [threads].... \n");

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

        printf("||CHILD PROCESS|| Terminated. \n");
        return 0; // Termination of the child process
    }
    else if (pid >0){ // PID > 0 denotes parent process

        wait(NULL); //makes the parent process wait for the termination of child process
        printf("||PARENT PROCESS|| Terminated. \n");
    
    }else{ // PID <0 indicates that there was some issue/error with creating the child process
    
        printf("Fork failed its execution! \n");
 
    }

    return 0;
}