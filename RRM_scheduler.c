void rrm_print_request(int * request,FILE * log_fp, int numPort)
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
void rrm_print_grant(int * grant, int *gi,FILE * log_fp, int numPort)
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
void rrm_print_accept(int * accept, int *ai,FILE * log_fp, int numPort)
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
void rrm_init(int * accept,int *request,int *grant, int numPort)
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
void rrm_send_request(int *request,int *ql, int numPort)
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
void rrm_do_grant(int *request,int *grant,int *gi,int numPort)
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
				*(gi+j) = (*(gi+j) + 1) % numPort;
				break;
			}
			i = (i + 1) % numPort;
		}
		while ( i != *(gi+j) ); 
	}
}

// Each input port i will accept the first grant that it recieved 
// from output.
void rrm_do_accept(int *accept, int *grant, int *ai, int numPort)
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
void rrm_schedule(int *accept,int *request,int *grant,int *ai,int *gi,int *ql,FILE * log_fp,int numPort)
{
	init(accept,request,grant,numPort);
	rrm_send_request(request,ql,numPort);
#ifdef DEBUG
	rrm_print_request(request,log_fp,numPort);
#endif
	rrm_do_grant(request,grant,gi,numPort);
#ifdef DEBUG
	rrm_print_grant(grant,gi,log_fp,numPort);
#endif
	rrm_do_accept(accept,grant,ai,numPort);
#ifdef DEBUG
	rrm_print_accept(accept,ai,log_fp,numPort);
#endif
}	
