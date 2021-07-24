#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "tIOT_PROTO.h"


void printType(const Msg *msg)
{
    printf("Type: %x\n", msg->hdr.type);
}

// Sends bytes of msg in an loop way until
// complete msg has been sent.
// returns 1 if everything was OK
// returns 0 if socket was closed
// returns -1 if there was an error
int sendMsg(int sockfd, const Msg *msg)
{
    size_t toSend = ntohs(msg->hdr.sz8);
    ssize_t sent;
    uint8_t *ptr = (uint8_t *) msg;

    while( toSend ) {
        sent = send(sockfd, ptr, toSend, 0);
        if( (sent == -1 && errno != EINTR) || sent == 0 )
            return sent;
        toSend -= sent;
        ptr += sent;
    }
    return 1;
}

// Msg is recieved. Size of msg is not known.
// First, header is recieved to know sz of msg.
// Second, arguments are recieved.
// returns 1 if everything was OK
// returns 0 if socket was closed
// returns -1 if there was an error
int recvMsg(int sockfd, Msg *msg)
{
    size_t toRecv = sizeof(Header);
    ssize_t recvd;
    uint8_t *ptr = (uint8_t *) &msg->hdr;
    int headerRecvd = 0;
    while( toRecv ) {
        recvd = recv(sockfd, ptr, toRecv, 0);
        if( (recvd == -1 && errno != EINTR) || recvd == 0 )
            return recvd;
        toRecv -= recvd;
        ptr += recvd;
        if( toRecv == 0 && headerRecvd == 0) {
            headerRecvd = 1;
            ptr = (uint8_t *) &msg->payload;
            toRecv = ntohs(msg->hdr.sz8) - sizeof(Header);
        }
    }
    return 1;
}


/***** Shared functions *****/

//Generates a drift of 10, positive or
//negative
int driftGenerator(){
    // if(rand() % 2 == 0){
    if(2 % 2 == 0){
        return 10;
    }else{
        return -10;
    }

}

//Sends packet to child, where timestamps for 
//reception and transmission are contained
void sendTimes(int sockfd, uint32_t time_received,uint32_t time_transmitted){
    Msg pkg;
    // uint32_t time_transmitted;
    // time_transmitted = time_received;
    setTimes(&pkg,time_received, time_transmitted);
    sendMsg(sockfd,&pkg);
    printf("Times sent. \n");
}



/*
//Initialization of parameters for the thread that will work as a clock
void paramsInit(ThreadArg_t *argThreadClock, pthread_t *thread_cr, void *function_thread){

    argThreadClock->flag = 0;
    argThreadClock->time_counter = 0;
    printf("hola \n");
    int condv = pthread_cond_init(&argThreadClock->cond_new_clock,NULL);
    if( condv ) {
        printf("ERROR; return code from pthread_cond_init() is %d\n", condv);
        exit(-1);
    }
    condv = pthread_cond_init(&argThreadClock->cond_wait_sync_req,NULL);
    if( condv ) {
        printf("ERROR; return code from pthread_cond_init() is %d\n", condv);
        exit(-1);
    }
    int mx = pthread_mutex_init(&argThreadClock->new_clock_mtx, NULL);
    if( mx ) {
        char buff[64];
        strerror_r(mx,buff, sizeof(buff));
        printf("Problem in pthread_mutex_init()1: %s \n", buff);
        exit(-1);
    }
    mx = pthread_mutex_init(&argThreadClock->wait_sync_req_mtx, NULL);
    if( mx ) {
        char buff[64];
        strerror_r(mx,buff, sizeof(buff));
        printf("Problem in pthread_mutex_init()2: %s \n", buff);
        exit(-1);
    }

    mx = pthread_mutex_init(&argThreadClock->counter_mtx, NULL);
    if( mx ) {
        char buff[64];
        strerror_r(mx,buff, sizeof(buff));
        printf("Problem in pthread_mutex_init()3: %s \n", buff);
        exit(-1);
    }

    int rc = pthread_create(thread_cr,NULL,function_thread,(void *)&argThreadClock);
    if( rc ) {
        perror("pthread_create()");
        exit(-1);
    }



*/