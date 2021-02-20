/* GEARATIO.C   M. W. Klotz   11/02, 8/03

Given a set of gears, find which to use for a given required ratio

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
/* #include <constant.h> */

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
char rdata (void);

/* vmark..------------ global variable declarations ----------------------- */
dbl desp;		/* desired pitch to cut (tpi) */
dbl ratio;		/* leadscrew pitch/desired pitch */
dbl rtest;		/* test ratio */
dbl pacc;		/* pitch cutting accuracy (%) */

FILE *fpt;			/* utility file pointer */
char dfile[]="GEARATIO.DAT";	/* input data file */
FILE *fp;
char ofile[]="GEARATIO.OUT";

#define ND 200			/* maximum number of data items */
int ng;				/* actual number of data items */
int gear[ND];			/* storage for gear data */
int npr;			/* number of gear pairs */
int maxpair;			/* maximum number of pairs to examine */

#define LMAX 10		/* maximum number of loops */
int index[LMAX];	/* indices for loops */
int llimit[LMAX];	/* loop lower limits */
int ulimit[LMAX];	/* loop upper limits */
int inc[LMAX];		/* loop increment */
int nl;			/* actual number of loops */
int gj[LMAX],gk[LMAX];	/* storage for gear numbers associated with loop index */

/* smark..----------- global structure declarations ----------------------- */

/* main..================================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
char c;
int i,j,k,l,flag,sum;
dbl error,r;

/* -------------------------- initializations ------------------------------ */
flag=0;
/* ------------------------------------------------------------------------- */
puts ("GEAR RATIO CALCULATIONS\n");

if ((fp=fopen(ofile,"wt")) == NULL)
	{printf ("FAILED TO OPEN OUTPUT FILE: %s\n",ofile); quit;}

if (rdata ()) goto theend;		/* read the data file */
printf ("Number of gears read = %d\n",ng);
npr=ng*(ng-1)/2;			/* number of change gear pairs */
printf ("Number of unique gear pairs = %d\n",npr);
for (l=0 ; l<LMAX ; l++) {llimit[l]=0; ulimit[l]=npr; inc[l]=1;}
nl=1;		/* initially, one gear pair loop */

vin ("Required ratio",&ratio,"lf",.5,"");
vin ("Allowable ratio accuracy",&pacc,"lf",.01,"%");
vin ("Maximum gear pairs to examine",&maxpair,"%d",1.,"");

fprintf (fp,"Required ratio = %g\n",ratio);
fprintf (fp,"Allowable ratio accuracy = %g %%\n",pacc);
puts ("\npatience...\n You can press any key to abort");

oncemore:
for (l=0 ; l<nl ; l++) index[l]=llimit[l];	/* initialize indices */
printf ("\nlooping on %d gear pairs\n",nl);
fprintf (fp,"\n %d GEAR PAIR(S)\n\n",nl);

repeat:			/* start of looping */
if (kbhit())		/* bailout */
{
puts ("\nRUN ABORTED BY USER"); fprintf (fp,"\nRUN ABORTED BY USER"); goto mexit;
}

/* construct gear indices corresponding to loop indices */
for (i=0 ; i<nl ; i++)
{
	for (j=0,sum=0 ; j<ng-1 ; j++,sum+=ng-j)
	{if (index[i] < (sum+ng-j-1)) {k=index[i]-sum+j+1; break;}}
	gj[i]=j; gk[i]=k;
}

/* ---- begin skipping identical indices code */
if (nl>1)
{
	for (i=1 ; i<nl ; i++)
	{
		for (j=i+1 ; j<nl ; j++)
		{
		if (index[i] == index[j]) goto increment;

		/* skip combinations that use same gear twice */
	
		if (gj[i] == gj[j] || gk[i] == gk[j]) goto increment;
		if (gj[i] == gk[j] || gk[i] == gj[j]) goto increment;
		}
	}
}
/* ---- end skipping identical indices code */

/* multiple loop processing here */

/* 
step through the gear loops
constructing the ratio to test from gears gj[i] and gk[i].
since the gears were ordered in ascending order when read,
r= gear[gj[i]]/gear[gk[i]] should always be <=1.
r can be 1 only if two identical gears are in the input data.
r=1 is useless so skip it.
*/

for (i=0,rtest=1. ; i<nl ; i++)
{
r=(dbl)gear[gj[i]]/(dbl)gear[gk[i]]; if (r == 1) goto increment;

/* use r to bring rtest closer to the desired ratio */
if (rtest > ratio) rtest*=r; else {rtest/=r; gk[i]*=-1;}  // gk[i] always > 0
}

error=100*(rtest-ratio)/ratio;			// error of ratio rtest
if (ABS(error) > pacc) goto increment;		// is it acceptable?

// output an acceptable solution

// print the change gear pairs, suitably formatted
for (i=0,c=' ' ; i<nl ; i++,c='-')
	{
	if (gk[i] > 0)
	fprintf (fp,"%c%d:%d",c,gear[gj[i]],gear[gk[i]]);
	else
	{gk[i]*=-1; fprintf (fp,"%c%d:%d",c,gear[gk[i]],gear[gj[i]]);}
	}
// print ratio and error
fprintf (fp,"  %.6lf  %.3E %%\n",rtest,error);
flag++;		// indicate that we found a solution
printf ("%d,",flag);

increment:			/* increment loop indices */
for (j=nl-1 ; j>=0 ; j--)
	{
	index[j]+=inc[j];
	if (inc[j] > 0) {if (index[j] < ulimit[j]) goto repeat;}
	if (inc[j] < 0) {if (index[j] > ulimit[j]) goto repeat;}
	index[j]=llimit[j];
	}
/* 
loop through the maximum number of change gear pairs specified
even if a solution is found with n pairs, the user may want to examine >n pairs
in an attempt to get better accuracy for the ratio
*/

if (nl < maxpair) {nl++; goto oncemore;}

if (!flag) {puts ("\nNO SOLUTION FOUND"); goto theend;}

/* post run processing */
mexit:
printf ("\n\nYour data is on: %s\n",ofile);
theend: fclose (fpt);
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* rdata..----------------------------------------------------------------- */

/* read the data file */

char rdata (void)

{
char temp[81],f;
int i;

if ((fpt=fopen(dfile,"rt")) == NULL)
	{printf ("FAILED TO OPEN DATA FILE: %s\n",dfile); beep; return 1;}

ng=0;			/* number of gear entries */
f=0;
while ((fgets(temp,sizeof(temp),fpt)) != NULL)
	{
	trim(temp);		/* uses library trim */
	if ((strstr(temp,"STARTOFDATA")) != NULL) {f=1; continue;}
	if (!f) continue;
	if ((strstr(temp,"ENDOFDATA")) != NULL) break;
	if (temp[0] != '\0' && temp[0] != ';')
		{
		{
		gear[ng]=(int)atof(strtok(temp,",\t;"));
		if (++ng == ND)
			{
			printf
			("More than maximum (%d) change gears in data file\n",ND);
			quit;
			}

		}

		}
	}
do		/* sort data */
{
for (i=0,f=0 ; i<ng-1 ; i++)
{
if (gear[i] > gear[i+1])
	{
	f=1;
	swap (&gear[i],&gear[i+1],sizeof(gear[i]));
	}
}
} while (f);

fclose (fpt);
return 0;
}
/* ------------------------------------------------------------------------ */
