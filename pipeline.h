#ifndef PIPELINE
#define PIPELINE

/**
 * This file defines a pipeline data structure that represents a shell command
 * line.  A pipeline is a list of command-line stages, defined by the clstage
 * structure.  A stage contains the name of a command, its redirected input,
 * redirected output, argc, and argv.
 *
 * To see what a pipeline looks like for a shell command, run the parse-cl
 * program.  It will dump the pipeline structure for each command line input
 * after the "what? " prompt.
 */

#include <stdio.h>
#include <sys/types.h>
#include "stringlist.h"

typedef struct clstage *clstage;

struct clstage {
  char *inname;                 /* input filename (or NULL for stdin) */
  char *outname;                /* output filename (NULL for stdout)  */
  int  argc;                    /* argc and argv for the child        */
  char **argv;                  /* Array for argv                     */

  clstage next;                 /* link pointer for listing in the parser */
};

typedef struct pipeline {
  char           *cline;              /* the original command line  */
  int            length;              /* length of the pipeline     */
  struct clstage *stage;              /* descriptors for the stages */
} *pipeline;


/* prototypes for pipeline.c */
extern void     print_pipeline(FILE *where, pipeline cl);
extern void     free_pipeline(pipeline cl);
extern pipeline crack_pipeline(char *line);
extern clstage  make_stage(slist l);
extern void     free_stage(clstage s);
extern void     free_stagelist(clstage s);
extern clstage  append_stage(clstage s, clstage t);
extern pipeline make_pipeline(clstage stages);
extern int      check_pipeline(pipeline pl, int lineno);

#endif
