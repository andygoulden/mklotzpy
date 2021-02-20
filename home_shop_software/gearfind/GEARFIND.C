/* GEARFIND.C   M. W. Klotz  8/03

Given a desired ratio and tolerance thereof, find sets of gears to produce that
ratio within the tolerance.

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

/* gmark..-------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* vmark..------------ global variable declarations ----------------------- */
FILE *fp;
char ofile[]="GEARFIND.OUT";

#define LMAX 10		/* maximum number of loops */
int index[LMAX];	/* indices for loops */
int llimit[LMAX];	/* loop lower limits */
int ulimit[LMAX];	/* loop upper limits */
int inc[LMAX];		/* loop increment */
int nl;			/* actual number of loops */

dbl dr;			// desired ratio
dbl aerr;		// allowable ratio error
dbl ar;			// actual ratio
dbl err;		// actual ratio error
dbl best;		// best solution found
int ii,nsol;

int npmax;		// maximum number of gear pairs
int gmin;		// minimum number of gear teeth
int gmax;		// maximum number of gear teeth

/* smark..----------- global structure declarations ----------------------- */

/* main..================================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i,j,k,l;

/* -------------------------- initializations ------------------------------ */
textscreen ();
/* ------------------------------------------------------------------------- */

puts ("FIND GEARS TO PRODUCE A GIVEN RATIO\n");

if ((fp=fopen(ofile,"wt")) == NULL)
	{printf ("FAILED TO OPEN OUTPUT FILE: %s\n",ofile); quit;}

vin ("Desired ratio",&dr,"lf",1.9945,"");
vin ("Allowable ratio error",&aerr,"lf",0.01,"%");
vin ("Maximum number of gear pairs (< 5)",&npmax,"d",2.,"");
vin ("Minimum number of gear teeth",&gmin,"d",16.,"");
vin ("Maximum number of gear teeth",&gmax,"d",40.,"");

fprintf (fp,"Desired ratio = %g\n\n",dr);

nsol=0; best=1.e9;
nl=2*npmax;		// number of loops
if (nl > LMAX) {beep; goto mexit;}

/* set loop limits */
for (l=0 ; l<nl ; l++) {llimit[l]=gmin; ulimit[l]=gmax+1; inc[l]=1;}


for (l=0 ; l<nl ; l++) index[l]=llimit[l];	/* initialize indices */

puts ("patience ...");
puts ("\n(Press any key to abort)\n");

repeat:			/* start of looping */

/* multiple loop processing here */
if (kbhit())
{
puts ("\nRUN ABORTED BY USER");
fprintf (fp,"\nRUN ABORTED BY USER");
goto mexit;
}

ar=1.;
for (ii=0 ; ii<nl ; ii+=2)
{
ar*=(dbl)index[ii]/(dbl)index[ii+1];
}
err=100.*(ar-dr)/dr;		// error in %
if (ABS(err) <= aerr)
{
printf ("%d,",++nsol);
for (ii=0 ; ii<nl ; ii+=2) fprintf (fp,"%d:%d-",index[ii],index[ii+1]);
fprintf (fp,"  %g (%g %%)",ar,err);
if (ABS(err) < best) {fprintf (fp, " **\n"); best=ABS(err);}
else fprintf (fp,"\n");
}

increment:			/* increment loop indices */
for (j=nl-1 ; j>=0 ; j--)
{
index[j]+=inc[j];
if (inc[j] > 0) {if (index[j] < ulimit[j]) goto repeat;}
if (inc[j] < 0) {if (index[j] > ulimit[j]) goto repeat;}
index[j]=llimit[j];
}

if (nsol == 0) puts ("\nNO SOLUTION FOUND");

/* post run processing */
mexit:
if (nsol != 0) printf ("\n\nYour data is on: %s\n",ofile);
fcloseall;
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* ------------------------------------------------------------------------ */
