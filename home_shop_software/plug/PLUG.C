/* PLUG.C   M. W. Klotz   3/02, 4/04

Use three plug gages to measure a hole too large for any of the plugs.
Written for Les Saunders (lsaunders@cox.net)

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
typedef unsigned short ushort;

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
#define beep printf ("\a")
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
#define DABS(a,b) (ABS((a)-(b)))
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
dbl soddy (dbl d1, dbl d2, dbl d3);

/* gmark..-------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* vmark..------------ global variable declarations ----------------------- */
dbl dmin;		/* smallest plug gage */
dbl dmax;		/* largest plug gage */
dbl dd;			/* plug gage increment */
dbl hole;		/* hole diameter to be measured */
dbl toler;		/* tolerance of match to hole diameter */
dbl dif;		/* allowable difference in pin sizes */

FILE *fp;
char ofile[]="PLUG.OUT";

/* smark..----------- global structure declarations ----------------------- */

/* main..================================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i,j,k;
char ch;
dbl d1,d2,d3,s,e,hmax;
dbl dsmin,dsmax,davg,c30,best;

/* -------------------------- initializations ------------------------------ */
textscreen ();
/* ------------------------------------------------------------------------- */

if ((fp=fopen(ofile,"wt")) == NULL)
	{printf ("FAILED TO OPEN OUTPUT FILE: %s\n",ofile); quit;}

vin ("Smallest plug gage available",&dmin,"lf",0.0608,"in");
vin ("Largest plug gage available",&dmax,"lf",0.9998,"in");
vin ("Plug-to-plug increment",&dd,"lf",0.001,"in");
hmax=soddy(dmax,dmax-dd,dmax-2*dd);
agin:
printf ("Largest hole that can be measured = %.4lf in\n",hmax);
vin ("Nominal hole diameter to measure",&hole,"lf",0.5366,"in");
if (hole > hmax)
{beep; puts ("TOO BIG, TRY AGAIN\n"); goto agin;}
vin ("Allowable tolerance in matching hole diameter",&toler,"lf",0.00001,"in");
c30=COSD(30.);
davg=hole*c30/(1+c30);
printf ("Single pin size = %.4lf in\n",davg);
vin ("Allowable difference in pin sizes",&dif,"lf",0.05,"in");
puts ("Have patience ... PRESS SPACEBAR IF YOU WANT TO BAIL OUT\n");
print;

fprintf (fp,"Hole size = %.6lf in\n\n",hole);

dsmin=dmin; dsmax=dmax; best=1;
for (d1=dmin ; d1<=dmax ; d1+=dd)
{if (d1 > davg-dif) {dsmin=d1-dd; break;}}
for (d1=dsmin ; d1<=dmax ; d1+=dd)
{if (d1 > davg+dif) {dsmax=d1; break;}}

for (d1=dsmin ; d1<=dsmax ; d1+=dd)
{
for (d2=d1+dd ; d2<=dsmax ; d2+=dd)
{
for (d3=d2+dd ; d3<=dsmax ; d3+=dd)
{
if (kbhit())		/* bailout */
	{
	puts ("\nRUN ABORTED BY USER");
	fprintf (fp,"\nRUN ABORTED BY USER\n");
	goto mexit;
	}

s=soddy(d1,d2,d3);
e=s-hole;
if (ABS(e) <= toler)
{
if (ABS(e) < best) {best=ABS(e); ch='*';} else ch=' ';
printf ("%.4lf, %.4lf, %.4lf => %.7lf (error = %+.7lf)  %c\n",d1,d2,d3,s,e,ch);
fprintf (fp,"%.4lf, %.4lf, %.4lf => %.7lf (error = %+.7lf) %c\n",d1,d2,d3,s,e,ch);
}

}
}
}


/* post run processing */
mexit:
fclose (fp);
printf ("\nYour data is on: %s\n",ofile);
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* soddy..----------------------------------------------------------------- */

/* 
compute diameter of outer Soddy circle given diameters of three inner circles
(for diameter of inner Soddy circle, change sign on radical)
*/

dbl soddy (dbl d1, dbl d2, dbl d3)

{
return ABS(d1*d2*d3/(d2*d3+d1*(d2+d3)-2.*sqrt(d1*d2*d3*(d1+d2+d3))));
}
/* ------------------------------------------------------------------------ */
