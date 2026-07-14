// Note: All of the code components have already been explained in each of the following files:
// deadlockprevention.c, landingwithsync.c, and roundrobin.c

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/wait.h>

pthread_mutex_t turn_lock;
int current = 5; 

pthread_mutex_t runway_lock;
pthread_mutex_t taxiway_lock;

void* airplane_lifecycle(void* arg) {
    int flight_id = *((int*)arg);
    int alt = 4000; 

    while (alt != 0) {
        pthread_mutex_lock(&turn_lock);

        if (current == flight_id) {
            alt -= 1000;
            printf("||RR Sched|| TQ Granted to Flight %d. Current Altitude = %d feet.\n", flight_id, alt);

            if (alt == 0) {
                printf("||CHILD PROCESS|| Flight %d successfully descended to ground level.\n", flight_id);
            }   

            sleep(1); 

            if (flight_id == 5) {
                current = 11;
            } else if (flight_id == 11) {
                current = 19;
            } else {
                current = 5;
            }

            printf("||RR Sched|| Turn passed to Flight %d.\n\n", current);
            pthread_mutex_unlock(&turn_lock);
            sleep(1); 
        }
        else {
            pthread_mutex_unlock(&turn_lock);
            sleep(1);
        }
    }

    printf("||CHILD PROCESS|| Flight %d nearing runway requesting runway lock...\n", flight_id);
    pthread_mutex_lock(&runway_lock);
    
    printf("||CHILD PROCESS|| Lock acquired by Flight %d. Initiating Landing...\n", flight_id);
    sleep(2);
    
    printf("||CHILD PROCESS|| Flight %d requesting taxiway lock...\n", flight_id);
    pthread_mutex_lock(&taxiway_lock);
    
    printf("||CHILD PROCESS|| Flight %d has locked the taxiway. Clearing off from the runway...\n", flight_id);
    sleep(2);

    printf("||CHILD PROCESS|| Flight %d completed both runway and taxi successfully.\n", flight_id);

    pthread_mutex_unlock(&taxiway_lock); 
    pthread_mutex_unlock(&runway_lock);  

    return NULL;
}

int main() {
    printf("||Parent Process||\n");

    pid_t pid = fork(); 

    if (pid == 0) { 
        printf("||Child Process|| Launching airplanes [threads]....\n");

        pthread_t flights[3]; 
        int flight_ids[3] = {5, 11, 19}; 

        pthread_mutex_init(&turn_lock, NULL);
        pthread_mutex_init(&runway_lock, NULL);
        pthread_mutex_init(&taxiway_lock, NULL);

        fflush(stdout);

        for (int i = 0; i < 3; i++) {
            pthread_create(&flights[i], NULL, airplane_lifecycle, &flight_ids[i]);
        }

        for (int i = 0; i < 3; i++) {
            pthread_join(flights[i], NULL);
        }

        pthread_mutex_destroy(&turn_lock);
        pthread_mutex_destroy(&runway_lock);
        pthread_mutex_destroy(&taxiway_lock);

        printf("||CHILD PROCESS|| Terminated.\n");
        return 0; 
    }
    else if (pid > 0) { 
        wait(NULL); 
        printf("||PARENT PROCESS|| Terminated.\n");
    
    } else { 
        printf("Fork failed its execution!\n");
    }

    return 0;
}