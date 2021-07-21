#ifndef IOT_PROTO_H_
#define IOT_PROTO_H_ 1

#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <arpa/inet.h>

typedef enum {
    TYPE_SYN_REQ,
    TYPE_SYN_RESP
} Type;

typedef struct __attribute__((__packed__)) 
{
    uint16_t sz8;
    uint8_t type;
} Header;

typedef struct __attribute__((__packed__)) 
{
    uint64_t time_received;
    uint64_t time_transmitted;
} Time_field;

typedef struct __attribute__((__packed__))
{
    Header hdr;
    union __attribute__((__packed__)) {
        Time_field time_field;
    } payload;
} Msg;

/***** Getters - Header *****/
inline static uint16_t getSize8(const Header *hdr)
{
    return ntohs(hdr->sz8);
}

inline static uint8_t getType(const Header *hdr)
{
    return hdr->type;
}

/***** Getters - Payload *****/
inline static uint64_t getTime_received(const Msg *pkg)
{   
    return ntohll(pkg->payload.time_field.time_received);
}

inline static uint64_t getTime_transmitted(const Msg *pkg)
{   
    return ntohll(pkg->payload.time_field.time_transmitted);
}

void printType(const Msg *m);
int sendMsg(int sockfd, const Msg *msg, const struct sockaddr *dest_addr, socklen_t addrlen);
int recvMsg(int sockfd, Msg *msg, struct sockaddr *src_addr, socklen_t *addrlen);

/***** Setters *****/

inline static void setTimes(Msg *msg, uint64_t time_received, uint64_t time_transmitted)
{   
    msg->hdr.type = TYPE_SYN_RESP;
    msg->hdr.sz8 = htons(sizeof(Header) + sizeof(Time_field));
    msg->payload.time_field.time_received = htonll(time_received);
    msg->payload.time_field.time_transmitted = htonll(time_transmitted);
}

inline static void setRequest(Msg *msg)
{   
    msg->hdr.type = TYPE_SYN_REQ;
    msg->hdr.sz8 = htons(sizeof(Header));
}

#endif // PROTO_DIST_QUEUE_H_