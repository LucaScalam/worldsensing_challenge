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

// #define MAX_SECONDS 5       //time(in seconds) to get a drift of 1ms 
// #define SIMULATION_TIME 5       //time of simulation in minutes

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sock_father = 0,sock_child;
    unsigned count = 0;
    // unsigned simulated_time = 0;
    fd_set rfds;
    struct timeval time_out_sync;
    int ret, err;
    Msg pkg;
    uint64_t time_received,time_transmitted;
    struct sockaddr_in father_addr;
    ThreadArg_t argThreadClock;
    pthread_t thread_clock;
    struct timespec time_value;
    
    time_value.tv_sec = 0;
    time_value.tv_nsec = NANO_EQ;

    if (argc < 3) {
       fprintf(stderr,"usage %s port port \n", argv[0]);
       exit(0);
    }

    argThreadClock.flag = 0;
    argThreadClock.time_counter = 0;

    int condv = pthread_cond_init(&argThreadClock.cond_new_clock,NULL);
    if( condv ) {
        printf("ERROR; return code from pthread_cond_init() is %d\n", condv);
        exit(-1);
    }
    condv = pthread_cond_init(&argThreadClock.cond_wait_sync_req,NULL);
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

    sock_father = socketCreate_serv_side(argv[1],&father_addr);
    sock_child = socketCreate_cli_side(argv[2]);

    int rc = pthread_create(&thread_clock,NULL,threadClock_client,(void *)&argThreadClock);
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

            if(getType(&pkg.hdr) == TYPE_SYN_REQ){
                printf("Got SYNC_REQ. Sending times... \n");
                pthread_mutex_lock(&argThreadClock.counter_mtx);
                time_received = argThreadClock.time_counter;
                // argThreadClock.time_counter += TX_RX_TIME + PROC_TIME;
                pthread_mutex_unlock(&argThreadClock.counter_mtx);
                nanosleep(&time_value,NULL);
                // pthread_mutex_lock(&argThreadClock.counter_mtx);
                // time_transmitted = argThreadClock.time_counter;
                // // argThreadClock.time_counter += TX_RX_TIME + PROC_TIME;
                // pthread_mutex_unlock(&argThreadClock.counter_mtx);
                time_transmitted = time_received + NANO_SECONDS * SIMU_TIME;
                sendTimes(sock_child,time_received,time_transmitted);
            }   
        }else{
            count++;
            // printf("No Msg from client. Count: %d \n",count);
            // if(count == SIMU_TIME){
            //     count = 0;
            // }
        }

        if(argThreadClock.flag == 1){
            child_protocol(sock_father,&argThreadClock);
            argThreadClock.flag = 0;
            pthread_cond_signal(&argThreadClock.cond_new_clock);
        }


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

void child_protocol(int sockfd,ThreadArg_t *thr_arg){
    Msg pkg;
    uint64_t request_time, response_time;
    int err;
    struct timespec time_value;

    time_value.tv_sec = 0;
    time_value.tv_nsec = NANO_EQ;
    
    request_time = thr_arg->time_counter;
    printf("Sending SYNC_REQ. \n");
    for(int i = 0;i < 5;i++){
        nanosleep(&time_value,NULL);
    }
    setRequest(&pkg);
    sendMsg(sockfd,&pkg);

    err = recvMsg(sockfd,&pkg);
    if (err == -1){
        perror("ERROR recv msg");
        return;
    }else if(err == 0){
        perror("socket closed");
        return;
    }

    if(getType(&pkg.hdr) == TYPE_SYN_RESP){
        printf("Got SYNC_RESP. \n");
        for(int i = 0;i < 5;i++){
            nanosleep(&time_value,NULL);
        }
        response_time = thr_arg->time_counter + (int) (11 * SIMU_TIME * NANO_SECONDS) ;
        update_internal_clock(request_time,response_time,getTime_received(&pkg),getTime_transmitted(&pkg),thr_arg);
    }
}

void update_internal_clock(uint64_t request_time,uint64_t response_time,uint64_t time_received,uint64_t time_transmitted,ThreadArg_t *thr_arg){
    float theta, delt_1, delt_2;

    printf("Times: \n");
    printf(" %llu \n",request_time);
    printf(" %llu \n",response_time);
    printf(" from msg: %llu \n",time_received);
    printf(" from msg: %llu \n",time_transmitted);

    delt_1 = time_received - (float)request_time;
    delt_2 = time_transmitted - (float)response_time;
    theta = (delt_1 + delt_2)/2;
    // printf("d1: %f \n",delt_1);
    // printf("d2: %f \n",delt_2);
    // printf("theta: %f \n",theta);
    // delta = response_time - request_time - (time_transmitted - time_received);

    thr_arg->time_counter = response_time + theta ;
    // printf("theta: %u \n",thr_arg->time_counter);
}

void *threadClock_client(void *thr_arg){
    ThreadArg_t *thread_arg = (ThreadArg_t*)thr_arg;
    struct timespec time_value;
    unsigned global_count = 0;
    time_value.tv_sec = 0;
    time_value.tv_nsec = NANO_EQ;
    unsigned local_flag = 0;
    // time_value.tv_sec = SEC_EQ;
    // unsigned internal_count = 0;
    struct timeval real_time;

    FILE *fp;
    fp=fopen("client_1.csv","w+");
    fprintf(fp,"counter,seconds,micro seconds,local clock [ns]\n");
    
    while(global_count < NUM_ITER_SIM+1){
        
        if(local_flag){
            for(int i = 0; i < (int) SECONDS_FOR_DRIFT/SIMU_TIME; i++){

                nanosleep(&time_value,NULL);
                pthread_cond_signal(&thread_arg->cond_wait_sync_req);
                pthread_mutex_lock(&thread_arg->counter_mtx);
                thread_arg->time_counter += SIMU_TIME * NANO_SECONDS + 167;
                // pthread_mutex_unlock(&thread_arg->counter_mtx);
                // if(i % 12 == 11){
                    // pthread_mutex_lock(&thread_arg->counter_mtx);
                    // thread_arg->time_counter += 4;
                    pthread_mutex_unlock(&thread_arg->counter_mtx);
                // }
                printf("clock time: %llu ns \n",thread_arg->time_counter);
                gettimeofday(&real_time, 0);
                fprintf(fp,"%d,%lu,%lu,%llu\n",i,(long)real_time.tv_sec, (long)real_time.tv_usec, thread_arg->time_counter);
            }
        }
        local_flag = 1;
        thread_arg->flag = 1;
        pthread_cond_signal(&thread_arg->cond_wait_sync_req);   

        pthread_mutex_lock(&thread_arg->new_clock_mtx);
        pthread_cond_wait(&thread_arg->cond_new_clock,&thread_arg->new_clock_mtx);
        pthread_mutex_unlock(&thread_arg->new_clock_mtx);
        global_count++;
    }  

    printf("Ending clock... \n");
    fclose(fp);
    pthread_exit(NULL);
}