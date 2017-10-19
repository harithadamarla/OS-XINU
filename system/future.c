#include<kernel.h>
#include<future.h>
#include<stdio.h>
#include<xinu.h>
syscall future_set(future_t *f, int value)
{
	intmask mask;
	mask=disable();

	if(f->state==FUTURE_WAITING||f->state==FUTURE_EMPTY)
	{
		switch(f->mode)
		{
			case FUTURE_EXCLUSIVE:
				//if the future is in exclusive, we directly set the value of the future, change the state 
				f->value=value;
                        	f->state=FUTURE_READY;
                                future_signal(f);
               			break;
			case FUTURE_SHARED:
				//if the future is in shared mode, we directly set the value, and empty the the getqueue, which are waiting on this future
				f->value = value;
            			f->state = FUTURE_READY;
            			resched_cntl(DEFER_START);
            			while (!isempty(f->get_queue)) {
                			ready(getfirst(f->get_queue));
            			}
            			resched_cntl(DEFER_STOP);
            			restore(mask);
            			return OK;


			case FUTURE_QUEUE:
				//if the future is in the queue mode, we set the value and dequeue only the first process from the getqueue
				if(isempty(f->get_queue))
	                        {
                	                enqueue(getpid(),f->set_queue);
        	                        future_wait(f);
				}
         		                f->value=value;
                        		f->state=FUTURE_READY;
                        		future_signal(f);
			
				break;
			default:
				return SYSERR;
				break;
			
		}

	 }
		
		
	else if(f->state==FUTURE_READY)
	{
		switch(f->mode)
		{
			case FUTURE_EXCLUSIVE:
				value = f->value;
            			f->state = FUTURE_EMPTY;
            			restore(mask);
            			return OK;
			case FUTURE_SHARED:
			case FUTURE_QUEUE:
			default:
			     return SYSERR;
			     break;
		}

	}
	else
	{
		return SYSERR;
	}	

return OK;

}
syscall future_get(future_t* f,int* value)
{
        intmask mask;
        mask=disable();
	struct procent *prptr;

        if(f->state==FUTURE_EMPTY)
        {
				switch(f->mode)
				{
					//we directly set the value of the future to FUTURE_WAITING, push the future to wait,i.e change the process state to thread wait					and call resched.
						case FUTURE_EXCLUSIVE:
							 f->state=FUTURE_WAITING;
							 future_wait(f);
							 *value=f->value;
							 f->state=FUTURE_EMPTY;
							 break;
					//we directly set the value of the future to FUTURE_WAITING,change the process state to wait,and enqueue the processes to getque					ue of the future.
						case FUTURE_SHARED:
							  
							f->state=FUTURE_WAITING;
	   						prptr = &proctab[getpid()];
           						prptr->prstate = PR_WAIT;    /* Set state to waiting	*/
           						enqueue(getpid(), f->get_queue);
           						resched();
           						*value = f->value;
          						restore(mask);
            						return OK;
					//if no processes are waiting in set_queue, then we enqueue the current process in getqueue. else we enqueue it in the setqueue 					of the future
					
						case FUTURE_QUEUE:
							 if(isempty(f->set_queue))
							 {
                                				enqueue(getpid(),f->get_queue);
                                				future_wait(f);
                                				future_get(f,value);
							 }
							else{
                                				enqueue(getpid(),f->get_queue);
                                				future_signal(f);
                                				future_wait(f);
                                				future_get(f,value);
							}
							break;
						default:
							return SYSERR;
							break;

								
				}
		}
		else if(f->state==FUTURE_READY)
		{
				switch(f->mode)
				{
				
					//wen the future is in ready state, we directly get the value from the future, and change the state to FUTURE_EMPTY
					case FUTURE_EXCLUSIVE:
					case FUTURE_SHARED:
					case FUTURE_QUEUE:
							*value = f->value;
							f->state = FUTURE_EMPTY;
							break;

					default:
						   return SYSERR;
						   break;
					
					
				}
		}
		else if(f->state==FUTURE_WAITING)
		{
			switch(f->mode)
			{
				//wen the future is in waiting state, we change the state of the process, whcih is trying to get the value of the future to PR_WAIT and 				enqueue it in get queue of the future.
				case FUTURE_EXCLUSIVE:
						return SYSERR;
						break;
				case FUTURE_SHARED:
	
 						prptr = &proctab[currpid];
            					prptr->prstate = PR_WAIT;    /* Set state to waiting	*/
            					enqueue(getpid(), f->get_queue);
            					resched();
            					*value = f->value;
            					restore(mask);
           					return OK;

				case FUTURE_QUEUE:
						break;
				default:	
					return SYSERR;
					break;
			}
		}
		else
		{
				return SYSERR;
		}
           return OK;
		   
}

