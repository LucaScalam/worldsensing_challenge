#ifndef IOT_PROTO_H_
#define IOT_PROTO_H_ 1

#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

#define SECONDS_FOR_DRIFT 60           //seconds(simulated) needed to have a drift of 1ms
#define NUM_ITER_SIM 10                //Total minutes to be simulated
#define NANO_SECONDS 1000000000        //ns in 1 second
#define MICRO_SECONDS 1000000          //us in 1 second
#define MILI_SECONDS 1000              //ms in 1 second
#define SIMU_TIME 0.060                //seconds simulated each NANO_EQ ns or each SEC_EQ seconds (clock simulated time in seconds)
#define NANO_EQ 500000000              //ns transcurred in SIMU_TIME 
#define SEC_EQ 1                       //seconds transcurred in SIMU_TIME
#define TIME_OUT_SYNC 200000           //real time out for a sync_request in us (100000)(1000)
#define AUX_VAL 500

typedef struct {
    uint32_t time_counter;
    pthread_mutex_t counter_mtx;
    unsigned flag;
    pthread_mutex_t flag_mtx;
    pthread_cond_t cond_wait_sync_req;
    pthread_mutex_t wait_sync_req_mtx;
    int sock_father;
    int sock_child;
} ThreadArg_t;

typedef enum {
    TYPE_SYN_REQ,
    TYPE_SYN_1,
    TYPE_SYN_2,
    TYPE_SYN_CLOCK
} Type;

typedef struct __attribute__((__packed__)) 
{
    uint16_t sz8;
    uint8_t type;
} Header;

typedef struct __attribute__((__packed__)) 
{
    uint32_t time_field;
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
inline static uint32_t getTime_field(const Msg *pkg)
{   
    // printf("%llu \n",ntohll(pkg->payload.time_field.time_received));
    return ntohl(pkg->payload.time_field.time_field);
}


void printType(const Msg *m);
int sendMsg(int sockfd, const Msg *msg);
int recvMsg(int sockfd, Msg *msg);
void sendSync1(int sock_child);
void sendSync2(int sock_child,ThreadArg_t *thr_arg);

/***** Setters *****/

inline static void setRequest(Msg *msg)
{   
    msg->hdr.type = TYPE_SYN_REQ;
    msg->hdr.sz8 = htons(sizeof(Header));
}

inline static void setSync1(Msg *msg)
{   
    msg->hdr.type = TYPE_SYN_1;
    msg->hdr.sz8 = htons(sizeof(Header));
}

inline static void setSync2(Msg *msg, uint32_t clock_time)
{   
    msg->hdr.type = TYPE_SYN_2;
    msg->hdr.sz8 = htons(sizeof(Header) + sizeof(Time_field));
    msg->payload.time_field.time_field = htonl(clock_time);
}

inline static void setSynClock(Msg *msg)
{   
    msg->hdr.type = TYPE_SYN_CLOCK;
    msg->hdr.sz8 = htons(sizeof(Header));
}

#endif // IOT_PROTO_H_