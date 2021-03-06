#ifndef _FUTURE_H_
#define _FUTURE_H_  
//#define NFUT 20
#include <kernel.h>

typedef enum {
  FUTURE_EMPTY,
  FUTURE_WAITING,
  FUTURE_READY
} future_state_t;

typedef enum {
  FUTURE_EXCLUSIVE,
  FUTURE_SHARED,
  FUTURE_QUEUE
} future_mode_t;

typedef struct {
  int value;
  future_state_t state;
  future_mode_t mode;
  pid32 pid;
 // queue set_queue;
 // queue get_queue;
 qid16 set_queue;
qid16 get_queue;
} future_t;

/* Interface for the Futures system calls */
future_t* future_alloc(future_mode_t mode);
syscall future_free(future_t*);
syscall future_get(future_t*, int*);
syscall future_set(future_t*, int);
 
#endif /* _FUTURE_H_ */
