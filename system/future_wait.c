#include<future.h>
#include<process.h>
#include<queue.h>
syscall future_wait(future_t* f)
{
        intmask mask;
        mask=disable();
	struct procent* ptr;
//	ptr=&proctab[getpid()];
// 
       ptr=&proctab[currpid];
	if(f->state==FUTURE_EMPTY||f->state==FUTURE_WAITING)
        {
		switch(f->mode)
		{
			case FUTURE_EXCLUSIVE:
				ptr->prstate=PR_WAIT;
				if(f->pid==-1)
				{
					f->pid=currpid;
				}
				resched();
				break;
			case FUTURE_SHARED:
				ptr->prstate=PR_WAIT;
				resched();
				break;
			case FUTURE_QUEUE:
				ptr->prstate=PR_WAIT;
				resched();
				break;
			default:
				return SYSERR;
				break;
		}

        }
	
        restore(mask);
	return OK;
}

