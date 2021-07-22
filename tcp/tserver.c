#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <net/if.h>
#include <sys/ioctl.h>
#include <pthread.h>

#include "tserver.h"
#include "tIOT_PROTO.h"

#define SIMU_TIME 6             //seconds simulated each NANO_SECONDS ns
#define NANO_EQ 500000          //ns transcurred in SIMU_TIME 
#define MAX_SECONDS 60          //time(in seconds) to get a drift of 1ms 
#define SIMULATION_TIME 60       //time of simulation in minutes

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sock_child;
    unsigned simulated_time = 0,count_time = MAX_SECONDS;
    fd_set rfds;
    struct timeval tv;
    int ret, err;
    Msg pkg;
    uint32_t time_received;
    ThreadArg_t argThreadClock;
    pthread_mutex_t clock_mtx;
    pthread_t thread_child;

    int mx = pthread_mutex_init(&clock_mtx, NULL);
    if( mx ) {
        char buff[64];
        strerror_r(mx,buff, sizeof(buff));
        printf("Problem in pthread_mutex_init()1: %s \n", buff);
        exit(-1);
    }
    argThreadClock.thr_arg_mtx = clock_mtx;
    argThreadClock.flag = 0;
    argThreadClock.time_counter = 0;

    if (argc < 2) {
       fprintf(stderr,"usage %s port \n", argv[0]);
       exit(0);
    }

    int rc = pthread_create(&thread_child,NULL,threadClock,(void *)&argThreadClock);
    if( rc ) {
        perror("pthread_create()");
        exit(-1);
    }

    sock_child = socketCreate_cli_side(argv[1]);

    tv.tv_sec = 1;
    // tv.tv_usec = 250;

    while(1){
        while(count_time != 0){
            FD_ZERO(&rfds);
            FD_SET(sock_child, &rfds);

            ret = select(sock_child+1, &rfds,NULL,NULL,&tv);
            if ( ret == -1 ){
                perror("while_main_client");
                return 0;
            }    

            if(FD_ISSET(sock_child, &rfds)){
    
                err = recvMsg(sock_child,&pkg);
                if (err == -1){
                    perror("ERROR recv msg");
                    break;
                }else if(err == 0){
                    perror("socket closed");
                    break;
                }

                if(getType(&pkg.hdr) == TYPE_SYN_REQ){
                    printf("Got SYNC_REQ. Sending times... \n");
                    time(&time_received);
                    sendTimes(sock_child, time_received);
                } 
            }else{
                printf("No Msg from client \n");
            }
        
            sleep(1-(tv.tv_sec));
            count_time--;
            printf("Count_time: %d \n", count_time);
        }

        if(simulated_time >= SIMULATION_TIME){
            break;
        }
        count_time = MAX_SECONDS;
        simulated_time += 1;
    }

    close(sock_child);
    return 0;
}

int socketCreate_cli_side(char *port){
    int sockfd, portno, val = 1,connfd;
    struct sockaddr_in serv_addr,cli_addr;
    socklen_t clilen;
    portno = atoi(port);

    printf("port: %d\n", portno);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
            sizeof(serv_addr)) < 0) 
            perror("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    connfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (connfd < 0) {
        printf("server acccept failed...\n");
        exit(0);
    }
    return connfd;
}

void sendTimes(int sockfd,time_t time_received){
    Msg pkg;
    time_t time_transmitted;
    time(&time_transmitted);
    // printf(" size of %lu \n",sizeof(time_received));
    // printf(" %ld \n",time_received);
    // printf(" %ld \n",time_transmitted);
    setTimes(&pkg,(uint64_t) time_received,(uint64_t) time_transmitted);
    // printf(" from msg: %llu \n",getTime_received(&pkg));
    sendMsg(sockfd,&pkg);
    printf("Times sent. \n");
}

void *threadClock(void *thr_arg){
    ThreadArg_t *thread_arg = (ThreadArg_t*)thr_arg;
    struct timespec time_value;
    time_value.tv_nsec = NANO_EQ;

    while(1){
        nanosleep(&time_value,NULL);
        thread_arg->time_counter++;
    }
}