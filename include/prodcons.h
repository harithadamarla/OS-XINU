//<<<<<<< HEAD
#include<stdio.h>
#include<stddef.h>
uint future_prod(future_t *fut,int n);
uint future_cons(future_t *fut);
//=======
/*Global variable for producer consumer*/
extern int n; /*this is just declaration*/

/*function Prototype*/
void consumer(int);
void producer(int);

/* Semaphore Declarations */
extern sid32 produced, consumed;
  
//>>>>>>> 3219648c48f3910b3280764d393176ac1100c3d6
