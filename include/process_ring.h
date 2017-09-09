int processes_count,rounds_count,value;
//int rounds_local=0;
void poll_function(int process_count, int rounds_count,volatile int *globalarray,volatile int* rounds_local,int index);
void sem_function(int process_count,int rounds_count,int* globalarray,int *semarray,sid32 *done_sem,int index);
