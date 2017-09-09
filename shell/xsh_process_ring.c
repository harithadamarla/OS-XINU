#include <xinu.h>
#include <string.h>
#include <stdio.h>
#include <process_ring.h>
#include <clock.h>

void poll(int process_count,int rounds_count,volatile int *globalarray)
{
         volatile int32 rounds_local = 0;

	 

	 printf("Number of Processes:%d\n",process_count);
         printf("Number of Rounds:%d\n",rounds_count);
	 int y;
 
	for(y=0;y<process_count;y++)
	{
		resume(create(poll_function,1024,20,"pollfunction",5,process_count,rounds_count,&globalarray[0],&rounds_local,y));
	}


}
void sem(int process_count,int rounds_count,volatile int *globalarray)
{

	int a,k,l,n;
        sid32 semarray[process_count];
	sid32 done_sem[process_count];

	int value=process_count*rounds_count-1;
	printf("Number of Processes:%d\n",process_count);
        printf("Number of Rounds:%d\n",rounds_count);

        for(l=0;l<process_count;l++)
        {
                if(l==0){
                        semarray[l]=semcreate(1);
                        globalarray[0]=value;
                }
                else{
                        semarray[l]=semcreate(0);
                }
        }
	for(a=0;a<process_count;a++)
	{
		done_sem[a]=semcreate(0);
	}

        for(k=0;k<process_count;k++)
        {
                resume(create(sem_function,1024,20,"semfunction",6,process_count,rounds_count,&globalarray[0],&semarray[0],&done_sem[0],k));

        }
      for(n=0;n<process_count;n++)
        {
		wait(done_sem[n]);
                semdelete(semarray[n]);
		semdelete(done_sem[n]);
	}
}

void print_usage(void)
{

printf("Usage:....\n");
printf("<-p>< --p>: no. of process <0-64>\n");
printf("<-i>< --i>: <poll or sync>\n");
printf("--help");

}
shellcmd xsh_process_ring(int32 argc,char *args[])
{
//	struct time t;
	int  process_count=2;
	int  rounds_count=3;

	int i,j,r,flag=0;

	volatile int globalarray[process_count];
	
	for(j=0;j<process_count;j++)
	globalarray[j]=-1;

	for(r=0;r<process_count;r++)
	{
		globalarray[r]=-1;
	}

	if((argc==1) &&strcmp(args[0],"process_ring")==0)
        poll(process_count,rounds_count,&globalarray[0]);

	//parsing command line arguments

	ulong start_time = clkcount();
	for( i=1;i<argc;i++)
	{
		if(strcmp("--help",args[i])==0)
		{
			
			print_usage();
			return SHELL_OK;
		
		}
	
		if(0 == strcmp("-p", args[i]))
		{
			if(args[i+1]==0) 
			{
				print_usage();
				printf("-p flag expects an argument\n");
				return SHELL_ERROR;
			}

			process_count=atoi(args[i+1]);

			if(process_count==0)
			{
				print_usage();
				printf("-p expects an integer\n");
				return SHELL_ERROR;
			}
			if(!(0<=process_count && process_count<=64))
			{
				printf("expected value between 0 and 64");
				print_usage();
				return SHELL_ERROR;
			}
		}
		if(strcmp(args[i],"-r")==0)
		{
			rounds_count=atoi(args[i+1]);
		
		}

                if(strcmp("-i",args[i])==0)
		{
			if(strcmp("sync",args[i+1])==0)
			{
				flag=1;
				//calling the semaphore function
				sem(process_count,rounds_count,&globalarray[0]);
				
			}
			else if(strcmp("poll",args[i+1])==0)
			{
					flag=1;
					//calling the polling function
					poll(process_count,rounds_count,&globalarray[0]);
			}
			else{
				flag=1;
				print_usage();
				printf("-i expects an argument");
			}
		}
	}
	if(flag==0)
	{
		poll(process_count,rounds_count,&globalarray[0]);
	}
	ulong end_time=clkcount();
	int timedifference=((int)(end_time-start_time))/CLKTICKS_PER_SEC;
	printf("Amount of time elapsed during count down= %d\n",timedifference);
	return SHELL_OK;
}
	
	
	

