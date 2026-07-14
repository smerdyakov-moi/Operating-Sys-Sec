#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>

pthread_mutex_t runway_lock; 
pthread_mutex_t taxiway_lock; 

//The above two resources satisfy the mutual exclusion part of the deadlock requirement (Mutual Exclusion)

// Path 1: Lands the runway, proceeds to request the taxiway
void* flight_path_A(void* arg) {
    int flight_id = *((int*)arg);

    printf("||CHILD PROCESS|| Flight %d nearing runway requesting runway lock... \n",flight_id);
    pthread_mutex_lock(&runway_lock); // race condition handled by the runway_lock 
    printf("||CHILD PROCESS|| Flight %d holding runway lock...\n",flight_id); 

    // Hold & Wait: Thread holds runway lock while actively waiting for taxiwaylock
    // No Preemption: OS scheduler can't forcibly take the runway lock from the thread.

    sleep(3); //Simulate pause

    printf("||CHILD PROCESS|| Flight %d requesting taxiway lock... \n", flight_id);
    pthread_mutex_lock(&taxiway_lock); // permanently freezes here due to circular wait (Fulfills Deadlock: Hold and Wait)

    printf("||CHILD PROCESS|| Flight %d clearing out after successfull landing. \n",flight_id);

    // Following LIFO (Unlocking in reverse order I locked)
    pthread_mutex_unlock(&taxiway_lock);
    pthread_mutex_unlock(&runway_lock);

    return NULL;
}

//Path 2: Grabs the taxiway line, then requests for the runway (This fulfills the requirement for Deadlock:
    // Circular Wait)
void* flight_path_B(void* arg){
    int flight_id = *((int*)arg);

    printf("||CHILD PROCESS|| Flight %d nearing runway requesting taxiway lock... \n",flight_id);
    pthread_mutex_lock(&taxiway_lock); // race condition handled by the taxiway_lock but creates a deadlock
    printf("||CHILD PROCESS|| Flight %d holding taxiway lock...\n",flight_id); 

    sleep(3); //Idle

    printf("||CHILD PROCESS|| Flight %d requesting runway lock... \n", flight_id);
    pthread_mutex_lock(&runway_lock); // permanently freeze here due to circular wait. (Fulfills Deadlock: Hold and Wait)

    printf("||CHILD PROCESS|| Flight %d clearing out after successfull landing. \n",flight_id);

    // Following LIFO (Unlocking in reverse order I locked)
    pthread_mutex_unlock(&runway_lock);
    pthread_mutex_unlock(&taxiway_lock);
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
        
        // Mixed locking routing paths so that it causes circular wait condition
        pthread_create(&flights[0],NULL,flight_path_A,&flight_ids[0]); // holds runway lock
        pthread_create(&flights[1],NULL,flight_path_B,&flight_ids[1]); // holds taxiway lock
        pthread_create(&flights[2],NULL,flight_path_A,&flight_ids[2]);


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