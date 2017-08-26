#include <xinu.h>
#include <string.h>
#include <stdio.h>
shellcmd xsh_hello(int n,char *args[]) 
{
	//check if no. of arguments are toomany
	if(n>2)
	{
		printf("too many arguments");

	}
	//check if no. of arguments are less
	else if(n<=1)
	{
		printf("too few arguments");
	}
	//if arguments are proper, greet the string
	else
	{
		printf("Hello %s, Welcome to the world of xinu!!",args[1]);
	}
}


