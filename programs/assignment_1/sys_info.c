#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
int main(int n,char *args[])
{
        int pid1,nb;
        int fd[2];


        char readbuffer[80];
        char s[25];
        char s1[25];

        pipe(fd);
        pid1=fork();

        if(pid1<0)
	{
        	printf("fork failed");
	}		

	if(pid1==0)
	{
        	printf("child pid %d\n",getpid());
        	//1 port is used to write,0 to read.
        	 //since child should read the string from parent, we are closing the wri        te port
                 close(fd[1]);
                 nb = read(fd[0], readbuffer, sizeof(readbuffer));
        
                 //check if the argument passed is echo
                 if(strcmp(args[1],"echo")==0)
                 {
                     //preparing a string /bin/echo
                     strcpy(s1,"/bin/");
                     strcat(s1,args[1]);
                     execl(s1,"echo","helloworld",0);
                 }
                 //else preparing a string /bin/date
                 if(strcmp(args[1],"date")==0)
		 {
		 	strcpy(s,"/bin/");
                 	strcat(s,args[1]);                                                                                                                                                      execl(s,"date",0,0);
        	                       		                                                                                                                      			}
        }
	else
	{
		printf("parent pid %d\n",getpid());	                                                                                                                                     	//parent needs to write to child, hence closing the read port
    	 	close(fd[0]);
	  	write(fd[1],args[1],(strlen(args[1])+1));
          	wait(NULL);
        }
}
