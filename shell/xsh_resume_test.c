#include<xinu.h>
#include<stdio.h>
#include<stdlib.h>

/*void print1(int x,struct procent *ptr)
{
//	int b;

//	b=create(print2,1024,20,"print2",2,6,ptr);

	ptr->prprio=30;
	
	//printf("print2 priority %d\n",resume(b));

	//exit(0);

	return OK;
}
void print0(int x)
{
	int b;
	struct procent *ptr;
 	printf("In function print0 %d\n",x);
	ptr=&proctab[getpid()];
	//sleep(1000);
	b=create(print1,1024,20,"print1",2,5,ptr);
	printf("b process priority:%d\n",resume(b));	
	printf("check if priority has changed:%d\n",ptr->prprio);
//	exit(0);
	return OK;

}*/
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
