#include<stdio.h>
#include<unistd.h>
#include<pthread.h> // Necessary library for working with threads in C
#include<stdbool.h> // Necessary library for true/false boolean logics in C
#include <sys/wait.h> // Necessary library for wait() {specifically used for parent process to wait for 
                        // the termination of its child process}


bool runway_occupied = false; // shared state         

pthread_mutex_t runway_lock; // declaring global binary lock structure. Essential in preventing race condition by
                        // allowing only one airplane/thread to run in its own critical section

// Helper function (Thread logic) for plane landing with synchronization             
void *airplane_landing(void *arg){
    
    int flight_id = *((int*)arg); // extracting the flight id of the current thread/airplane

    printf("||CHILD PROCESS|| Flight %d nearing runway requesting runway lock... \n",flight_id);
    
    // To fix the race condition from the previous case, I implemented a lock mechanism (MUTEX).
    // If the lock is held by another thread, the function stops the thread in its line of code, and the
    // OS moves it out of execution until the lock is freed.

    pthread_mutex_lock(&runway_lock);

    // Beginning of Critical Section
    printf("||CHILD PROCESS|| Lock acquired. Checking whether runway's occupied or not... \n");

    if (!runway_occupied){
 
        printf("||CHILD PROCESS|| Flight %d reads runway clear. Landing...\n",flight_id);

        runway_occupied = true;
        
        // Simulating time taken it is still in the runway before being cleared out.
        sleep(3);

        runway_occupied = false; // Meaning the plane has successfully left the runway after landing on it.
        printf("||Child Process|| FLight %d successfully completed its landing sequence. \n", flight_id);
    
    }else{
      
        printf("||CHILD PROCESS|| Runway Occupied. Flight %d on halt flying around",flight_id);
    }

    // Critical Section End
    // Release the acquired lock by the airplane so the other airplanes can successfully complete their landing sequence.
    
    pthread_mutex_unlock(&runway_lock);
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

        // Initializing the lock
        pthread_mutex_init(&runway_lock,NULL);

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

        // Releasing the lock from resourcess
        pthread_mutex_destroy(&runway_lock);

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