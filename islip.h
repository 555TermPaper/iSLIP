#include <stdio.h>
#define NUMPORT 10 
typedef struct packet
{
	int pkt_num;
	int dest;	//Destination port
	int timeEnter;	//time slot at which it entered the input queue 
	int timeLeft;	//time slot at which it left the queue
}packet;
char dbgMsg[512];
FILE * log_fp = NULL;
FILE * ql_fp = NULL;
FILE * avg_ql_fp = NULL;
FILE * param_fp = NULL;
