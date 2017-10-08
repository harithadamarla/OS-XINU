#include<future.h>
future_t* future_alloc(future_mode_t mode)
{

future_t* f;
f=(future_t *)getmem(sizeof(future_t));

f->mode=mode;
f->state=FUTURE_EMPTY;
f->value=0;

//if 

f->set_queue=-1;
f->get_queue=-1;
f->pid=-1;
if(mode==FUTURE_SHARED||mode==FUTURE_QUEUE)
{
f->get_queue=newqueue();
}
if(mode==FUTURE_QUEUE)
{
f->set_queue=newqueue();
}
return f;
}
