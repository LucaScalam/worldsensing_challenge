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
#include <sys/time.h>

#include "tserver.h"
#include "tIOT_PROTO.h"

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sock_child;
    fd_set rfds;
    struct timeval time_out_sync;
    int ret, err;
    Msg pkg;
    ThreadArg_t argThreadClock;
    pthread_t thread_clock;
    struct timespec time_value;

    time_value.tv_sec = 0;
    time_value.tv_nsec = NANO_EQ;
    
    if (argc < 2) {
       fprintf(stderr,"usage %s port \n", argv[0]);
       exit(0);
    }
    
    argThreadClock.flag = 0;
    argThreadClock.time_counter = 0;

    // int condv = pthread_cond_init(&argThreadClock.cond_new_clock,NULL);
    // if( condv ) {
    //     printf("ERROR; return code from pthread_cond_init() is %d\n", condv);
    //     exit(-1);
    // }
    int condv = pthread_cond_init(&argThreadClock.cond_wait_sync_req,NULL);
    if( condv ) {
        printf("ERROR; return code from pthread_cond_init() is %d\n", condv);
        exit(-1);
    }
    int mx = pthread_mutex_init(&argThreadClock.new_clock_mtx, NULL);
    if( mx ) {
        char buff[64];
        strerror_r(mx,buff, sizeof(buff));
        printf("Problem in pthread_mutex_init()1: %s \n", buff);
        exit(-1);
    }
    mx = pthread_mutex_init(&argThreadClock.wait_sync_req_mtx, NULL);
    if( mx ) {
        char buff[64];
        strerror_r(mx,buff, sizeof(buff));
        printf("Problem in pthread_mutex_init()1: %s \n", buff);
        exit(-1);
    }

    mx = pthread_mutex_init(&argThreadClock.counter_mtx, NULL);
    if( mx ) {
        char buff[64];
        strerror_r(mx,buff, sizeof(buff));
        printf("Problem in pthread_mutex_init()3: %s \n", buff);
        exit(-1);
    }
    
    sock_child = socketCreate_cli_side(argv[1]);
    argThreadClock.sock_child = sock_child;
    
    int rc = pthread_create(&thread_clock,NULL,threadClock_server,(void *)&argThreadClock);
    if( rc ) {
        perror("pthread_create()");
        exit(-1);
    }

    
    time_out_sync.tv_usec = TIME_OUT_SYNC;

    while(1){
        
        pthread_mutex_lock(&argThreadClock.wait_sync_req_mtx);
        pthread_cond_wait(&argThreadClock.cond_wait_sync_req,&argThreadClock.wait_sync_req_mtx);
        pthread_mutex_unlock(&argThreadClock.wait_sync_req_mtx);

        FD_ZERO(&rfds);
        FD_SET(sock_child, &rfds);

        ret = select(sock_child+1, &rfds,NULL,NULL,&time_out_sync);
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

            // pthread_mutex_lock(&argThreadClock.wait_sync_req_mtx);
            // pthread_cond_wait(&argThreadClock.cond_wait_sync_req,&argThreadClock.wait_sync_req_mtx);
            // pthread_mutex_unlock(&argThreadClock.wait_sync_req_mtx);

            if(getType(&pkg.hdr) == TYPE_SYN_REQ){
                printf("Got SYNC_REQ. \n");

                sendSync1(sock_child);
                
                pthread_mutex_lock(&argThreadClock.wait_sync_req_mtx);
                pthread_cond_wait(&argThreadClock.cond_wait_sync_req,&argThreadClock.wait_sync_req_mtx);
                pthread_mutex_unlock(&argThreadClock.wait_sync_req_mtx);

                sendSync2(sock_child,&argThreadClock);

                } 
        }

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


void *threadClock_server(void *thr_arg){
    ThreadArg_t *thread_arg = (ThreadArg_t*)thr_arg;
    struct timespec time_value;
    unsigned global_count = 0;
    time_value.tv_nsec = NANO_EQ;
    struct timeval real_time;
    uint32_t counter_aux;

    FILE *fp;
    fp=fopen("server.csv","w+");
    fprintf(fp,"counter,seconds,micro seconds,local clock [us]\n");

    for(int i = 0;i < 32;i++){
        nanosleep(&time_value,NULL);
    }

    Msg pkg_sync;
    setSynClock(&pkg_sync);
    printf("Type %d \n",getType(&pkg_sync.hdr));
    printf("sending for pkg\n");
    sendMsg(thread_arg->sock_child,&pkg_sync);


    while(global_count < (int) (SECONDS_FOR_DRIFT * NUM_ITER_SIM / SIMU_TIME) + NUM_ITER_SIM - 1){

        for(int i = 0; i < (int) SECONDS_FOR_DRIFT/SIMU_TIME; i++){
            
            nanosleep(&time_value,NULL);
            
            pthread_mutex_lock(&thread_arg->counter_mtx);
            thread_arg->time_counter += SIMU_TIME * MICRO_SECONDS;
            counter_aux = thread_arg->time_counter;
            pthread_mutex_unlock(&thread_arg->counter_mtx);
            
            // printf("Atomic time: %u us\n",counter_aux);
            gettimeofday(&real_time, 0);
            fprintf(fp,"%d,%lu,%lu,%u\n",i,(long)real_time.tv_sec, (long)real_time.tv_usec, counter_aux);
            
            pthread_cond_signal(&thread_arg->cond_wait_sync_req);
        }
        global_count++;
    }  

    printf("Ending clock... \n");
    fclose(fp);
    pthread_exit(NULL);
}