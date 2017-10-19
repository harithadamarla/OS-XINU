#include<future.h>
syscall future_free(future_t *f)
{
return freemem((char *)f,sizeof(future_t));

}
