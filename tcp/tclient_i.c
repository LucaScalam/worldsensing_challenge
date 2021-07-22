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

#include "tclient_i.h"
#include "tIOT_PROTO.h"

#define MAX_SECONDS 5       //time(in seconds) to get a drift of 1ms 
#define SIMULATION_TIME 5       //time of simulation in minutes

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sock_father,sock_child = 0;
    unsigned simulated_time = 0,count_time = MAX_SECONDS;
    fd_set rfds;
    struct timeval tv;
    int ret, err;
    unsigned int len;
    Msg pkg;
    time_t time_received;
    struct sockaddr_in child_addr, father_addr;

    len = sizeof(child_addr);

    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port \n", argv[0]);
       exit(0);
    }

    sock_father = socketCreate_serv_side(argv[1],&father_addr);
    // sock_child = socketCreate_cli_side(argv[2]);

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
                    sendTimes(sock_child, time_received,(struct sockaddr *)&child_addr,len);
                } 
            }else{
                printf("No Msg from client \n");
            }
        
            sleep(1-(tv.tv_sec));
            count_time--;
            printf("Count_time: %d \n", count_time);
        }

        //Drift of 1ms
        child_protocol(sock_father,(const struct sockaddr *)&father_addr);

        if(simulated_time >= SIMULATION_TIME){
            break;
        }
        count_time = MAX_SECONDS;
        simulated_time += 1;
    }

    close(sock_father);
    // close(sock_child);
    return 0;
}
  
int socketCreate_serv_side(char *port, struct sockaddr_in *serv_addr){
    int sockfd, portno;
    struct hostent *server;
    

    portno = atoi(port);
    server = gethostbyname("localhost");

    printf("port: %d\n", portno);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    bzero((char *) serv_addr, sizeof(serv_addr));
    serv_addr->sin_family = AF_INET;
    // serv_addr->sin_addr.s_addr = INADDR_ANY;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr->sin_addr.s_addr,
         server->h_length);
    serv_addr->sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *) serv_addr,sizeof(*serv_addr)) < 0) 
        error("ERROR connecting");

    return sockfd;
}

int socketCreate_cli_side(char *port){
    int sockfd, portno, val = 1;
    struct sockaddr_in serv_addr;

    portno = atoi(port);

    printf("port: %d\n", portno);
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
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

    return sockfd;
}

void sendTimes(int sockfd,time_t time_received){
    Msg pkg;
    time_t time_transmitted;
    time(&time_transmitted);
    setTimes(&pkg,(uint64_t) time_received,(uint64_t) time_transmitted);
    sendMsg(sockfd,&pkg);
    printf("Times sent. \n");
}

void child_protocol(int sockfd, const struct sockaddr *father_addr){
    Msg pkg;
    fd_set rfds;
    struct timeval tv;
    time_t request_time, response_time;
    int flag_sync_received = 0, ret, err;
    struct sockaddr_in child_addr;
    unsigned int len;
    len = sizeof(child_addr);
    
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    while(!flag_sync_received){
        time(&request_time);
        printf("Sending SYNC_REQ. \n");
        setRequest(&pkg);
        sendMsg(sockfd,&pkg);

        FD_ZERO(&rfds);
        FD_SET(sockfd, &rfds);

        ret = select(sockfd+1, &rfds,NULL,NULL,&tv);
        if ( ret == -1 ){
            perror("select: child_protocol()");
            return;
        }    

        if(FD_ISSET(sockfd, &rfds)){
            err = recvMsg(sockfd,&pkg);
            if (err == -1){
                perror("ERROR recv msg");
                break;
            }else if(err == 0){
                perror("socket closed");
                break;
            }
            if(getType(&pkg.hdr) == TYPE_SYN_RESP){
                printf("Got SYNC_RESP. \n");
                time(&response_time);
                update_internal_clock(request_time,response_time,getTime_received(&pkg),getTime_transmitted(&pkg));
                flag_sync_received = 1;
            } 
        }else{
            printf("No response from father \n");
        }
    }

}

void update_internal_clock(time_t request_time,time_t response_time,uint64_t time_received,uint64_t time_transmitted){

    printf("Times: \n");
    printf(" %ld \n",request_time);
    printf(" %ld \n",response_time);
    printf(" from msg: %llu \n",time_received);
    printf(" from msg: %llu \n",time_transmitted);


}