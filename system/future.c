#include<kernel.h>
#include<future.h>
//#include<process.h>
//#include<queue.h>
#include<stdio.h>
#include<xinu.h>
syscall future_set(future_t *f, int value)
{

intmask mask;
mask=disable();
//int processid;


	if(f->state==FUTURE_WAITING||f->state==FUTURE_EMPTY)
	{
		switch(f->mode)
		{
			case FUTURE_EXCLUSIVE:
				f->value=value;
                        	f->state=FUTURE_READY;
                                future_signal(f);
               			break;
			case FUTURE_SHARED:


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
				if(isempty(f->get_queue))
	                        {
                	                enqueue(getpid(),f->set_queue);
        	                        future_wait(f);
					f->value=value;
	                	        f->state=FUTURE_READY;
        		                future_signal(f);

                        	}
				else{
         		                f->value=value;
                        		f->state=FUTURE_READY;
                        		future_signal(f);
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
			case FUTURE_EXCLUSIVE:
value = f->value;
            f->state = FUTURE_EMPTY;
            restore(mask);
            return OK;

	//		     return SYSERR;
	//		     break;
			case FUTURE_SHARED:
			     return SYSERR;
			     break;
			case FUTURE_QUEUE:
			     return SYSERR;
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
syscall future_get(future_t* f,int* value)
{
        intmask mask;
        mask=disable();
struct procent *prptr;

        if(f->state==FUTURE_EMPTY)
        {
				switch(f->mode)
				{
						case FUTURE_EXCLUSIVE:
							 f->state=FUTURE_WAITING;
							 future_wait(f);
							 *value=f->value;
							 f->state=FUTURE_EMPTY;
							 break;
						case FUTURE_SHARED:
							  
							f->state=FUTURE_WAITING;
	   prptr = &proctab[getpid()];
           prptr->prstate = PR_WAIT;    /* Set state to waiting	*/
           enqueue(getpid(), f->get_queue);
           resched();
           *value = f->value;
          restore(mask);
            return OK;
		//					enqueue(getpid(),f->get_queue);
		//					future_wait(f);
		//					*value=f->value;
		//					if(isempty(f->get_queue))
		//					{
                  //           				   f->state=FUTURE_EMPTY;
		//					}
		//					break;
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
				//	case FUTURE_QUEUE:
					
					case FUTURE_EXCLUSIVE:
							*value = f->value;
							f->state = FUTURE_EMPTY;
							break;

					case FUTURE_SHARED:
							*value = f->value;
							if(isempty(f->get_queue))
							f->state = FUTURE_EMPTY;
							break;

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
//
//						enqueue(getpid(),f->get_queue);
//						future_wait(f);
//						*value=f->value;
//						if(isempty(f->get_queue))
//						{
//							f->state=FUTURE_EMPTY;
//						}
//						break;
				case FUTURE_QUEUE:
						break;
				defalut:	
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
/*syscall future_get(future_t* f,int* value)
{
	intmask mask;
	mask=disable();
	if(f->state==FUTURE_EMPTY)
	{
		if(f->mode==FUTURE_EXCLUSIVE)
		{
			f->state=FUTURE_WAITING;
			future_wait(f);
			*value=f->value;
			f->state=FUTURE_EMPTY;
			
		}
		if(f->mode==FUTURE_SHARED)
		{
		//	printf("checking");
			f->state=FUTURE_WAITING;
			enqueue(getpid(),f->get_queue);
			future_wait(f);
			*value=f->value;
			if(isempty(f->get_queue))
			{
				f->state=FUTURE_EMPTY;
			}
		}
		if(f->mode==FUTURE_QUEUE)
		{
		
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
		
		}
		else
		{
			return SYSERR;
		}

	}
	else if(f->state==FUTURE_WAITING)
	{
		if(f->mode==FUTURE_EXCLUSIVE)
		return SYSERR;
		else if(f->mode==FUTURE_SHARED)
		{
			//printf("checking in waiting state");
			enqueue(getpid(),f->get_queue);
			future_wait(f);
			*value=f->value;
			if(isempty(f->get_queue))
			f->state=FUTURE_EMPTY;
		}
		else if(f->mode==FUTURE_QUEUE)
		{
			//do nothing;	
		}
		else{
			return SYSERR;
		}
		
	}	
	else if(f->state==FUTURE_READY)
	{
		printf("entered");

		if(f->mode==FUTURE_EXCLUSIVE)
		{
			 *value=f->value;
	                f->state=FUTURE_EMPTY;

		}
		else if(f->mode==FUTURE_SHARED)
		{
				*value=f->value;
				if(isempty(f->get_queue))
					f->state=FUTURE_EMPTY;
		}
		else if(f->mode==FUTURE_QUEUE)
		{
			*value=f->value;
			f->state=FUTURE_EMPTY;
		}
		else
		{
			return SYSERR;
		}

			
	}
	else
	{
		return SYSERR;
	}
	restore(mask);
	return OK;
}*/
