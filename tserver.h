#include "tIOT_PROTO.h"


//Creates socket in the child or client side
int socketCreate_cli_side(char* port);

//Function to simulate clock
void *threadClock_server(void *thr_arg);
