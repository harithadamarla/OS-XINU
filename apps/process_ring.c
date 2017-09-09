#include <xinu.h>
#include <process_ring.h>
#include <stdio.h>
void sem_function(int processes,int rounds,int *globalarray,int *semarray,sid32 *done_sem,int index)
{
        int a,value;
        int rounds_local=0;

	//globalarray is used to maintain all the updated values
	//semarray is used to maintain semaphore ids
	//index is to track the process id
	//rounds_local is used to maintain the track for no.of rounds of each process takes
	
        while(rounds_local<rounds && value>=0)
        {
                wait(semarray[index]);
                value = globalarray[index];
                value--;

                printf("RingElement %d : Round %d : Value : %d\n",index,rounds_local,globalarray[index]);
	
		//updating the index value
                if(index==processes-1)
                {
                        index =0;
                        rounds_local=rounds_local+1;
                }
                else{
                        index =index +1;
                }

                globalarray[index]=value;
                signal(semarray[index]);
        }
        signal(done_sem[index]);
}
void poll_function( int process_count, int rounds_count,volatile int *globalarray,volatile int* rounds_local, int index)
{

	int z,h;

       	volatile int value=(process_count*rounds_count)-1;
	volatile int run_count = 0;

	globalarray[0]=value;	
	while(*rounds_local<rounds_count && value >= 0 )
	{				
		if(globalarray[index] != -1) 
		{
			int temp = globalarray[index];
			temp = temp -1;
			printf("RingElement %d:Round %d: Value:%d\n",index,*rounds_local,globalarray[index]);
			if(value==-1)
			{
				return SHELL_OK;
			}
 			
			//updating the next index value and rounds_local(no. of rounds each process takes)	
			if(index==process_count-1)
			{
				index=0;
				*rounds_local=*rounds_local+1;
			}
			else{
				index=index+1;
			}

			globalarray[index]=temp;
			value = temp;
		}

	}	
	return OK;
}
