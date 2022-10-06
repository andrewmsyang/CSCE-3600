#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
int main() {

	char *name;
	pid_t pid;
	pid=fork();

	if(pid == 0)
	{
		printf("Child:%d started\n",getpid());
		printf("Child: parent = %d\n",getpid());
		printf("Child is sleeping...\n");
		sleep(25);
		printf("Child has woke up\n");
	}
	else if(pid > 0)
	{
		printf("parent:%d started\n",getpid());
		printf("parent: parent = %d\n",getpid());
	}
	else
	{
		perror("Fork error: ");
	}

	name=(pid==0)?"Child":"parent";
	printf("%s:terminating...\n",name);

return 0;
}
