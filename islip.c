#ifdef DEBUG
#	define dbg(x) printf("%s",x);
#else
#	define dbg(x) {} //NUMPORTO DEBUG
#endif
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <error.h>
#include <string.h>
#include "islip.h"
#include "my402list.h"
#include "islipLib.c"
#include "iSLIP_scheduler.c"
#include "RRM_scheduler.c"
int main (int argc, char * argv[])
{
	int numPort = 4;
	int timeSlot;  		//counter to keep track of simulation time
	int simTime = 100000;
	double packet_c = 0; 		//counter for total number of packets generated
	double finPacket_c = 0; 	//counter for total number of packets finished service
	float  total_delay = 0;
	float avg_delay = 0;
	double percent_throughput; 
	int i,j;		
	int dest,pcktArrived;	//destination port of packet arrived
	int ld;			//load on a particular interger number
	double ldM;		//load on a particular port (remaining fraction)
	int tmpC;
	double incremental_load = 0;
	double load_step = 0.01;
	FILE* log_fp, *ql_fp, *avg_ql_fp, *param_fp;	
	packet *pkt;
	My402ListElem *f; 	//Used for dequeue
	packet *srvdPckt; 	//Used for dequeue
	if ( (strcmp(argv[1],"iSLIP")) && (strcmp(argv[1],"rrm") ) )
	{
		fprintf(stderr,"Usage:\n./islip rrm \nOR\n./islip iSLIP\n");
		exit(1);
	}
	My402List *Q = (My402List *)malloc(sizeof(My402List)*numPort*numPort);
	if (Q == NULL)
	{
		printf("malloc for queue failed. exiting\n");
		exit(1);
	}
	double *load = (double *)malloc(sizeof(double)*numPort*numPort); 
	if (load == NULL)
	{
		printf("malloc for queue failed. exiting\n");
		exit(1);
	}
	int *ql = (int *)malloc(sizeof(int)*numPort*numPort); 
	if (ql == NULL)
	{
		printf("malloc for queue failed. exiting\n");
		exit(1);
	}
	int *accept = (int *)malloc(sizeof(int)*numPort*numPort); 
	if (accept == NULL)
	{
		printf("malloc for queue failed. exiting\n");
		exit(1);
	}
	int *request = (int *)malloc(sizeof(int)*numPort*numPort); 
	if (request == NULL)
	{
		printf("malloc for queue failed. exiting\n");
		exit(1);
	}
	int *grant = (int *)malloc(sizeof(int)*numPort*numPort); 
	if (grant == NULL)
	{
		printf("malloc for queue failed. exiting\n");
		exit(1);
	}
	int *ai = (int *)malloc(sizeof(int)*numPort); 
	if (ai == NULL)
	{
		printf("malloc for queue failed. exiting\n");
		exit(1);
	}
	int *gi = (int *)malloc(sizeof(int)*numPort); 
	if (gi == NULL)
	{
		printf("malloc for queue failed. exiting\n");
		exit(1);
	}
	double *p = (double *)malloc(sizeof(double)*numPort*numPort); 
	if (p == NULL)
	{
		printf("malloc for queue failed. exiting\n");
		exit(1);
	}
	double *probSlab = (double *)malloc(sizeof(double)*numPort*numPort); 
	if (probSlab == NULL)
	{
		printf("malloc for queue failed. exiting\n");
		exit(1);
	}
	//My402List Q[NUMPORT * NUMPORT];
	InitQueues(Q,numPort);		//Initialise Virtual output queues
	//parseConfig(load,p,numPort);
	assignEqualProb(p,numPort);
	initProbSlab(probSlab,p,numPort);
	srand (time(NULL));
	packet_c = 0; 
	finPacket_c = 0; 
	init_priority_pointers(ai,gi,numPort);
	printProbSlab(probSlab,numPort);
	if ( (ql_fp = fopen("queueLen.dat","w+")) == NULL)
	{
		fprintf(stderr,"Cannot open file for writing\n");
		perror("fopen::");
		exit(1);
	}	
	if ( (avg_ql_fp = fopen("avg_queueLen.dat","w+")) == NULL)
	{
		fprintf(stderr,"Cannot open file for writing\n");
		perror("fopen::");
		exit(1);
	}
	if ( (log_fp = fopen("Simulation.log","w+")) == NULL)
	{
		fprintf(stderr,"Cannot open file for writing\n");
		perror("fopen::");
		exit(1);
	}
	if ( (param_fp = fopen("param.dat","w+")) == NULL)
	{
		fprintf(stderr,"Cannot open file for writting\n");
		perror("fopen::");
		exit(1);
	}
	fprintf(param_fp,"load\ttotalPackets\ttotalServicePAckets\tpercent_throughput\tavg_delay\ttotal_delay\n");
	for (incremental_load = 0.01; incremental_load < 0.99; incremental_load+=load_step)
	{
		assign_load(load,incremental_load,numPort);	
	for (timeSlot = 0; timeSlot < simTime; timeSlot++)
	{
	//Generate packet
	for (i = 0; i < numPort; i++)	//For all input ports
	{
		dest = -1;
		pcktArrived = -1;
		ld = (int)(*(load+i));
		ldM = *(load+i) - ld;
		tmpC = 0;
		while (tmpC < ld) //geenrate a packet if the load is beyond 1.
		{
			dest = destPort(i,probSlab,numPort);	//Get the destination port
			pkt = (packet *) malloc(sizeof(packet));
			if (NULL == pkt)
			{
				fprintf(stderr,"Error in malloc\n");
				perror("Malloc::");
				exit(1);
			}
			pkt->timeEnter = timeSlot;
			pkt->timeLeft = -1;
			pkt->dest = dest;
			pkt->pkt_num = packet_c;
			packet_c++;
			if (!(My402ListAppend(Q+(i * numPort) + dest,(void *)pkt)))
			{
				fprintf(stderr,"Error Appending packet%g to Queue%d%d\n",packet_c,i,dest);
				exit(1);
			}
			fprintf(log_fp,"TimeSlot:%d\tp%g\ti/p Port:%d\to/p Port:%d\n",timeSlot,packet_c,i,dest);
			tmpC++;
		}
		dest = -1;
		pcktArrived = -1;
		if (1 == (pcktArrived = packetArrives(ldM)) )	//Packet arrived
		{
			//printf("Input: %d\n",i);
			dest = destPort(i,probSlab,numPort);	//Get the destination port
			pkt = (packet *) malloc(sizeof(packet));
			if (NULL == pkt)
			{
				fprintf(stderr,"Error in malloc\n");
				perror("Malloc::");
				exit(1);
			}
			pkt->timeEnter = timeSlot;
			pkt->timeLeft = -1;
			pkt->dest = dest;
			pkt->pkt_num = packet_c;
			packet_c++;
			if (!(My402ListAppend(Q+(i * numPort) + dest,(void *)pkt)))
			{
				fprintf(stderr,"Error Appending packet%g to Queue%d%d\n",packet_c,i,dest);
				exit(1);
			}
			//printf("Packet Arrived Destined to %d\n",dest);
			fprintf(log_fp,"TimeSlot:%d\tp%g\ti/p Port:%d\to/p Port:%d\n",timeSlot,packet_c,i,dest);
		}
		
	}
	getWeight(Q,ql,numPort);
	//printWeights(ql_fp, avg_ql_fp, timeSlot);
	if (!strcmp(argv[1],"iSLIP"))
	{
		iSLIP_schedule(accept,request,grant,ai,gi,ql,numPort);	//Do the iSLIP scheduling 
	}
	else if (!strcmp(argv[1],"rrm"))
	{
		rrm_schedule(accept,request,grant,ai,gi,ql,log_fp,numPort);		//Do the RRM scheduling 
	}
	//Service a packet based on iSLIP shcduler' output. Use the toDequeue array.
	//This assumes that toDequeue array will have atleast one queue marked as 1 per input port
	for (i = 0; i < numPort; i++)
	{
		for ( j = 0; j < numPort; j++)
		{
			if ( (*(ql+(i*numPort)+j) > 0) && *(accept + (i*numPort) + j) ) //i.e. if Q[i][j] is not empty
			{
				f = My402ListFirst(Q + (i * numPort) + j);
				if (f != NULL)
				{
					srvdPckt = (packet *) f->obj;
					My402ListUnlink(Q +(i * numPort) + j, f);
					srvdPckt->timeLeft = timeSlot;
					finPacket_c++;
					total_delay = total_delay + (srvdPckt->timeLeft - srvdPckt->timeEnter + 1);
					fprintf(log_fp,"TimeSlot:%d\tp%d\tfrom port:%d\tto port:%d serviced delay:%d imtLeft:%d timeEnter:%d\n",timeSlot,srvdPckt->pkt_num,i,j,(srvdPckt->timeLeft - srvdPckt->timeEnter + 1),srvdPckt->timeLeft,srvdPckt->timeEnter);
					free(srvdPckt);
				}
				else
				{
					fprintf(stderr,"Logical flaw. Even if weight is greater than zero the Q[%d][%d] is empty\n",i,j);
					exit(1);
				}
			}
		}
	}
	getWeight(Q,ql,numPort);
	//printWeights(ql_fp,avg_ql_fp,timeSlot);
	fprintf(log_fp,"TimeSlot:%d ####Ends####\n",timeSlot);
	}
	fprintf(log_fp,"####Load:%g####\n",incremental_load);
	percent_throughput =  ((double) finPacket_c / (double)packet_c ) * 100;
	avg_delay = total_delay / finPacket_c;
	fprintf(param_fp,"%g\t%g\t\t%g\t\t\t%g\t\t\t%f\t%f\n", incremental_load, packet_c, finPacket_c, percent_throughput, avg_delay, total_delay);
	//Load loop ends. New load
	printf("Total delay:%f\n",total_delay);
	timeSlot = 0;
	total_delay = 0;
	packet_c = 0;
	finPacket_c = 0;
	percent_throughput = 0;
	avg_delay = 0;
	flushQueues(Q,numPort);
	}
	return(0);
}
