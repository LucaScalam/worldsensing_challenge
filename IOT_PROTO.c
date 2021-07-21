#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "IOT_PROTO.h"


void printType(const Msg *msg)
{
    printf("Type: %x\n", msg->hdr.type);
}

// Sends bytes of msg in an loop way until
// complete msg has been sent.
// returns 1 if everything was OK
// returns 0 if socket was closed
// returns -1 if there was an error
int sendMsg(int sockfd, const Msg *msg, const struct sockaddr *dest_addr, socklen_t addrlen)
{
    size_t toSend = ntohs(msg->hdr.sz8);
    ssize_t sent;
    uint8_t *ptr = (uint8_t *) msg;

    while( toSend ) {
        sent = sendto(sockfd, ptr, toSend, 0, dest_addr, addrlen);
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
int recvMsg(int sockfd, Msg *msg, struct sockaddr *src_addr, socklen_t *addrlen)
{
    size_t toRecv = sizeof(Header);
    ssize_t recvd;
    uint8_t *ptr = (uint8_t *) &msg->hdr;
    int headerRecvd = 0;
    while( toRecv ) {
        recvd = recvfrom(sockfd, ptr, toRecv, 0, src_addr, addrlen);
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

int recvMsg2(int sockfd, Msg *msg)
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