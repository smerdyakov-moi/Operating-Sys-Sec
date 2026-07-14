#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>

pthread_mutex_t runway_lock; 
pthread_mutex_t taxiway_lock; 
//The above two resources satisfy the mutual exclusion part of the deadlock requirement (Mutual Exclusion)


// Deadlock Prevention:
// One can tackle one of the four conditions to prevent deadlock, but the most feasible one is 
// Circular Wait because of its minimal system complexity compared to other three.

// To prevent deadlock, all threads execute exactly one function in sequential order: Runway -> TaxiWay.
void *safe_path(void *arg){
    int flight_id = *((int*)arg);

    printf("||CHILD PROCESS|| Flight %d nearing airport. Requesting runway lock...\n",flight_id);

    pthread_mutex_lock(&runway_lock); 
    printf("||CHILD PROCESS|| Flight %d has locked the runway. Landing...\n",flight_id);

    sleep(3); // Simulating context switch to show that this holds regardless of heavy OS scheduling

    pthread_mutex_lock(&taxiway_lock); // Breaking the circular wait by requiring runway to be held first
    printf("||CHILD PROCESS|| Flight %d has locked the taxiway. Clearing off from the runway...\n",flight_id);

    sleep(3); // Simulation of time taken to clear the plane

    printf("||CHILD PROCESS|| Flight %d completed both runway and taxi successfully. \n",flight_id);

    pthread_mutex_unlock(&taxiway_lock);
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
        pthread_mutex_init(&taxiway_lock,NULL);

        //Threads are in 'Runnable' state
        for(int i = 0; i< 3; i++){
            
            // &flights[i] -> address where the thread id will be stored
            // NULL -> no specific features
            // safe_path ->  the function the thread will be running (In this case, the task is for 
            // sequential execution of waiting for runway, then taxiway
            // &flight_ids[i] -> argument passed to 'safe_path' so as to track down the plane 

            pthread_create(&flights[i],NULL,safe_path,&flight_ids[i]);
        }

        // The child process only terminates when all of its threads successfully terminate
        for (int i = 0; i < 3; i++) {
            pthread_join(flights[i], NULL);
        }

        // Releasing the lock from resourcess
        pthread_mutex_destroy(&runway_lock);
        pthread_mutex_destroy(&taxiway_lock);

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