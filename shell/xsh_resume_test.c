#include<xinu.h>
#include<stdio.h>
#include<stdlib.h>


void test(int x)
{
struct procent *ptr;
ptr=&proctab[getpid()];
ptr->prprio=40;
}

shellcmd xsh_resume_test(int nargs, char* args[])
{
	pid32 a;
	int n;

	a=create(test,1024,30,"print0",1,4);
	

	n=resume(a);


	printf("priority of the process %d\n",n);

	sleep(1);
	return 0;

}
