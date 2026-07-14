#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/wait.h>

pthread_mutex_t turn_lock;
int current = 5 ; // Flight 5 gets the first turn 

void* descent(void* arg){
    int flight_id = *((int*)arg);
    int alt = 4000; // each airplane starts at an altitude of 4000 feet

    //The thread loops until the airplane has landed (i.e. alt = 0)
    while (alt != 0){

        // Acquiring the lock for scheduling state tracking memory
        pthread_mutex_lock(&turn_lock);

        if(current == flight_id){

            // Executing exactly one time quantum of descent (i.e. 1000 feet minus)
            alt -= 1000;
            printf("||RR Sched|| TQ Granted to Flight %d. Current Altitude = %d \n", flight_id,alt);

            if (alt == 0){
                printf("Flight %d successfully landed. \n", flight_id);
            }   

            sleep(3); // Simulating CPU core processingg the first slice

            // Updating the tracker to enforce circular execution. Observe {5,11,19}. For it to be circular execution
            // If current turn is 5, next turn should be 11. if current turn is 19, next turn should be 5.

            if (flight_id == 5){
                current = 11;
            }else if (flight_id == 11 ){
                current = 19;
            }else{
                current = 5 ;
            }

            printf("||RR Sched|| Turn passed to Flight %d \n",current);

            pthread_mutex_unlock(&turn_lock);

            sleep(1); //The ongoing threads moves away from CPU so the next airplane thread has an opportunity
            // to grab the lock.
        }
        else{
            // When it's not the airplanes' turn. The turn lock has to be released here otherwise the correct
            // thread will freeze on its lock statement 
        
            pthread_mutex_unlock(&turn_lock);
            sleep(1);
        }
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

        // Initializing the lock
        pthread_mutex_init(&turn_lock,NULL);

        //Threads are in 'Runnable' state
        for(int i = 0; i< 3; i++){
            
            // &flights[i] -> address where the thread id will be stored
            // NULL -> no specific features
            // descent ->  the function the thread will be running (In this case, the task is for airplanes
            // to descend)
            // &flight_ids[i] -> argument passed to 'safe_path' so as to track down the plane 

            pthread_create(&flights[i],NULL,descent,&flight_ids[i]);
        }

        // The child process only terminates when all of its threads successfully terminate
        for (int i = 0; i < 3; i++) {
            pthread_join(flights[i], NULL);
        }

        // Releasing the lock from resourcess
        pthread_mutex_destroy(&turn_lock);

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