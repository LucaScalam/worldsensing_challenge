#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <unistd.h>
// #include <sys/types.h> 
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <pthread.h>
#include <assert.h>
#include <time.h>

#include "server.h"
#include "IOT_PROTO.h"

int main(int argc, char *argv[]){
    int socket_server, socket_client;
    char buff[256];
    int cmd;

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    
    socket_client = socketCreate(argv[1]);

    while(1){
        printf("Insert command: \n");
        bzero(buff,256);
        scanf("%s",buff);
        getchar();

        if ( strcmp(buff, "stop") == 0 ){
            break;
        } else if ( strcmp(buff, "t") == 0 ){

            printf("Total of jobs: %d \n",main_server.history_putidx);

        } else if ( strcmp(buff, "jobs") == 0 ){

            printHistoryJobs(&main_server);

        } else if ( strcmp(buff, "q") == 0 ){
            bzero(buff,256);
            scanf("%s",buff);
            cmd = atoi(buff);
            if ( cmd < 0 || cmd >= NUM_QUEUE_MAX ){
                printf("Bad command, try again \n");
            } else {
                printQueueJobs(main_server.queue[cmd]);
            }

        } else {

            printf("Bad command, try again \n");

        }
    }

    close(socket_client);
    return 0;
}
