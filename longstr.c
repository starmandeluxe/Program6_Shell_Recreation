/*
 * Read long strings
 *
 *
 * Revision History:
 *
 *    $Log: longstr.c,v $
 *    Revision 1.3  2003-04-12 10:37:33-07  pnico
 *    added config.h to list of inclusions
 *
 *    Revision 1.2  2003-04-12 10:26:41-07  pnico
 *    removed uses of __FUNCTION__ because MINIX doesn't
 *    support it.
 *
 *    Revision 1.1  2003-04-10 19:36:27-07  pnico
 *    Initial revision
 *
 *    Revision 1.1  2002-05-23 14:13:24-07  pnico
 *    Initial revision
 *
 *    Revision 1.1  2002-05-16 15:09:20-07  pnico
 *    Initial revision
 *
 *
 */
#include "config.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define CHUNK 80

char *readLongString(FILE *infile){
  /* readLongString() reads a string of arbitrary length from the given
   * file and returns it as a newly allocated string
   *
   * The technique is to read the string into an allocated buffer, increasing
   * the buffer size as necessary.
   *
   * The caller is responsible for freeing the allocated string
   *
   * returns
   *   on success: a pointer to the newly allocated string.
   *   on EOF without reading something or read-error: NULL
   */
  int len;
  char *buff;
  char *ret;
  int size=0;
  char c;

  /* Allocate the initial buffer to hold the string as we grow */
  size=CHUNK;
  if(NULL==(buff=(char*)malloc(size * sizeof(char)))) {
    perror("readLongString");
    exit(-1);
  }

  for( len=0, c=getc(infile); (c!='\n') && (c!=EOF) ; c=getc(infile) ) {
    if( len >= size-1 ) {         /* buffer is too small */
      size += CHUNK;
      if(NULL==(buff=(char*)realloc(buff,size * sizeof(char)))) {
        perror("readLongString");
        exit(-1);
      }
    }
    buff[len++]=c;
  }
  buff[len]='\0';         /* final nul */

  /* check for errors */
  if ( c == EOF ) {
    /* we hit an end of file or a read error.
     * if it's a legit EOF and we have read something, return
     * just that, otherwise toss the string and return NULL
     *  At this point, len holds the length of the string read.
     */
    if ( !feof(infile) || ( len == 0 ) ) {
      ret = NULL;
      free(buff);
      buff = NULL;
    }
  }

  if ( buff ) {
    /* make a copy that's an exact fit. */
    ret = (char *) malloc(len+1);
    if ( ret )                  /* check to be sure malloc() worked */
      strcpy(ret,buff);
    else
      ret = NULL;
    free(buff);
  }

  return ret;
}
