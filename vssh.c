/*! \file
*
* Alex Kort, 5-25-06, Program 6, vssh.c
*
* NOTE: Most commands are functional,
* under the correct test conditions. most
* tests were done with sfmt. resume % had
* string parsing problems
*
*/
#include "vssh.h"
#include "pipeline.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <signal.h>
///function that prints all suspended jobs
void printJobs()
{
	int i;
	int k = 0;
	for (i = 0; i < 100; i++)
	{
		if (jobs[i]->status == sus && strlen(jobs[i]->name) > 0)
		{
			k++;
			printf("[%d] %s\n", k, jobs[i]->name);
		}
	}
}

///function that initializes history list to strings with zero
///length, for reallocation later
void initHistory()
{
	int i;
	for (i = 0; i < 100; i++)
	{
		history[i] = (char*)malloc(0);
	}
}
///function that initializes the jobs list to strings with zero
///length, for reallocation later
void initJobs()
{
	int i;
	for (i = 0; i < 100; i++)
	{
		jobs[i] = (process)malloc(sizeof(process));
		jobs[i]->name = (char*)malloc(0);
	}
}

///adds a job to keep track of child processes
void addJob(int pid, char* cmd)
{
	realloc(jobs[jobIndex]->name, strlen(cmd));
	strcpy(jobs[jobIndex]->name, cmd);
	jobs[jobIndex]->id = pid;
	jobIndex++;
}

///prints history starting from the current index,
///which is the most recent command, then going back
///to the oldest command
void printHistory()
{
	/*
	int i;
	for (i = 0; i < 100; i++)
	{
		if (strlen(history[i]) > 0)
			printf("[%d] %s\n", i+1, history[i]);
	}
	*/
	//print history backwards, loop around
	int start;
	int index = 1;
	if (histIndex == 0)
		start = 99;
	else	
		start = histIndex - 1;
	while (start != histIndex)
	{
		if (strlen(history[start]) > 0)
			printf("[%d] %s\n", index, history[start]);
		start--;
		if (start < 0)
		{
			start = 99;
		}
		index++;
		if (index == 101)
		{
			index = 1;
		}
	}
}

///function that adds an item to the history list
void addHistory(char* item)
{
	/*
	int i;
	for (i = 99; i > 0; i--)
	{
		realloc(history[i], sizeof(history[i-1]));
		strcpy(history[i], history[i-1]);
	}
	//history[0] = (char*)malloc(strlen(item));
	realloc(history[0], strlen(item));
	strcpy(history[0], item);
	*/
	realloc(history[histIndex], strlen(item));
	strcpy(history[histIndex], item);
	//printf("Added: %s\n", history[histIndex]);
	histIndex++;
	if (histIndex == 100)
		histIndex = 0;
}

///function that exits, but first checks if there are 
///suspended jobs before exiting. If exit is typed 
///twice the program will exit.
void exitCmd()
{
	if (numExits > 1)
	{
		int i; 
		for (i = 0; i < 100; i++)
		{
			if (strlen(jobs[i]->name) > 0 && jobs[i]->status == sus)
				kill(jobs[i]->id, SIGKILL);
		}
		printf("exit\n");
		exit(0);	
	}
	int i;
	int jobsExist = 0;
	for (i = 0; i < 100; i++)
	{
		if (strlen(jobs[i]->name) > 0 && jobs[i]->status == sus)
			jobsExist = 1;
	}
	if (jobsExist == 1)
	{
		printf("There are suspended jobs.\n");
	}
	else if (jobsExist == 0)
	{
		printf("exit\n");
		exit(0);
	}
}

///backgrounds the specified process
void bg(process p)
{
	kill(p->id, SIGCONT);
}

///foregrounds the specified process
void fg(process p)
{
	kill(p->id, SIGCONT);
	waitpid(p->id, NULL, 0);
}

///function that changes the current working 
///directory of vssh
void cd(char* path)
{
	chdir(path);
}

///signal handler for killing a process,
///when Ctrl-C is pressed
void killHandle(int sig)
{
	int jobsExist = 0;
	//int pid = getpid();
	int pid = globalPid;
	int i;
	for (i = 0; i < 100; i++)
	{
		
		if (jobs[i]->id == pid)
		{
			jobsExist = 1;
			///find the id of the currently 
			///running process
			jobs[i]->status = sus;
			break;
		}
	}
	if (jobsExist == 1)
	{
		
		killProcess(i, jobs[i]->id);
	}
	//else
		printf("\n");
}

///signal handler for suspending a process,
///when Ctrl-V is pressed
void susHandle(int sig)
{
	int jobsExist = 0;
	//int pid = getpid();
	int pid = globalPid;
	
	int i;
	for (i = 0; i < 100; i++)
	{
		if (strlen(jobs[i]->name) > 0)
		{
			
			if (jobs[i]->id == pid)
			{
				jobsExist = 1;
				///find the id of the currently 
				///running process
				jobs[i]->status = sus;
				break;
			}
						
		}
	}
	
	if (jobsExist == 1)
	{
		//suspend(jobs[i]->id);
		suspend(pid);
	}
	//else
	//{
		printf("\n");
	//}
}

///function that does the killing of the process
void killProcess(int job, pid_t id)
{
	jobs[job]->status = 0;
	kill(id, SIGINT);	
}

///suspend and add to suspended job list
void suspend(pid_t id)
{
	kill(id, SIGTSTP);
}


///function that resumes a suspended process
///by sending it the SIGCONT signal
void resume(pid_t id, int job)
{
	kill(id, SIGCONT);
	jobs[job]->status = 0;
	
}

///function that redirects both input and output
void redirectBoth(char** argv, char* infile, char* outfile)
{
	pid_t id1;
	if ((id1 = fork()) < 0) 
	{
		perror("fork");
		exit(0);
	}
	else if (id1 == 0)
	{	
		int fd1 = open(infile, O_RDONLY, S_IRUSR);
		int fd2 = open(outfile, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
		
		///send input from the infile, send the output to the outfile
		dup2(fd1, STDIN_FILENO);
		dup2(fd2, STDOUT_FILENO);
		close(fd1);
		execvp(argv[0], argv);
		
		fprintf(stderr, "%s: Command not found.\n", argv[0]);
	}
	else
	{
		if (back == 0)
			waitpid(id1, NULL, 0);
	}
}

///function that redirects input
void redirectInput(char** argv, char* infile, char* cline)
{
	
	pid_t id1;
	int id;
	
	if ((id1 = fork()) < 0) 
	{
		perror("fork");
		exit(0);
	}
	id = id1;
	if (id1 == 0)
	{	
		int fd1 = open(infile, O_RDONLY, S_IRUSR);
		///send input from the infile
		dup2(fd1, STDIN_FILENO);
		close(fd1);
		execvp(argv[0], argv);
		
		fprintf(stderr, "%s: Command not found.\n", argv[0]);
	}
	else
	{
		addJob(id, cline);
		if (back == 0)
			waitpid(id1, NULL, 0);
	}
}

///function that redirects output
void redirectOutput(char** argv, char* outfile, char* cline)
{
	pid_t id1;
	int id;
	

	int fd1 = open(outfile, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
	
	if ((id1 = fork()) < 0) 
	{
		perror("fork");
		exit(0);
	}
	id = id1;
	if (id1 == 0)
	{	
		
		///send output to the outfile
		dup2(fd1, STDOUT_FILENO);
		close(fd1);
		execvp(argv[0], argv);
		fprintf(stderr, "%s: Command not found.\n", argv[0]);
	}
	else
	{
		addJob(id, cline);
		if (back == 0)
			waitpid(id1, NULL, 0);
	}
}

///piping function that handles redirection and pipes
///over the supplied pipeline
void pipeCmd(pipeline pl)
{
	int i;
	for (i = 0; i < pl->length; i++)
	{
		int fd[2];
		int fd2[2];
		
		int fd1;
		///pipe in pairs, until there are no more pairs left
		if (i+1 < pl->length)
		{
			
			pipe(fd);
			if (pl->stage[i].inname != NULL)
			{
				///redirect input if necessary
				pid_t id1;
	
				int id;
				if ((id1 = fork()) < 0) 
				{
					perror("fork");
					exit(0);
				}
				id = id1;
				if (id1 == 0)
				{	
					
					fd1 = open(pl->stage[i].inname, O_RDONLY, S_IRUSR);
					
					
					dup2(fd1, STDIN_FILENO);
					dup2(fd[1], STDOUT_FILENO);
					close(fd1);
					execvp(pl->stage[i].argv[0], pl->stage[i].argv);
					
					fprintf(stderr, "%s: Command not found.\n", pl->stage[i].argv[0]);
				}
				else
				{
					//addJob(id, pl->cline);
					//waitpid(id1, NULL, 0);
				}
			}
			else
			{
				///otherwise simply execute
				int pid;
		   		int id;
				if ((pid = fork()) < 0)
				{
					perror("fork");
					exit(-1);
				}
				id = pid;
				
				if (pid == 0)
				{
					///send pipe to standard out
					dup2(fd[1], STDOUT_FILENO);
					execvp(pl->stage[i].argv[0], pl->stage[i].argv);
					
					fprintf(stderr, "%s: Command not found.\n", pl->stage[i].argv[0]);
					exit(-1);
				}
				else
				{
					//addJob(id, pl->cline);
					//waitpid(pid, NULL, 0);
				}
			}
			
			if (pl->stage[i+1].outname != NULL)
			{
				///at the next stage, redirect output if necessary
				pid_t id2;
				int id;
				fd1 = creat(pl->stage[i+1].outname, 0755);
				if ((id2 = fork()) < 0) 
				{
					perror("fork");
					exit(0);
				}
				id = id2;
				if (id2 == 0)
				{	
					
					
					dup2(fd[0], STDIN_FILENO);
					dup2(fd1, STDOUT_FILENO);
					close(fd1);
					if (i == pl->length - 1)
					{
						dup2(fd[1], fd2[0]);
					}
						close(fd[1]);
					execvp(pl->stage[i+1].argv[0], pl->stage[i+1].argv);
					
					fprintf(stderr, "%s: Command not found.\n", pl->stage[i+1].argv[0]);
				}
				else
				{
					//addJob(id, pl->cline);
					//waitpid(id2, NULL, 0);
				}
			}
			else
			{
				///otherwise simply execute the next stage
				int pid;
		   		int id;
				if ((pid = fork()) < 0)
				{
					perror("fork");
					exit(-1);
				}
				id = pid;
				
				if (pid == 0)
				{
					dup2(fd[0], STDIN_FILENO);
					//dup2(fd[1], STDOUT_FILENO);
					//if (i != pl->length - 1)
					if (i == pl->length - 1)
					{
						dup2(fd[1], fd2[0]);
					}
						close(fd[1]);
					execvp(pl->stage[i+1].argv[0], pl->stage[i+1].argv);
					
					fprintf(stderr, "%s: Command not found.\n", pl->stage[i+1].argv[0]);
					exit(-1);
				}
				else
				{
					//addJob(id, pl->cline);
					//waitpid(pid, NULL, 0);
				}
			}
			close(fd[0]);
			close(fd[1]);
			close(fd1);	
		}
		///if the pipe is at the end
		else if (pl->length%2 != 0 && i == pl->length-1)
		{
			
			pipe(fd2);
			//printf("LAST\n");
			if (pl->stage[i].outname != NULL)
			{
				///at the next stage, redirect output if necessary
				pid_t id2;
				int id;
				fd1 = creat(pl->stage[i].outname, 0755);
				if ((id2 = fork()) < 0) 
				{
					perror("fork");
					exit(0);
				}
				id = id2;
				if (id2 == 0)
				{	
					
					
					dup2(fd2[0], STDIN_FILENO);
					dup2(fd1, STDOUT_FILENO);
					close(fd1);
					close(fd2[1]);
					execvp(pl->stage[i].argv[0], pl->stage[i].argv);
					
					fprintf(stderr, "%s: Command not found.\n", pl->stage[i+1].argv[0]);
				}
				else
				{
					//addJob(id, pl->cline);
					//waitpid(id2, NULL, 0);
				}
			}
			else
			{
				///otherwise simply execute the next stage
				int pid;
		   		int id;
				if ((pid = fork()) < 0)
				{
					perror("fork");
					exit(-1);
				}
				id = pid;
				if (pid == 0)
				{
					dup2(fd2[0], STDIN_FILENO);
					//dup2(fd[1], STDOUT_FILENO);
					close(fd2[1]);
					execvp(pl->stage[i].argv[0], pl->stage[i].argv);
					
					fprintf(stderr, "%s: Command not found.\n", pl->stage[i].argv[0]);
					exit(-1);
				}
				else
				{
					//addJob(id, pl->cline);
					//waitpid(pid, NULL, 0);
				}
			}
			close(fd2[0]);
			close(fd2[1]);
			close(fd1);	
		}
		close(fd[0]);
		close(fd[1]);
		close(fd1);
	}
}

///executes a program in the background
void backExec(char* prog, char* argv[], char* cmd)
{
	int pid;
		   	
	if ((pid = fork()) < 0)
	{
		perror("fork");
		exit(-1);
	}
	addJob(pid, cmd);
	if (pid == 0)
	{
		execvp(prog, argv);
		
		fprintf(stderr, "%s: Command not found.\n", prog);
		exit(-1);
	}
	else
	{
		//waitpid(pid, NULL, 0);
	}
}

///function that executes a program
void execute(char* prog, char* argv[], char* cmd)
{
	
	struct stat statbuf;

		
		int id;
		lstat(prog, &statbuf);
		
	   	
		   	int pid;
		   	
			if ((pid = fork()) < 0)
			{
				perror("fork");
				exit(-1);
			}
			id = pid;
			globalPid = pid;
			//addJob(pid, cmd);
			if (pid == 0)
			{
				execvp(prog, argv);
				
				fprintf(stderr, "%s: Command not found.\n", prog);
				exit(-1);
			}
			else
			{
				addJob(id, cmd);
				if (back == 0)
					waitpid(pid, NULL, 0);
			}
	   	
}


