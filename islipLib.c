//Assigning incremental load
void assign_load(double * load,double incr_load, int numPort)
{
	int i;
	for (i = 0; i < numPort; i++)
	{
		*(load+i) = incr_load;
		//printf("load on Port%d: %g\n",i,*(load+i));
	}
}

void flushQueues(My402List *Q,int numPort)
{	
	int i,j;
	for (i = 0; i < numPort; i++)
	{
		for (j = 0; j < numPort; j++)
		{
			while (!My402ListEmpty(Q+((i*numPort) + j)))
			{
				My402ListElem * f = My402ListFirst(Q+((i*numPort) + j));
				packet * srvdPckt = f->obj;
				My402ListUnlink(Q +(i * numPort) + j, f);
				free(srvdPckt);
			}
		}
	}
}
//Initialise all queues
void InitQueues(My402List *Q,int numPort)
{	
	int i,j;
	for (i = 0; i < numPort; i++)
	{
		for (j = 0; j < numPort; j++)
		{
			if (!My402ListInit(  Q + ((i * numPort) + j ) ) )
			{
				fprintf(stderr,"Error initialisind queue for input port %d and dest port %d\n",i,j);
				exit(1);
			}
			else
			{
				sprintf(dbgMsg,"Initilised Queue: Input Port:%d Destination Port:%d\n",i,j);
				dbg(dbgMsg);
			}
		}
	}
}
//Initialize all priority pointers
void init_priority_pointers(int *ai,int *gi,int numPort)
{
	int i;	
	for (i = 0; i < numPort; i++)
	{
		*(ai+i) = rand() % numPort;
		*(gi+i) = rand() % numPort;
	}
}
//It is Bernouli function with probability of returning 1 (packet arriving) as load 
int packetArrives(double load)
{
	double t1 = rand() % 100;
	double t2 = 100*load;
	if ( 0 == load)
		return(-1);
	if ( t1 <= t2 )	
	{
		return(1);
	}
	else
	{
		return(-1);
	}
}
//Initialise the array probslab, so that it is easy to find destination port in the function destPort()
void initProbSlab(double *probSlab,double *p,int numPort)
{
	int c1,c2;
	int sum = 0;
	for (c1 = 0; c1 < numPort; c1++)
	{
		sum = 0;
		for (c2 = 0; c2 < numPort; c2 ++)
		{
			sum = sum + ( *(p+(c1*numPort)+c2)*100);
			*(probSlab+(c1*numPort)+c2) = sum ;	
		}
	}
}
//Used for debug purpose
void printProbSlab(double *probSlab, int numPort)
{
	int c1, c2;
	for (c1 = 0; c1 < numPort; c1 ++)
	{
		printf("ProbSlab for port: %d  ",c1);
		for (c2 = 0; c2 < numPort; c2 ++)
		{
			printf("%g ",*(probSlab+(c1*numPort)+c2));
		}
		printf("\n");
	}
}
//It wil return the destination port equaly likely
//**Works only for 2 port. It will return either 0 or 1 with equal probabilities
int destPort(int port,double * probSlab,int numPort)
{
	//printf("probability:%g\n",p0);
	//if (rand() % 10 < p0*10)
	double rnd;
	rnd = (rand() % 100);
	int count = 0;
	while (count < numPort )
	{
		if (rnd <= *(probSlab+(port*numPort)+count))
		{
			return(count);
		}
		count++;
	}
	return(numPort-1);
}

//Will fill the 2D array of weights "w" with the queue lengths
void getWeight(My402List *Q,int *ql,int numPort)
{
	int i,j;
	for (i = 0; i < numPort; i++)
	{
		for (j =0; j < numPort; j++)
		{
			//*(w + (i*numPort) + j) = My402ListLength(&Q[i * numPort + j]);	
			*(ql+(i*numPort)+j) = My402ListLength(Q+(i * numPort) + j);
		}
	}
	//printf("Returning from get weight\n");
}
void assignEqualProb(double *p, int numPort)
{
	int i,j;
	for (i = 0; i < numPort; i++)
	{
		for ( j = 0; j < numPort; j ++)
		{
			*(p+(i*numPort)+j) = 1.0/numPort;
		}
	}
}


//Parse the config file and populate the load and probabilities of destinations
/*void parseConfig(double * load, double *p,int numPort)
{
	FILE *fp;
	char buf[1024];
	char *ptr,*bufPtr;
	int c1,c2;
	double probSum;
	if (NULL == (fp = fopen("config","r")))
	{
		fprintf(stderr,"Could not open config file\n");
		exit(1);
	}
	//Get simTime
	if (NULL != fgets(buf, 100,fp))
	{
		bufPtr = buf;
		while (*bufPtr != ':')
		{
			(bufPtr)++;
		}
		bufPtr++;
		ptr = bufPtr;
		while (*ptr != '\n')
			ptr++;
		*ptr = '\0';
		simTime = atof(bufPtr);
		sprintf(dbgMsg,"simTime:%d(int)\n",simTime);
		dbg(dbgMsg);
	}
	for (c1 = 0; c1 < numPort; c1++)
	{
	//Get load[c1]
		if (NULL != fgets(buf, 100,fp))
		{
			bufPtr = buf;
			while (*bufPtr != ':')
			{
				(bufPtr)++;
			}
			bufPtr++;
			ptr = bufPtr;
			while (*ptr != '\n')
				ptr++;
			*ptr = '\0';
			*(load+c1) = atof(bufPtr);
			sprintf(dbgMsg,"Load%d:%f (double)\n",c1,*(load+c1));
			dbg(dbgMsg);
		}
	
		//Get probability to destination port
		c2 = 0;
		if (NULL != fgets(buf, 100,fp))
		{
			bufPtr = buf;
			while (c2 < numPort)
			{
				while (	*bufPtr != ':')
				{
					(bufPtr)++;
				}
				bufPtr++;
				ptr = bufPtr;
				while(*ptr != ' ' && *ptr != '\n')
				{	
					ptr++;
				}
				if ( ('\n' == *ptr) && ( c2 < (numPort-1) ) )
				{
					fprintf(stderr, "Please specify all probabilities for Port:%d in config file\n",c1);
					exit(1);	
				}
				*ptr = '\0';
				*(p+(c1*numPort)+c2) = atof(bufPtr);
				sprintf(dbgMsg,"P%d%d:%g (double)\n",c1,c2,*(p+(c1*numPort)+c2));
				dbg(dbgMsg);
					
				ptr++;
				bufPtr = ptr;
				c2++;
			}
		}
		else
		{
			fprintf(stderr,"Config file is not in proper format\n");
			exit(1);
		}
	}

	//Sanity check. Sum of prob should be 1.
	for (c1 = 0; c1 < numPort; c1++)
	{
		probSum = 0;
		for ( c2 = 0;  c2 < numPort; c2++)
		{
			probSum = probSum + *(p+(c1*numPort)+c2);
		}
		if (1 != probSum )
		{
			fprintf(stderr,"The sum of probabilities does not add up to 1. it is %g\n",probSum);
			exit(1);
		}
	}
}
*/
//Print weight and weight normals in repective files
void printWeights(FILE *ql_fp,FILE * avg_ql_fp,int *ql,int ts,int numPort)
{
	int i,j;
	for (i =0; i < numPort; i++)
	{
		for ( j = 0; j < numPort; j++)
		{
			fprintf(ql_fp,"%d\t",*(ql+(i*numPort)+j));
			fprintf(avg_ql_fp,"%.4g\t",(double)*(ql+(i*numPort)+j)/ts);
#ifdef DEBUG
			fprintf(log_fp,"w[%d][%d]=%d\t",i,j,*(ql+(i*numPort)+j));
#endif
		}
#ifdef DEBUG
	fprintf(log_fp,"\n");
#endif
	}
	fprintf(ql_fp,"\n");
	fprintf(avg_ql_fp,"\n");
}

