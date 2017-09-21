#include<xinu.h>
#include<stdio.h>
#include<stdlib.h>

void print(int x)
{
printf("In print function %d\n",x);

}
shellcmd xsh_resume_test(int nargs, char* args[])
{
int r,a;
struct procent *ptr;

 a=create(print,1024,20,"print",1,5);

ptr=&proctab[a];

printf("Before calling modfied resume \n");
printf("printing state %d",ptr->prstate);

//printf("printing prio  %d",ptr->prprio);

r=resume(a);

printf("After calling modfied resume \n");
printf("printing state %d",ptr->prstate);
//printf("printing prio  %d",ptr->prprio);

//sleep(1);

//printf("In main function printing priority %d\n",r);

return 0;

}
