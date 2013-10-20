#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <strings.h>
#include <unistd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <stdint.h>
#include <pwd.h>
#include <ftw.h>
#include "pipeline.h"
#include "config.h"
#include "longstr.h"
#include "parser.h"
#include <sys/statvfs.h>
#define sus 1
#define PROMPT "%- "

typedef struct process
{
	char* name;
	int id;
	int status;
} *process;

int numExits;
int jobIndex;
int reNumber;
int histIndex;
int back;
int script;
int globalPid;
char* history[100];
process jobs[100];
void initJobs();
void printJobs();
void printHistory();
void initHistory();
void addHistory(char* item);
void addJob(int pid, char* cmd);
void exitCmd();
void killHandle(int sig);
void susHandle(int sig);
void bg(process p);
void fg(process p);
void killProcess(int job, pid_t id);
void suspend(pid_t id);
void resume(pid_t id, int job);
void cd(char* path);
void redirectInput(char** argv, char* infile, char* cline);
void redirectOutput(char** argv, char* outfile, char* cline);
void redirectBoth(char** argv, char* infile, char* outfile);
void pipeCmd(pipeline pl);
void backExec(char* prog, char* argv[], char* cmd);
void execute(char* prog, char* argv[], char* cmd);
