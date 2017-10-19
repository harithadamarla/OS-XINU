#include<future.h>
#include<process.h>
#include<queue.h>
syscall future_signal(future_t* f)
{
	intmask mask;
	mask=disable();
	if(f->state==FUTURE_READY)
	{
		switch(f->mode)
		{
			case FUTURE_EXCLUSIVE:
				ready(f->pid);
				break;
			case FUTURE_SHARED:
				while(!isempty(f->get_queue))
				{

					ready(dequeue(f->get_queue));
				}
				break;
			case FUTURE_QUEUE:
				ready(dequeue(f->get_queue));
				break;
			default:
				return SYSERR;
				break;
		}
	}
	else if(f->state==FUTURE_EMPTY)
	{
		switch(f->mode)
		{
			case FUTURE_EXCLUSIVE:
				break;
			case FUTURE_SHARED:
				break;
			case FUTURE_QUEUE:
				ready(dequeue(f->set_queue));
				break;
			default:
				return SYSERR;
				break;
		}
	}

	restore(mask);
	return OK;
}
