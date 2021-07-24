#ifndef IOT_PROTO_H_
#define IOT_PROTO_H_ 1

#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

// #define NUM_ITER_SYNC_REQ 5            //number of iterations needed to send a SYNC_REQ to father
#define SECONDS_FOR_DRIFT 60            //seconds(simulated) needed to have a drift of 1ms
#define NUM_ITER_SIM 2                //total number of loops for the simulation
#define NANO_SECONDS 1000000000          //ns in 1 second
#define MICRO_SECONDS 1000000          //us in 1 second
#define MILI_SECONDS 1000           //ms in 1 second
#define SIMU_TIME 0.050             //seconds simulated each NANO_EQ ns or each SEC_EQ seconds (clock simulated time in seconds)
#define NANO_EQ 200000000        //ns transcurred in SIMU_TIME 
#define SEC_EQ 1                //seconds transcurred in SIMU_TIME
#define TIME_OUT_SYNC 1000     //real time out for a sync_request in us (100000)(1000)
// #define SLEEP_TIME 25000000        //real sleep time for the device, before waiting for a sync_request, in ns
// #define RECALC_SIMU_TIME 60      //number to recalculate real time transcurred in SIMU_TIME
#define PROC_TIME 4             //time in ms to get a SYNC_REQ and send the SYNC_RESP
#define TX_RX_TIME 28              //time in ms from tx to rx and viceversa.

typedef struct {
    uint64_t time_counter;
    pthread_mutex_t counter_mtx;
    unsigned flag;
    pthread_cond_t cond_new_clock;
    pthread_cond_t cond_wait_sync_req;
    pthread_mutex_t new_clock_mtx;
    pthread_mutex_t wait_sync_req_mtx;
} ThreadArg_t;

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
    // printf("%llu \n",ntohll(pkg->payload.time_field.time_received));
    return ntohll(pkg->payload.time_field.time_received);
}

inline static uint64_t getTime_transmitted(const Msg *pkg)
{   
    return ntohll(pkg->payload.time_field.time_transmitted);
}

void printType(const Msg *m);
int sendMsg(int sockfd, const Msg *msg);
int recvMsg(int sockfd, Msg *msg);
int driftGenerator();
void sendTimes(int sockfd, uint64_t time_received,uint64_t time_transmitted);
// void paramsInit(ThreadArg_t *argThreadClock, pthread_t *thread_cr, void *function_thread);

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

#endif // IOT_PROTO_H_