/*
Name: Andrew Yang
Course: CSCE3600.001
Date: 3/26/2020
Description: C program that will act as a simple command-line interpreter (i.e., a shell) for the Linux kernel
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>//strok()
#include<sys/wait.h>//wait()
#include<unistd.h>//fork(), execvp()
#define LINE_length 100//set the line length

int main()
{
	int index_value=0;//variable of index value
	int pid;//variable of process id
	int i;//variable of loop
	char user_input[100];//string of user input
	char *process;//variable of process
	char *args[LINE_length/2+1];//store the token value

while(1)
{
	printf("minor3>");//prompt the user
	scanf("%[^\n]", user_input);//ask for input
	index_value=0;

	for(i = 0; i < 10; i++)//store the token value using a for loop
	{
	args[i] = (char*)malloc(20*sizeof(char));
	}
	
	if(strcmp(user_input, "quit")==0)//if the user enter quit
	{
		break;
	}

	process=strtok(user_input," ");//make an argument list
	strcpy(args[index_value++],process);//add to string arry

	while(process!=0)// get the next token
	{
		process = strtok(NULL," ");
		if(process == 0)
		break;
	
		strcpy(args[index_value++],process);
	}

	args[index_value]=NULL;
	pid = fork();//create the child process

	if(pid == 0) //child process
	{

		execvp(args[0],args);//execute command using execvp
		printf("%s: command not found\n",args[0]);
		break;
	}

	else//parent process
	{
		wait(NULL);//wait for child to finish
		if(strcmp(user_input,"quit") == 0)
		{	
		break;
		}
	}

	getchar();//skip newline
}
exit(0);
}
