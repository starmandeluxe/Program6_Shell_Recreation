/*
 * cpl:  A shell pipeline cracker
 *
 * Author: Dr. Phillip Nico
 *         Department of Computer Science
 *         California Polytechnic State University
 *         One Grand Avenue.
 *         San Luis Obispo, CA  93407  USA
 *
 * Email:  pnico@csc.calpoly.edu
 *
 * Revision History:
 *         $Log: main.c,v $
 *         Revision 1.4  2003-04-15 18:11:34-07  pnico
 *         Checkpointing before distribution
 *
 *         Revision 1.3  2003-04-12 10:37:22-07  pnico
 *         added config.h to list of inclusions
 *
 *         Revision 1.2  2003-04-11 08:38:55-07  pnico
 *         Ready for first release of mice
 *
 *         Revision 1.1  2003-04-10 19:36:24-07  pnico
 *         Initial revision
 *
 *
 */
#include "config.h"

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <termios.h>

#include "pipeline.h"
#include "longstr.h"
#include "parser.h"
#include "vssh.h"

//static void printusage(char *name);
static void prompt(char *prompt);

int main(int argc, char *argv[]){
  char *line;
  pipeline pl;
  int run;
  
  //flags
  int redo = 0;
  int skip = 0;
  int isResume = 0;
  
  char *promptstr;
  char* buffer;
  numExits = 0;
  jobIndex = 0;
  histIndex = 0;
  back = 0;
  script = 0;
  
  FILE* in;
  /* check for the right number of arguments */
  //if ( argc > 2 ) {
  //  printusage(argv[0]);
  //  exit(-1);
  //}
  if (argc >= 2)
  {
  	DIR* dirp;
	struct dirent *dp;
	int isFound = 0;
	if ((dirp = opendir(".")) == NULL) 
        {
        	perror("couldn't open path");
                return 0;
        }
        
        //put files into array and sort their names
        while ((dp = readdir(dirp)) != NULL)
	{
		if (strcmp(dp->d_name, argv[1]) == 0)
		{
			isFound = 1;
		}
	}
	if (isFound == 0)
	{
		printf("%s: Command not found\n", argv[1]);
	}
	else
	{
		in = fopen(argv[1], "r");
		buffer = (char*)malloc(sizeof(in));
		script = 1;
	}
  }
  //struct termios saved_term_mode; 
  struct sigaction action;
  action.sa_handler = killHandle;     /* set tick to be the handler function */
  sigemptyset(&action.sa_mask); /* clear out masked functions */
  action.sa_flags   = 0;        /* no special handling */
  
  struct sigaction action2;
  action2.sa_handler = susHandle;     /* set tick to be the handler function */
  sigemptyset(&action2.sa_mask); /* clear out masked functions */
  action2.sa_flags   = 0;        /* no special handling */

    /*
     * Use the sigaction function to associate the signal action with SIGALRM.
     */
    if (sigaction(SIGINT, &action, NULL) < 0 ) {
        fprintf(stderr, "SIGINT\n");
        exit(-1);
    }
    
    if(sigaction(SIGTSTP, &action2, NULL) < 0) {
    	fprintf(stderr, "SIGTSTP\n");
        exit(-1);
    }
    //saved_term_mode = set_raw_term_mode();
	initHistory();
	initJobs();
	//strcpy(jobs[1], "hey");
  /* set prompt */
  promptstr = PROMPT;

  run = TRUE;
  if (script == 0)
  	prompt(promptstr);
  while ( run ) 
  {
    /// if reexecute was not called
    if (redo == 0)
    {
    	if (script == 0)
    	{
	    if ( NULL == (line = readLongString(stdin)) ) 
	    {
	      if ( feof(stdin) )
	        run = FALSE;
	    } 
	}
	else
	{
		fgets(line, sizeof(in), in);
		if (line[0] == '%')
		{
			line++;
			isResume = 1;
		}
	}
	
    }
    else
    {
    	//printf("histInd: %d\n", histIndex);
    	///if reexecute was called,
    	///find the correct place in the history
    	///list to extract the reexecuted command
    	int temp = histIndex;
    	temp--;
    	int ind = 0;
    	while (ind != reNumber)
    	{
    		temp--;
    		ind++;
    	}
    	line = history[temp];
    	printf("%s\n", line);
    }
    if (line == NULL)
    {
	    if (feof(stdin))
        run = FALSE;
    }
    else 
    {
    	if (line[0] == '%')
	{
		line++;
		isResume = 1;
	}
      /* We got a line, send it off to the pipeline cracker and
       * launch it
       */
       //if (redo == 0)
       //{
       	  pl = crack_pipeline(line);     	  
       //}
	redo = 0;
      /*
       * Show that it worked.  This is where you're going to do
       * something radically different: rather than printing the
       * pipeline, you're going to execute it.
       */
       
      if ( pl != NULL && strlen(pl->cline) > 0) 
      {
      	/*
      	if (pl->cline[0] == '%')
        {
        	int i;
        	isResume = 1;
        	pl->stage[0].argv[0][0] = ' ';
        	for (i = 0; i < strlen(pl->cline)-1; i++)
        	{
        		pl->stage[0].argv[0][i] = pl->stage[0].argv[0][i+1];
        	}
        	pl->stage[0].argv[0][strlen(pl->cline)-1] = '\0';
        }
        
        */
        //print_pipeline(stdout,pl); /* print it. */
        addHistory(pl->cline);
        
        ///add to the history what was typed into the command line
        if (strcmp(pl->stage[0].argv[0], "history") == 0)
        {
        	///print the history if possible
        	if (pl->stage[0].argc > 1)
        	{
        		printf("Junk after built-in command\n");
        		skip = 1;
        	}
        	numExits = 0;
        	if (skip == 0)
        		printHistory();
        	skip = 0;
        }
        else if (strcmp(pl->stage[0].argv[0], "jobs") == 0)
        {
        	///print the suspended jobs if possible
        	if (pl->stage[0].argc > 1)
        	{
        		printf("Junk after built-in command\n");
        		skip = 1;
        	}
        	numExits = 0;
        	if (skip == 0)
        		printJobs();
        	skip = 0;
        }
        else if (strcmp(pl->stage[0].argv[0], "exit") == 0)
        {
        	///exit vssh if possible
        	if (pl->stage[0].argc > 1)
        	{
        		fprintf(stderr, "Junk after built-in command\n");
        		skip = 1;
        	}
        	numExits++;
        	if (skip == 0)
        		exitCmd();
        	skip = 0;
        }
        else if (strcmp(pl->stage[0].argv[0], "bg") == 0)
        {
        	///background the most recently suspended job if possible
        	if (pl->stage[0].argc > 1)
        	{
        		fprintf(stderr, "Junk after built-in command\n");
        		skip = 1;
        	}
        	numExits = 0;
        	if (skip == 0)
        		bg(jobs[jobIndex-1]);
        	skip  = 0;
        }
        else if (strcmp(pl->stage[0].argv[0], "fg") == 0)
        {
        	///foreground the most recently backgrounded job if possible
        	if (pl->stage[0].argc > 1)
        	{
        		fprintf(stderr, "Junk after built-in command\n");
        		skip = 1;
        	}
        	numExits = 0;
        	if (skip == 0)
        		fg(jobs[jobIndex-1]);
        	skip = 0;
        }
        else if (strcmp(pl->stage[0].argv[0], "cd") == 0)
        {
        	///change the directory of vssh if possible
        	if (pl->stage[0].argc > 2)
        	{
        		fprintf(stderr, "Junk after built-in command\n");
        		skip = 1;
        	}
        	numExits = 0;
        	if (skip == 0)
        		cd(pl->stage[0].argv[1]);
        	skip = 0;
        }
        else if (isResume == 1)
        {
        	///foreground the specified job if possible
        	if (pl->stage[0].argc > 1)
        	{
        		fprintf(stderr, "Junk after built-in command\n");
        		skip = 1;
        	}
        	numExits = 0;
        	if (skip == 0)
        	{
        		//pl->stage[0].argv[0]++;
        		int i; 
        		int killFlag = 0;
        		for (i = 0; i < strlen(pl->stage[0].argv[0]); i++)
        		{
        			if (!isdigit(pl->stage[0].argv[0][i]))
        			{
        				///if the supplied job number is not a number
        				///then print error
        				killFlag = 1;
        				fprintf(stderr, "%s: No such job.\n", pl->stage[0].argv[0]);	
        			}
        		}
        		if (killFlag == 0)
        		{
        			if (atoi(pl->stage[0].argv[0]) <= 0 || atoi(pl->stage[0].argv[0]) > jobIndex)
        				///if the supplied job number is not a valid job index
        				///then print error
        				fprintf(stderr, "%s: No such job.\n", pl->stage[0].argv[0]);
        			else
        				resume(jobs[atoi(pl->stage[0].argv[0])-1]->id, atoi(pl->stage[0].argv[0])-1);
        		}
        	}
        	skip = 0;
        }
        else if (strcmp(pl->stage[0].argv[0], "!") == 0)
        {
        	///reexecute the specified history command if possible
        	if (pl->stage[0].argc > 2 || strlen(pl->stage[0].argv[1]) > 3)
        	{
        		fprintf(stderr, "Junk after built-in command\n");
        		skip = 1;
        	}
        	///if the number supplied is not a valid history index
        	///then print error
        	if (pl->stage[0].argv[1][0] == '0' && skip == 0)
        	{
        		fprintf(stderr, "%s: Event not found\n", pl->stage[0].argv[1]);
        		skip = 1;
        	}
        	if (strlen(history[atoi(pl->stage[0].argv[1])]) <= 0 && skip == 0)
		{
			fprintf(stderr, "%s: Event not found\n", pl->stage[0].argv[1]);
			skip = 1;
		}
        	int i;
        	for (i = 0; i < strlen(pl->stage[0].argv[1]); i++)
        	{
			if (!isdigit(pl->stage[0].argv[1][i]) && skip == 0)
			{
				///if the number supplied is not a number
        			///then print error
				fprintf(stderr, "%s: Event not found\n", pl->stage[0].argv[1]);
				skip = 1;
			}
		}
        	numExits = 0;
        	if (skip == 0)
        	{
        		redo = 1;
        		reNumber = atoi(pl->stage[0].argv[1]);
        		//printf("renum: %d\n", reNumber);
        		//pl = crack_pipeline(history[atoi(pl->stage[0].argv[1])]);
			//printf("%s\n", pl->cline);
			//lineno++;
        		//continue;
        	}
        	skip = 0;
        }
        else if (strcmp(pl->stage[0].argv[0], "kill") == 0)
        {
        	///kill the specified job if possible
        	if (pl->stage[0].argc > 2)
        	{
        		fprintf(stderr, "Junk after built-in command\n");
        		skip = 1;
        	}
        	numExits = 0;
        	if (skip == 0)
        	{
        		int i; 
        		int killFlag = 0;
        		for (i = 0; i < strlen(pl->stage[0].argv[1]); i++)
        		{
        			///if the supplied job number is not a number
        			///then print error
        			if (!isdigit(pl->stage[0].argv[1][i]))
        			{
        				killFlag = 1;
        				fprintf(stderr, "%s: No such job.\n", pl->stage[0].argv[1]);	
        			}
        		}
        		if (killFlag == 0)
        		{
        			if (atoi(pl->stage[0].argv[1]) <= 0 || atoi(pl->stage[0].argv[1]) > jobIndex)
        				///if the supplied job number is not a valid job index
        				///then print error
        				fprintf(stderr, "%s: No such job.\n", pl->stage[0].argv[1]);
        			else
        				killProcess(atoi(pl->stage[0].argv[1])-1, jobs[atoi(pl->stage[0].argv[1])-1]->id);
        		}
        	}
        	skip = 0;
        }
        else
        {
        	///pipe/redirect as necessary
        	numExits = 0;
        	//int i;
        	//for (i = 0; i < pl->length; i++)
        	//{
        		
	        	//else
	        	//{
	        		//handle backgrounding "&"
	        			//find the &
		        		int j;
		        		int andFound = 0;
		        		int pipeFound = 0;
		        		for (j = 1; j < strlen(pl->cline); j++)
		        		{
		        			if (pl->cline[j] == '|')
		        			{
		        				//fprintf(stderr, "Pipelines cannot be backgrounded\n");
		        				pipeFound = 1;
		        			}
		        			else if (pl->cline[j] == '&')
		        			{
		        				//error if & is not the last arg, exec nothing
		        				if (pipeFound == 1)
		        				{
		        					fprintf(stderr, "Pipelines cannot be backgrounded\n");	
		        				}
		        				else if (j != strlen(pl->cline) - 1)
		        				{
		        					andFound = 1;
		        					fprintf(stderr, "Junk after '&'.\n");
		        				}
		        				else if (pl->cline[j-1] == ' ')
		        				{
		        					andFound = 1;
		        					pl->cline[j] = '\0';
		        					//backExec(pl->stage[0].argv[0], pl->stage[0].argv, pl->cline);
		        					back = 1;
		        				}
		        			}
		        		}
		        if (pl->length >= 2)
	        	{
	        		///pipe if the pipline has 2 or more stages
	        		pipeCmd(pl);
	        	}
	        	else if (pl->stage[0].outname != NULL && pl->stage[0].inname != NULL)
	        	{
	        		redirectBoth(pl->stage[0].argv, pl->stage[0].inname, pl->stage[0].outname);
	        	}
	        	else if (pl->stage[0].outname != NULL)
	        	{
	        		///otherwise redirect output if there is an outfile
	        		redirectOutput(pl->stage[0].argv, pl->stage[0].outname, pl->cline);	
	        	}
	        	else if (pl->stage[0].inname != NULL)
	        	{
	        		///otherwise redirect input if there is an infile
	        		redirectInput(pl->stage[0].argv, pl->stage[0].inname, pl->cline);	
	        	}
		        else if (andFound == 0)
		        {
			        //execute if not a built-in command
			        execute(pl->stage[0].argv[0], pl->stage[0].argv, pl->cline);
			}
			back = 0;
	        	//}
	        	
	        //}
        }
      }
      free_pipeline(pl);   
      fflush(stdout);       /* also frees line */

      lineno++;  /* readLongString trims newlines, so increment it manually */
    }
    if (run )
    {     
    	if (script == 0)            /* assuming we still want to run */
      		prompt(promptstr);
    }
  }
  return 0;
}

static void prompt(char *pr) {
  /* If this is an interactive shell, flush the output streams and
   * print a prompt
   */

  if ( isatty(STDIN_FILENO) && isatty(STDOUT_FILENO) ) {
    printf("%s", pr);
    fflush(stdout);
  }
}

//static void printusage(char *name){
//  fprintf(stderr,"usage: %s\n",name);
//}
