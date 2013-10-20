#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
# define U(x) x
# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# ifndef YYLMAX 
# define YYLMAX BUFSIZ
# endif 
#ifndef __cplusplus
# define output(c) (void)putc(c,yyout)
#else
# define lex_output(c) (void)putc(c,yyout)
#endif

#if defined(__cplusplus) || defined(__STDC__)

#if defined(__cplusplus) && defined(__EXTERN_C__)
extern "C" {
#endif
	int yyback(int *, int);
	int yyinput(void);
	int yylook(void);
	void yyoutput(int);
	int yyracc(int);
	int yyreject(void);
	void yyunput(int);
	int yylex(void);
#ifdef YYLEX_E
	void yywoutput(wchar_t);
	wchar_t yywinput(void);
#endif
#ifndef yyless
	int yyless(int);
#endif
#ifndef yywrap
	int yywrap(void);
#endif
#ifdef LEXDEBUG
	void allprint(char);
	void sprint(char *);
#endif
#if defined(__cplusplus) && defined(__EXTERN_C__)
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
	void exit(int);
#ifdef __cplusplus
}
#endif

#endif
# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
# define yymore() (yymorfg=1)
#ifndef __cplusplus
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
#else
# define lex_input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
#endif
#define ECHO fprintf(yyout, "%s",yytext)
# define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng;
#define YYISARRAY
char yytext[YYLMAX];
int yymorfg;
extern char *yysptr, yysbuf[];
int yytchar;
FILE *yyin = {stdin}, *yyout = {stdout};
extern int yylineno;
struct yysvf { 
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;};
struct yysvf *yyestate;
extern struct yysvf yysvec[], *yybgin;

# line 3 "lexer.l"
/*
 * lexer.l:  A scanner for cracking shell pipelines.  Rather than
 * reading stdin, though, this parser is set up to read from a
 * passed string.
 *
 * $Log: lexer.l,v $
 * Revision 1.6  2003-04-15 18:11:33-07  pnico
 * Checkpointing before distribution
 *
 * Revision 1.5  2003-04-10 19:35:30-07  pnico
 * Checkpointing distribution version
 *
 * Revision 1.4  2003-04-10 14:33:03-07  pnico
 * checkpointing the lexer before abandonin the approach ofo
 * multi-line strings
 *
 * Revision 1.3  2003-04-09 18:18:19-07  pnico
 * checkpointing before trying to clean the scanner for
 * Minix
 *
 * Revision 1.2  2003-04-06 15:34:22-07  pnico
 * scanner properly supports strings.  Checkpointing
 * before removing line-oriented interface.
 *
 * Revision 1.1  2003-04-06 14:16:27-07  pnico
 * Initial revision
 *
 * Revision 1.1  2003-04-06 14:15:21-07  pnico
 * Initial revision
 *
 *
 */

#include "config.h"

#include <stdio.h>
#include "stringstuff.h"
#include "parser.h"
#include <unistd.h>

int lineno=1;

#ifdef DEBUG_SCANNER
#define announce(s)             {fprintf(stdout,s);    fflush(stdout);}
#define announce1(s,arg)        {fprintf(stdout,s,arg);fflush(stdout);}
#else
#define announce(s)             /* nothing */
#define announce1(s,arg)        /* nothing */
#endif


# line 53 "lexer.l"
/* Set up the input appropriately for the scanner.  Normally
 * working with lex/flex is very easy, but this parser has to
 * work on several different versions of unix.  That, naturally,
 * breeds complication...
 */
#ifdef FLEX_SCANNER
  #if ((YY_FLEX_MAJOR_VERSION < 2) || \
      ((YY_FLEX_MAJOR_VERSION <= 2) && (YY_FLEX_MINOR_VERSION < 5)))
    /* Set up the input to read from a string, not from standard in
     * This is for "old" versions of flex
     */
    #ifdef YY_INPUT
    #undef YY_INPUT
    #endif
    #define YY_INPUT(buf,result,max_size) {\
                          int c;                                \
                          c=nextfromstring();                   \
                          if (c==EOF) {                         \
                            result=YY_NULL;                     \
                          } else {                              \
                            *buf = c;                           \
                            result=1;                           \
                          }                                     \
                        }
  extern int nextfromstring(void); /* give it a prototype... */
  #else
  /* newer versions of FLEX can do it all by themselves. */
  #define SCANSTRING_DEFINED
  #endif
#else
  /* for real lex */
  #undef input
  #define input()  nextfromstring()
  #undef unput
  #define unput(c) putbackonstring(c)
#endif

# line 89 "lexer.l"
/************************* Done setting up the input. ************/
# define YYNEWLINE 10
int yylex(){
int nstr; extern int yyprevious;
#ifdef __cplusplus
/* to avoid CC and lint complaining yyfussy not being used ...*/
static int __lex_hack = 0;
if (__lex_hack) goto yyfussy;
#endif
while((nstr = yylook()) >= 0)
yyfussy: switch(nstr){
case 0:
if(yywrap()) return(0); break;
case 1:

# line 102 "lexer.l"
               { /* ignore comment */; }
break;
case 2:

# line 104 "lexer.l"
                 { /* ignore whitespace*/; }
break;
case 3:

# line 106 "lexer.l"
                   { lineno++; return T_newline; }
break;
case 4:

# line 108 "lexer.l"
                    { announce("<T_from>\n"); return T_from; }
break;
case 5:

# line 110 "lexer.l"
                    { announce("<T_into>\n"); return T_into; }
break;
case 6:

# line 112 "lexer.l"
                    { announce("<T_pipe>\n"); return T_pipe; }
break;
case 7:

# line 114 "lexer.l"
               {
			  int token,ok;
			  ok=matchquotes(yytext);
                          yylval.v.string=cleancpystring(yytext);
                          if ( ok  ) {
                            announce1("<T_string(%s)>\n",yylval.v.string);
			    token=T_string;
                          } else {
                            announce1("<T_badstring(%s)>\n",yylval.v.string);
			    fprintf(stderr,"Unterminated string, line %d.\n",
				    lineno);
			    token=T_badstring;
                          } 
                          lineno += countlines(yylval.v.string);
                          return token;
                        }
break;
case -1:
break;
default:
(void)fprintf(yyout,"bad switch yylook %d",nstr);
} return(0); }
/* end of yylex */

# line 132 "lexer.l"

/* yywrap must be defined, but all it has to do is to tell lex
 * that there is no more input by returning true.
 */
#ifndef yywrap
int yywrap() {return 1;}
#endif


#ifndef SCANSTRING_DEFINED

/* these data and routines are only needed if yy_scan_string is not
 * defined
 */
static char *parsestring=NULL;
static int  atend;
static int  sindex;
static int  pushedback=FALSE;
static int  unputchar;

int nextfromstring(void) {
  /* return the next element from the string, or a newline
   * at the end of the string.
   *   NOTE:  This means that when using this input method
   * the scanner will _never_ see an EOF.  It does avoid a
   * problem under old flex where the scanner latches the EOF
   * state and never relinquishes it.
   */
  int res;

  if (pushedback) {
    res = unputchar;
    pushedback = FALSE;
  } else if ( ( parsestring == NULL) ||
              ( parsestring[sindex] == '\0' ) ) {
    res =  '\n';
  } else {
    res = parsestring[sindex++];
  }
  return res;
}

void putbackonstring(int c) {
  /* put back a character we didn't want after all */
  if ( pushedback ) {
    fprintf(stderr,
            "%s: Pushed back too many characters.\n",
            "putbackonstring");
    exit(-1);
  } else {
    pushedback=TRUE;
    unputchar=c;
  }
}

#endif

void set_scanstring(char *str) {
  /* set up flex to scan from a particular string */

  #ifdef SCANSTRING_DEFINED
    yy_scan_string(str);			/* use flex's interface */
  #else
    parsestring = str;
    atend = FALSE;
    sindex=0;
    pushedback=FALSE;
  #endif
}

int yyvstop[] = {
0,

7,
0, 

2,
0, 

3,
0, 

7,
0, 

1,
7,
0, 

4,
0, 

5,
0, 

7,
0, 

6,
0, 

7,
0, 

7,
0, 

1,
0, 

1,
7,
0, 
0};
# define YYTYPE unsigned char
struct yywork { YYTYPE verify, advance; } yycrank[] = {
0,0,	0,0,	1,3,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	3,3,	1,4,	1,5,	
6,6,	4,4,	7,7,	0,0,	
0,0,	3,0,	3,0,	0,0,	
6,6,	6,0,	7,14,	7,0,	
14,14,	10,0,	10,0,	13,0,	
0,0,	0,0,	0,0,	0,0,	
14,14,	14,0,	0,0,	1,6,	
4,4,	0,0,	1,7,	2,7,	
3,0,	0,0,	3,0,	0,0,	
0,0,	6,12,	0,0,	7,14,	
10,0,	0,0,	10,3,	13,6,	
0,0,	0,0,	0,0,	0,0,	
0,0,	14,14,	15,7,	0,0,	
0,0,	1,8,	2,8,	1,9,	
2,9,	0,0,	15,14,	15,0,	
3,0,	0,0,	3,0,	6,0,	
0,0,	6,0,	0,0,	0,0,	
10,0,	13,0,	10,0,	13,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	15,7,	
0,0,	1,10,	2,10,	0,0,	
0,0,	0,0,	0,0,	0,0,	
3,10,	0,0,	0,0,	6,13,	
0,0,	7,15,	0,0,	0,0,	
10,10,	13,13,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	15,14,	0,0,	15,14,	
0,0,	0,0,	0,0,	0,0,	
0,0,	1,11,	2,11,	0,0,	
0,0,	0,0,	0,0,	0,0,	
3,0,	0,0,	0,0,	6,0,	
0,0,	0,0,	0,0,	0,0,	
10,0,	13,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	15,15,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	15,14,	0,0,	0,0,	
0,0};
struct yysvf yysvec[] = {
0,	0,	0,
yycrank+-1,	0,		0,	
yycrank+-2,	yysvec+1,	0,	
yycrank+-8,	0,		yyvstop+1,
yycrank+4,	0,		yyvstop+3,
yycrank+0,	0,		yyvstop+5,
yycrank+-11,	0,		yyvstop+7,
yycrank+-13,	0,		yyvstop+9,
yycrank+0,	0,		yyvstop+12,
yycrank+0,	0,		yyvstop+14,
yycrank+-16,	yysvec+3,	yyvstop+16,
yycrank+0,	0,		yyvstop+18,
yycrank+0,	0,		yyvstop+20,
yycrank+-17,	yysvec+6,	yyvstop+22,
yycrank+-23,	0,		yyvstop+24,
yycrank+-57,	0,		yyvstop+26,
0,	0,	0};
struct yywork *yytop = yycrank+181;
struct yysvf *yybgin = yysvec+1;
char yymatch[] = {
  0,   1,   1,   1,   1,   1,   1,   1, 
  1,   9,  10,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  9,   1,  34,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,  34,   1,  34,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,  34,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
0};
char yyextra[] = {
0,0,0,0,0,0,0,0,
0};
/*	Copyright (c) 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#pragma ident	"@(#)ncform	6.12	97/12/08 SMI"

int yylineno =1;
# define YYU(x) x
# define NLSTATE yyprevious=YYNEWLINE
struct yysvf *yylstate [YYLMAX], **yylsp, **yyolsp;
char yysbuf[YYLMAX];
char *yysptr = yysbuf;
int *yyfnd;
extern struct yysvf *yyestate;
int yyprevious = YYNEWLINE;
#if defined(__cplusplus) || defined(__STDC__)
int yylook(void)
#else
yylook()
#endif
{
	register struct yysvf *yystate, **lsp;
	register struct yywork *yyt;
	struct yysvf *yyz;
	int yych, yyfirst;
	struct yywork *yyr;
# ifdef LEXDEBUG
	int debug;
# endif
	char *yylastch;
	/* start off machines */
# ifdef LEXDEBUG
	debug = 0;
# endif
	yyfirst=1;
	if (!yymorfg)
		yylastch = yytext;
	else {
		yymorfg=0;
		yylastch = yytext+yyleng;
		}
	for(;;){
		lsp = yylstate;
		yyestate = yystate = yybgin;
		if (yyprevious==YYNEWLINE) yystate++;
		for (;;){
# ifdef LEXDEBUG
			if(debug)fprintf(yyout,"state %d\n",yystate-yysvec-1);
# endif
			yyt = yystate->yystoff;
			if(yyt == yycrank && !yyfirst){  /* may not be any transitions */
				yyz = yystate->yyother;
				if(yyz == 0)break;
				if(yyz->yystoff == yycrank)break;
				}
#ifndef __cplusplus
			*yylastch++ = yych = input();
#else
			*yylastch++ = yych = lex_input();
#endif
#ifdef YYISARRAY
			if(yylastch > &yytext[YYLMAX]) {
				fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
				exit(1);
			}
#else
			if (yylastch >= &yytext[ yytextsz ]) {
				int	x = yylastch - yytext;

				yytextsz += YYTEXTSZINC;
				if (yytext == yy_tbuf) {
				    yytext = (char *) malloc(yytextsz);
				    memcpy(yytext, yy_tbuf, sizeof (yy_tbuf));
				}
				else
				    yytext = (char *) realloc(yytext, yytextsz);
				if (!yytext) {
				    fprintf(yyout,
					"Cannot realloc yytext\n");
				    exit(1);
				}
				yylastch = yytext + x;
			}
#endif
			yyfirst=0;
		tryagain:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"char ");
				allprint(yych);
				putchar('\n');
				}
# endif
			yyr = yyt;
			if ( (uintptr_t)yyt > (uintptr_t)yycrank){
				yyt = yyr + yych;
				if (yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					if(lsp > &yylstate[YYLMAX]) {
						fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
						exit(1);
					}
					goto contin;
					}
				}
# ifdef YYOPTIM
			else if((uintptr_t)yyt < (uintptr_t)yycrank) {	/* r < yycrank */
				yyt = yyr = yycrank+(yycrank-yyt);
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"compressed state\n");
# endif
				yyt = yyt + yych;
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					if(lsp > &yylstate[YYLMAX]) {
						fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
						exit(1);
					}
					goto contin;
					}
				yyt = yyr + YYU(yymatch[yych]);
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"try fall back character ");
					allprint(YYU(yymatch[yych]));
					putchar('\n');
					}
# endif
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transition */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					if(lsp > &yylstate[YYLMAX]) {
						fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
						exit(1);
					}
					goto contin;
					}
				}
			if ((yystate = yystate->yyother) && (yyt= yystate->yystoff) != yycrank){
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"fall back to state %d\n",yystate-yysvec-1);
# endif
				goto tryagain;
				}
# endif
			else
				{unput(*--yylastch);break;}
		contin:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"state %d char ",yystate-yysvec-1);
				allprint(yych);
				putchar('\n');
				}
# endif
			;
			}
# ifdef LEXDEBUG
		if(debug){
			fprintf(yyout,"stopped at %d with ",*(lsp-1)-yysvec-1);
			allprint(yych);
			putchar('\n');
			}
# endif
		while (lsp-- > yylstate){
			*yylastch-- = 0;
			if (*lsp != 0 && (yyfnd= (*lsp)->yystops) && *yyfnd > 0){
				yyolsp = lsp;
				if(yyextra[*yyfnd]){		/* must backup */
					while(yyback((*lsp)->yystops,-*yyfnd) != 1 && lsp > yylstate){
						lsp--;
						unput(*yylastch--);
						}
					}
				yyprevious = YYU(*yylastch);
				yylsp = lsp;
				yyleng = yylastch-yytext+1;
				yytext[yyleng] = 0;
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"\nmatch ");
					sprint(yytext);
					fprintf(yyout," action %d\n",*yyfnd);
					}
# endif
				return(*yyfnd++);
				}
			unput(*yylastch);
			}
		if (yytext[0] == 0  /* && feof(yyin) */)
			{
			yysptr=yysbuf;
			return(0);
			}
#ifndef __cplusplus
		yyprevious = yytext[0] = input();
		if (yyprevious>0)
			output(yyprevious);
#else
		yyprevious = yytext[0] = lex_input();
		if (yyprevious>0)
			lex_output(yyprevious);
#endif
		yylastch=yytext;
# ifdef LEXDEBUG
		if(debug)putchar('\n');
# endif
		}
	}
#if defined(__cplusplus) || defined(__STDC__)
int yyback(int *p, int m)
#else
yyback(p, m)
	int *p;
#endif
{
	if (p==0) return(0);
	while (*p) {
		if (*p++ == m)
			return(1);
	}
	return(0);
}
	/* the following are only used in the lex library */
#if defined(__cplusplus) || defined(__STDC__)
int yyinput(void)
#else
yyinput()
#endif
{
#ifndef __cplusplus
	return(input());
#else
	return(lex_input());
#endif
	}
#if defined(__cplusplus) || defined(__STDC__)
void yyoutput(int c)
#else
yyoutput(c)
  int c; 
#endif
{
#ifndef __cplusplus
	output(c);
#else
	lex_output(c);
#endif
	}
#if defined(__cplusplus) || defined(__STDC__)
void yyunput(int c)
#else
yyunput(c)
   int c; 
#endif
{
	unput(c);
	}
