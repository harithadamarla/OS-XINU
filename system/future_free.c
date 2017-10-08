#include<future.h>
syscall future_free(future_t *f)
{
return freemem(f,sizeof(future_t));

}
