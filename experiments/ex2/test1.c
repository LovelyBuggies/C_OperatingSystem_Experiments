#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/shm.h>
#define PERM S_IRUSR|S_IWUSR
#define MAX_SEQUENCE 10

// share_data structure
typedef struct 
{
    long sequence [MAX_SEQUENCE];
    int num;
    
}shared_data;

int main(int argc, char const **argv)
{
    key_t key;           // key of the memory segment
    pid_t pid;           // parent process id
    int count;           // the count of fibonacci
    shared_data* share_pointer;  // share pointer recording the begin address of share memory

    // create share memory error
    if( (key = shmget(IPC_PRIVATE, sizeof(shared_data), PERM)) == -1 ) {
        fprintf(stderr, "Create Share Memory Error:%s\n\a", strerror(errno));
        exit(1);
    }

    // initialize the fibonacci
    share_pointer = shmat(key, 0, 0);
    share_pointer->num = 0;

    // the count of fibonacci
    scanf("%d", &count);
    if(count < 1){
        printf("Input Invalid!\n");
        exit(0);
    }

    // start fibonacci loop
    for(int i = 0 ; i < count; i++){

        // fork to generate child process
        pid = fork();
        wait(NULL);

        // if is the child process
        if (pid == 0){
            
            // fibonacci algorithm
            if(share_pointer->num == 0){
                share_pointer->sequence[0] = 1;
                share_pointer->num++;
            }
            else if(share_pointer->num == 1){
                share_pointer->sequence[1] = 1;
                share_pointer->num++;
            }
            else{
                share_pointer->sequence[share_pointer->num] = 
                    share_pointer->sequence[share_pointer->num - 1] +
                    share_pointer->sequence[share_pointer->num - 2];

                share_pointer->num++;

            }
            exit(0);
        }
    }
    
    // output
    for(int i = 0; i < share_pointer->num; i++){
        printf("%ld\n", share_pointer->sequence[i]);
    }
}