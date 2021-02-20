/* GEARF1.C   M. W. Klotz   9/04

Find a pair of gears needed to create an a priori known ratio

*/

/* head..-------------------- header files -------------------------------- */

/* Unneeded headers can be left in place - the header files consist mainly of
'defines' for the compiler and do not add to the size of the executable code.
*/

/* turns off assert macros when not debugging */
#define NDEBUG

#include <assert.h>
#include <conio.h>
#include <ctype.h>
#include <dos.h>
#include <float.h>
#include <graph.h>
#include <malloc.h> 
#include <math.h>
#include <search.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* def..----------------------- defines -------------------------------------*/

/* type synonyms */
typedef double dbl;
typedef float real;
typedef unsigned long ulong;
typedef unsigned int uint;
typedef unsigned char uchar;

/*
typ	format		pointer		size	range
0	c		char		1	
1	s		char		?
2	h(d,i)		char		1	-128 : 127
3	hu		uchar		1	0 : 255
4	d,i		int		2	-32768 : 32767
5	u		uint		2	0 : 65535
6	ld,li		long		4	-2,147,483,647 : 2,147,483,648
7	lu		ulong		4	0 : 4,294,967,295
8	e,E,f,F,g,G	float (real)	4	7 digit 10^+/-38
9	l(e,E,f,F,g,G)	double (dbl)	8	15 digit 10^+/-308
*/

/* macro declarations */
#define _in stdin			/* makes redirection easier */
#define _out stdout
#define pause getch()
#define pausem getkey("press a key\n")
#define beep tone(440,100L)
#define bpause beep; pause
#define bpausem beep; pausem
#define quit beep; fcloseall(); exit(1)
#define print printf("\n")
#define FALSE 0
#define TRUE !FALSE
#define cls _clearscreen(0)

/* global utility functions (can return only one value)
   note: no space between function name and parameter list
*/
#define SQR(a) ((a)*(a))
#define CUBE(a) ((a)*(a)*(a))
#define FRAC(a) ((a)-floor(a))		/* or use modf function */
	/* see also split() in C:\CCC\GOODIES.TXT */
#define MAX(a,b) ((a)>=(b) ? (a) : (b))
#define MIN(a,b) ((a)<=(b) ? (a) : (b))
#define ABS(a) ((a)>=0 ? (a) : -(a))
#define SGN(a) ((a)!=0 ? ((a)>0 ? 1 : -1) : 0)
#define ATOB(a,b) (pow((a),(b)))
#define LOG(a) (log(a))
#define LOG10(a) (log10(a))
#define SIND(a) sin((a)*RPD)
#define COSD(a) cos((a)*RPD)
#define TAND(a) tan((a)*RPD)
#define ASN(a) asin((ABS(a)<1) ? (a) : SGN(a))
#define ACS(a) acos((ABS(a)<1) ? (a) : SGN(a))
#define ASND(a) DPR*asin((ABS(a)<1) ? (a) : SGN(a))
#define ACSD(a) DPR*acos((ABS(a)<1) ? (a) : SGN(a))
#define ATAND(a) DPR*atan(a)
#define ATAN2(s,c) ((s!=0||c!=0) ? atan2((s),(c)) : 0)
#define ATAN2D(s,c) DPR*((s!=0||c!=0) ? atan2((s),(c)) : 0)
#define URAND(a,b) (((b)-(a))*(float)rand()/32767.+(a))
#define RSS2(a,b) sqrt((a)*(a)+(b)*(b))
#define RSS3(a,b,c) sqrt((a)*(a)+(b)*(b)+(c)*(c))
#define RND(a) floor ((a)+.5)

/* scan codes for keys */
/* #include <keycode.h> */

/* constant declarations - all 'defines' - no executable code */
#include <constant.h>

/* -------------------------- function prototypes ------------------------- */

/*
Minimize the number of these included since they contribute to exectuable
size.  Many of the more oft-used functions are now in MWK.LIB so they can
be called from source code without the need to include the include files.
*/

/*
global utility functions (now in MWK.LIB)
void delay (clock_t wait);
char getkey (char *prompt);
void ptext (char *text, int row, int col, int fgd, int bkg);
void textscreen (void);
void tone (int frequency, long duration);
void kbdset (uint kbtog);
*/
/* #include <util.h> */

/*
generalized io functions (now in MWK.LIB)
void vin (char *prompt, void *x, char *format, dbl def, char *udef);
void vpr (char *label, void *x, char *format);
char cf (char *format);
int places (void *x, char *format);
void apr (char *label, void *x, char *format, int n1, int n2);
*/
/* #include <io.h> */

/*
simplified string tools (now in MWK.LIB)
char *sex (char *d, char *s, int p, int n);
char *sdel (char *s, char c);
char *ssi (char *s, char *p, char *r, int n);
void sdp (char *s);
*/
/* #include <str.h> */

/*
string manipulation
char *trim (char *s);
char *left (char *d, char *s, int num);
char *right (char *d, char *s, int num);
char *mid (char *d, char *s, int start, int num);
void strdelc (char *s, char c);
void strreplace (char *s, char new, char old);
char *strinsert (char *s, char insert);
char *strsub (char *s, char *pat, char *rep);
int stoint (char *s);
*/
/* #include <strings.h> */

/* data input routines */
/* #include <input.h> */

/* printing data */
/* #include <output.h> */

/* vector utilities */
/* #include <vector.h> */

/* graphics routines */
/* #include <graphics.h> */

/* file management routines */
/* #include <file.h> */

/* pmark..  functions peculiar to this program */
void cfrac (int nc, long *ln, long *ld);
void ffrac (dbl fp, dbl eps);

/* gmark..-------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* vmark..------------ global variable declarations ----------------------- */
FILE *ffp;
char ofile[]="GEARF1.OUT";

#define NCF 100		/* maximum terms in continued fraction */
dbl x[NCF];		/* storage for continued fraction terms */
dbl f,eps,fint,fp,z,app;
long ln,ld,g;

dbl aerr;		// allowable error
dbl dr;			// desired ratio
dbl ijr;		// arbitrary ratio
dbl ar;			// actual ratio
dbl err;		// actual ratio error
dbl best;		// best solution found
int ii,nsol;

int gmin;		// minimum number of gear teeth
int gmax;		// maximum number of gear teeth

/* smark..----------- global structure declarations ----------------------- */

/* main..================================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i,j,k;

/* -------------------------- initializations ------------------------------ */
textscreen ();
/* ------------------------------------------------------------------------- */

puts ("FIND A PAIR OF GEARS TO PRODUCE A GIVEN RATIO\n");

if ((ffp=fopen(ofile,"wt")) == NULL)
	{printf ("FAILED TO OPEN OUTPUT FILE: %s\n",ofile); quit;}

vin ("Desired ratio",&dr,"lf",2.54,"");
fp=modf(dr,&fint);
if (fp == 0.)
{
puts ("INTEGER RATIOS NOT ALLOWED - TOO EASY TO DO BY HAND");
goto mexit;
}
vin ("Allowable ratio error",&aerr,"lf",0.5,"%");
vin ("Minimum number of gear teeth",&gmin,"d",15.,"");
vin ("Maximum number of gear teeth",&gmax,"d",100.,"");
print;


fprintf (ffp,"Desired ratio = %g\n\n",dr);

best=1.0e99; nsol=0;
for (i=gmin ; i<=gmax ; i++)
{
for (j=gmin ; j<=gmax ; j++)
{
if (j == i) continue;
if (kbhit())
{
puts ("\nRUN ABORTED BY USER");
fprintf (ffp,"\nRUN ABORTED BY USER");
goto mexit;
}
ijr=(dbl)i/(dbl)j;
ar=ijr;

err=100.*(ar-dr)/dr;
if (ABS(err) > aerr) continue;
printf ("%d,",++nsol);
fprintf (ffp,"%d:%d => %.6lf (%.6lf%%)",i,j,ar,err);
if (ABS(err) < best) {fprintf (ffp," **\n"); best=ABS(err);}
else fprintf (ffp,"\n");

}	// jend
}	// iend

if (nsol == 0) puts ("\nNO SOLUTION FOUND");


/* post run processing */
mexit:
if (nsol != 0) printf ("\n\nYour data is on: %s\n",ofile);
fcloseall;
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* cfrac..----------------------------------------------------------------- */

/* 
Evaluate continued fraction [x0, x1, x2, ...]
The numerator, ln, and denominator, ld, are returned.
*/

void cfrac (int nc, long *ln, long *ld)

{
int k;
dbl la,lb,lc,lt;

k=nc; lc=1; lb=0.; la=x[k];
while (k) {k--; lt=lc; lc=lt*la+lb; lb=lt; la=x[k];}
*ln=(long)lc; *ld=(long)(lc*la+lb);
}
/* ffrac..----------------------------------------------------------------- */

/* 
Generate the continued fraction
*/

void ffrac (dbl fp, dbl eps)

{
dbl z,error;
int nc;

z=fp; nc=-1; error=1.e6;
while (ABS(error) > eps)
{
z=1./z; if (++nc > NCF-1) break;
x[nc]=floor(z); z-=x[nc]; cfrac (nc,&ln,&ld);
error=((dbl)ln/(dbl)ld-fp)/fp;
}
}
/* ------------------------------------------------------------------------ */
