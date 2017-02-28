#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/sem.h>
#include <unistd.h>
#include <string.h>

#include <errno.h>
/** Shared Memory and Semaphores**/
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

/** Semaphores  **/
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
//And signal
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

int main()
{
    int pid, status;
    if ((pid = fork()) < 0){
        perror("Could not fork, run gazebo-streamer manually");
    }
    else if (pid==0){
            //Child process run gazebo-streamer
           // execl("/home/michele/tests/3118858/sem_server","sem_server", (char*)0);
            execl("/home/michele//gst-rtsp-server-1.2.3/examples/run_gazebo_streamer.sh","gazebo-streamer", (char*)0);
            _exit(127);
    }

    printf("Running child pid=%d", pid);
    sleep(50);

    for (;;) {
            // Remove the zombie process, and get the pid and return code
            pid = wait(&status);
            if (pid < 0) {
                if (errno == ECHILD) {
                    printf("All children have exited\n");
                    break;
                }
                else {
                    perror("Could not wait");
                }
            }
            else {
                printf("Child %d exited with status %d\n", pid, status);
            }
        }
}
