#include "tIOT_PROTO.h"

//Creates socket in the server side
int socketCreate_serv_side(char *port, struct sockaddr_in *serv_addr);

//Creates socket in the server side
int socketCreate_cli_side(char* port);

//Sends packet to child with timestamps corresponding to the 
//received and transmitted times
void sendTimes(int sockfd,time_t time_received);

//Protocol executed by child to get a sync pkg from father
void child_protocol(int sockfd, const struct sockaddr *father_addr);

//Sync internal clock of the client with timestamps from the pkg
void update_internal_clock(time_t request_time,time_t response_time,uint64_t time_received,uint64_t time_transmitted);

//Function to simulate clock
void *threadClock(void *thr_arg);


/*
//Gets arguments from stdin and 
//creates pkg "setFileExecNoInter_CLIENT" to send
//to main_server
void getArgsSubmit(Msg *pkg, int sockfd);

//Gets arguments from stdin, creates socket to connect with exec_server 
//and creates pkg "setFileExecInter_CLIENT" to send
//to main_server
int getArgsSubmitInt(Msg *pkg, int sockfd);

//Gets response pkg from main_server
void client_protocol(int socket);

//Gets response from submitted job
void jobSubmittedResponse(Msg *pkg);

//Gets response from job state
void jobStateResponse(Msg *pkg);

//Gets response from queue state
void queueStateResponse(const Msg *pkg);

//Gets response from unsubmited job
void unsubmitResponse(const Msg *pkg);

//Parameters from msg sent were wrong
void badParamsResponse();

//blocks the client with accept() until
//exec_server connects throught socket "sock"
void client_accept(int sock);

//Creates and sends pkg with job state request
void JobState(Msg *pkg, char *to_jobid, int sockfd);

//Creates and sends pkg with queue state request
void QueueState(Msg *pkg, char *to_queue_idx, int sockfd);

//Creates and sends pkg with jobid to unsubmit
void unsubmit(Msg *pkg, char *to_jobid, int sockfd);

//function 1 to test iter jobs
void interaction_1(int sockfd);

*/