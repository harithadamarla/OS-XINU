#include<xinu.h>
#include<stdio.h>
#include<stdlib.h>

void print1(int x,struct procent *ptr)
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

}

shellcmd xsh_resume_test(int nargs, char* args[])
{
	pid32 a;
// 	struct procent *ptr;

	//ptr=&proctab[getpid()];i
	//
	a=create(print0,1024,20,"print0",1,4);


	resume(a);

// 	a=create(print1,1024,20,"print1",2,5,ptr);
//	ptr=&proctab[a];


//	printf("Before calling modified resume\n");
//	printf("printing prio %d",ptr->prprio);

 //	printf("After calling modified resume : print1 priority %d\n",resume(a));

//	printf("Check if priority changed : %d\n",ptr->prprio);

//	wait(NULL);
//	exit(1);
	sleep(1);
	return 0;

}
