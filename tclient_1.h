#include "tIOT_PROTO.h"

//Creates socket in the server side
int socketCreate_serv_side(char *port, struct sockaddr_in *serv_addr);

//Creates socket in the client/child side
int socketCreate_cli_side(char* port);

//Protocol executed by child to get a sync pkg from father
void child_protocol(int sock_server,int sock_child,ThreadArg_t *thr_arg);

//Sync internal clock of the client with timestamps from the pkg
void update_internal_clock(uint32_t request_time,uint32_t response_time,uint32_t time_received,uint32_t time_transmitted,ThreadArg_t *thr_arg,int sock_child);

//Function to execute once the thread for the clock is created
void *threadClock_client(void *thr_arg);

