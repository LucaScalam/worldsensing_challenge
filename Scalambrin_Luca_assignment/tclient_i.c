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

#include "tclient_i.h"
#include "tIOT_PROTO.h"

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sock_father,sock_child;
    fd_set rfds;
    struct timeval time_out_sync;
    int ret, err;
    struct sockaddr_in father_addr;
    ThreadArg_t argThreadClock;
    pthread_t thread_clock;
    struct timespec time_value;
    Msg pkg_sync1_father,pkg_sync2_father;
    Msg pkg_sync1_child,pkg_sync2_child;
    uint32_t t1,t2,t0,aux;
    
    time_value.tv_sec = 0;
    time_value.tv_nsec = NANO_EQ;

    if (argc < 3) {
       fprintf(stderr,"usage %s port port \n", argv[0]);
       exit(0);
    }

    argThreadClock.flag = 0;
    argThreadClock.time_counter = 0;

    int condv = pthread_cond_init(&argThreadClock.cond_wait_sync_req,NULL);
    if( condv ) {
        printf("ERROR; return code from pthread_cond_init() is %d\n", condv);
        exit(-1);
    }

    int mx = pthread_mutex_init(&argThreadClock.wait_sync_req_mtx, NULL);
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

    mx = pthread_mutex_init(&argThreadClock.flag_mtx, NULL);
    if( mx ) {
        char buff[64];
        strerror_r(mx,buff, sizeof(buff));
        printf("Problem in pthread_mutex_init()3: %s \n", buff);
        exit(-1);
    }

    int rc = pthread_create(&thread_clock,NULL,threadClock_client,(void *)&argThreadClock);
    if( rc ) {
        perror("pthread_create()");
        exit(-1);
    }

    sock_father = socketCreate_serv_side(argv[1],&father_addr);
    sock_child = socketCreate_cli_side(argv[2]);

    argThreadClock.sock_child = sock_child;
    argThreadClock.sock_father = sock_father;

    time_out_sync.tv_usec = TIME_OUT_SYNC;

    while(1){

        pthread_mutex_lock(&argThreadClock.wait_sync_req_mtx);
        pthread_cond_wait(&argThreadClock.cond_wait_sync_req,&argThreadClock.wait_sync_req_mtx);
        pthread_mutex_unlock(&argThreadClock.wait_sync_req_mtx);

        FD_ZERO(&rfds);
        FD_SET(sock_father, &rfds);

        ret = select(sock_father+1, &rfds,NULL,NULL,&time_out_sync);
        if ( ret == -1 ){
            perror("while_main_client");
            return 0;
        }    

        if(FD_ISSET(sock_father, &rfds)){

            
            err = recvMsg(sock_father,&pkg_sync1_father);
            if (err == -1){
                perror("ERROR recv msg");
                return 0;
            }else if(err == 0){
                perror("socket closed");
                return 0;
            }

            if(getType(&pkg_sync1_father.hdr) == TYPE_SYN_1){

                printf("Got SYNC_1. \n");
                pthread_mutex_lock(&argThreadClock.counter_mtx);
                t1 = argThreadClock.time_counter;
                pthread_mutex_unlock(&argThreadClock.counter_mtx);

                pthread_mutex_lock(&argThreadClock.wait_sync_req_mtx);
                pthread_cond_wait(&argThreadClock.cond_wait_sync_req,&argThreadClock.wait_sync_req_mtx);
                pthread_mutex_unlock(&argThreadClock.wait_sync_req_mtx);

                err = recvMsg(sock_father,&pkg_sync2_father);
                if (err == -1){
                    perror("ERROR recv msg");
                    return 0;
                }else if(err == 0){
                    perror("socket closed");
                    return 0;
                }
                if(getType(&pkg_sync2_father.hdr) == TYPE_SYN_2){
                    printf("Got SYNC_2. \n");
                    t0 = getTime_field(&pkg_sync2_father);
                    pthread_mutex_lock(&argThreadClock.counter_mtx);
                    t2 = argThreadClock.time_counter;
                    argThreadClock.time_counter = (unsigned) (t2 - t1 + t0 - SIMU_TIME * MICRO_SECONDS);
                    pthread_mutex_unlock(&argThreadClock.counter_mtx);
                    printf("t0: %u, t1: %u, t2: %u\n",t0, t1, t2);


                    pthread_mutex_lock(&argThreadClock.wait_sync_req_mtx);
                    pthread_cond_wait(&argThreadClock.cond_wait_sync_req,&argThreadClock.wait_sync_req_mtx);
                    pthread_mutex_unlock(&argThreadClock.wait_sync_req_mtx);

                    setSync1(&pkg_sync1_child);

                    sendMsg(sock_child,&pkg_sync1_child);

                    pthread_mutex_lock(&argThreadClock.wait_sync_req_mtx);
                    pthread_cond_wait(&argThreadClock.cond_wait_sync_req,&argThreadClock.wait_sync_req_mtx);
                    pthread_mutex_unlock(&argThreadClock.wait_sync_req_mtx);
                    
                    pthread_mutex_lock(&argThreadClock.counter_mtx);
                    aux = argThreadClock.time_counter;
                    pthread_mutex_unlock(&argThreadClock.counter_mtx);
                    printf("sync2: %u\n",aux);

                    setSync2(&pkg_sync2_child,aux);
                    sendMsg(sock_child,&pkg_sync2_child);
                }
            }
        }

    }

    mx = pthread_mutex_destroy(&argThreadClock.wait_sync_req_mtx);
    if( mx ) {
        char buff[64];
        strerror_r(mx,buff, sizeof(buff));
        printf("Problem in pthread_mutex_destroy()1: %s \n", buff);
    }

    mx = pthread_mutex_destroy(&argThreadClock.counter_mtx);
    if( mx ) {
        char buff[64];
        strerror_r(mx,buff, sizeof(buff));
        printf("Problem in pthread_mutex_destroy()2: %s \n", buff);
    } 

    mx = pthread_mutex_destroy(&argThreadClock.flag_mtx);
    if( mx ) {
        char buff[64];
        strerror_r(mx,buff, sizeof(buff));
        printf("Problem in pthread_mutex_destroy()3: %s \n", buff);
    } 

    condv = pthread_cond_destroy(&argThreadClock.cond_wait_sync_req);
    if( condv ) {
        printf("ERROR; return code from pthread_cond_destroy() is %d\n", condv);
        exit(-1);
    }

    close(sock_father);
    close(sock_child);
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
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr->sin_addr.s_addr,
         server->h_length);
    serv_addr->sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *) serv_addr,sizeof(*serv_addr)) < 0) 
        error("ERROR connecting");

    printf("Connected to father \n");

    return sockfd;
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
    int bind_res = bind(sockfd, (struct sockaddr *) &serv_addr,
            sizeof(serv_addr));
    if (bind_res < 0) 
            perror("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    connfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (connfd < 0) {
        printf("server acccept failed...\n");
        exit(0);
    }

    printf("Connected to child \n");

    return connfd;
}


void *threadClock_client(void *thr_arg){
    ThreadArg_t *thread_arg = (ThreadArg_t*)thr_arg;
    struct timespec time_value;
    unsigned global_count = 0;
    time_value.tv_sec = 0;
    time_value.tv_nsec = NANO_EQ;
    struct timeval real_time;
    uint32_t counter_aux;

    FILE *fp;
    fp=fopen("client_ii.csv","w+");
    fprintf(fp,"counter,seconds,micro seconds,local clock [us]\n");
    
    for(int i = 0;i < 8;i++){
        nanosleep(&time_value,NULL);
    }

    Msg pkg_sync;
    printf("waiting for pkg\n");

    recvMsg(thread_arg->sock_father,&pkg_sync);
    printf("Type %d \n",getType(&pkg_sync.hdr));
    sendMsg(thread_arg->sock_child,&pkg_sync);

    while(global_count < NUM_ITER_SIM + 1){
        
            for(int i = 0; i < (int) SECONDS_FOR_DRIFT/SIMU_TIME; i++){
            // for(int i = 0; i < AUX_VAL; i++){
                nanosleep(&time_value,NULL);
                pthread_mutex_lock(&thread_arg->counter_mtx);
                thread_arg->time_counter += SIMU_TIME * MICRO_SECONDS + 1;
                counter_aux = thread_arg->time_counter;
                pthread_mutex_unlock(&thread_arg->counter_mtx);
                
                // printf("clock time: %u us \n",counter_aux);

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