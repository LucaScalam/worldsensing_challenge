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


void sendSync1(int sock_child){
    Msg pkg;
    setSync1(&pkg);
    sendMsg(sock_child,&pkg);
}
void sendSync2(int sock_child,ThreadArg_t *thr_arg){
    Msg pkg;
    uint32_t clock;
    
    pthread_mutex_lock(&thr_arg->counter_mtx);
    clock = thr_arg->time_counter;
    pthread_mutex_unlock(&thr_arg->counter_mtx);
    setSync2(&pkg,clock);
    sendMsg(sock_child,&pkg);
}
