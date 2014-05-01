void print_request(int * request, int numPort)
{
	int i,j;
	for (i = 0; i < numPort; i++)
	{
		for (j = 0; j < numPort; j++)
		{
			fprintf(log_fp,"request[%d][%d]: %d \t", i, j, *(request+(i*numPort) + j ) );
		}
		fprintf(log_fp,"\n");
	}
}
void print_grant(int * grant, int *gi, int numPort)
{
	int i,j;
	for (i = 0; i < numPort; i++)
	{
		for (j = 0; j < numPort; j++)
		{
			fprintf(log_fp,"grant[%d][%d]: %d \t",i,j,*(grant+ (i*numPort) +j) );
		}
		fprintf(log_fp,"\n");
	}
	for (j = 0; j < numPort; j++)
	{
		fprintf(log_fp,"gi[%d] = %d \t",j,*(gi+j));
	}
	fprintf(log_fp,"\n");
}
void print_accept(int * accept, int *ai, int numPort)
{
	int i,j;
	for (i = 0; i < numPort; i++)
	{
		for (j = 0; j < numPort; j++)
		{
			fprintf(log_fp,"accept[%d][%d]: %d \t",i,j,*(accept+(i*numPort)+j) );
		}
		fprintf(log_fp,"\n");
	}
	for (i = 0; i < numPort; i++)
	{
		fprintf(log_fp,"ai[%d] = %d \t",i,ai[i]);
	}
	fprintf(log_fp,"\n");
}
void init(int * accept,int *request,int *grant, int numPort)
{
	int i,j;
	for (i = 0; i < numPort; i++)
	{
		for (j = 0; j < numPort; j++)
		{				//set these variables 1 when
			*(accept+(i*numPort)+j) = 0;	//input port i accepts the grant from output port j
			*(request+(i*numPort)+j) = 0;	//input port i requests for output port j
			*(grant+(i*numPort)+j) = 0; 	//input port i granted by output port j
		}
		
	}
}

//Each input should send a request to correcponding output
//where it has a packet destined to
void send_request(int *request,int *ql, int numPort)
{
	int i,j;
	for (i = 0; i < numPort; i++)
	{
		for (j = 0; j < numPort; j++)
		{
			if (*(ql+(i*numPort)+j) > 0 ) 
			//input i send s a request to output j
			//if it has a packet to send
			{
				*(request+(i*numPort)+j) = 1;
			}
		}
	}	
}

//Each output port will grant an input port which comes 
//next according to a fixed round robin with highest priority
//the pririty is pointed by g[j]
void do_grant(int *request,int *grant,int *gi,int numPort)
{
	int i,j;
	for (j = 0; j < numPort; j++)
	{
		i = *(gi+j);
		do 
		{
			if (*(request+(i*numPort)+j))
			{
				*(grant+(i*numPort)+j) = 1;
				break;
			}
			i = (i + 1) % numPort;
		}
		while ( i != *(gi+j) ); 
	}
}

// Each input port i will accept the first grant that it recieved 
// from output.
void do_accept(int *accept, int *grant, int *ai, int numPort)
{
	int i,j;
	int tmp_ai;
	for (i = 0; i < numPort; i++)
	{
		j = *(ai+i);
		tmp_ai = *(ai+i);
		do 
		{
			if (*(grant+(i*numPort)+j))
			{
				*(accept+(i*numPort)+j) = 1;
				*(ai+i) = (j + 1) % numPort;
				break;
			}
			j = (j + 1) % numPort;
		}
		while ( j != tmp_ai );
	}
}
void update_priorityPointers(int *grant,int *accept,int *gi, int numPort)
{
	int i,j;
	for (j = 0 ; j < numPort; j ++)
	{
		for (i = 0; i < numPort; i++ )
		{
			if ( *(grant+(i*numPort)+j)  && *(accept+(i*numPort)+j)) //Update gi only if its grant is accepted
			{
				*(gi+j) = (i + 1) % numPort;
			}
		}
	}
}
void print_pointers(int *gi,int numPort)
{
	int j;
	for (j =0; j < numPort; j++)
	{
		fprintf(log_fp,"gi[%d] = %d\t",j,*(gi+j));
	}
	fprintf(log_fp,"\n");
}
void iSLIP_schedule(int *accept,int *request,int *grant,int *ai,int *gi,int *ql,int numPort)
{
	init(accept,request,grant,numPort);
	send_request(request,ql,numPort);
#ifdef DEBUG
	print_request(request,numPort);
#endif
	do_grant(request,grant,gi,numPort);
#ifdef DEBUG
	print_grant(grant,gi,numPort);
#endif
	do_accept(accept,grant,ai,numPort);
	update_priorityPointers(grant,accept,gi,numPort);
#ifdef DEBUG
	print_accept(accept,ai,numPort);
	print_pointers(gi,numPort);
#endif
}	
