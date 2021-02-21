/* CALIBRAT.C   M. W. Klotz   7/01

Calibrate a linear scale from measured values.  Uses least-square fit to a
linear curve.

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
char rdata (void);
void lsq (void);
void wdata (void);

/* gmark..-------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* vmark..------------ global variable declarations ----------------------- */

FILE *fp;			/* utility file pointer */
char dfile[]="CALIBRAT.DAT";	/* input data file */
char ofile[]="CALIBRAT.OUT";	/* output data file */

#define ND 400			/* maximum number of data pairs */
int nd;				/* actual number of data pairs */
dbl x[ND];			/* storage for truth values */
dbl y[ND];			/* storage for measured values */

dbl a,b;			/* slope,intercept for lsq fit */
dbl sumx,sumx2,sumy,sumxy,det;	/* used in making lsq fit */
dbl tstart,tstop,tinc;

/* smark..----------- global structure declarations ----------------------- */

/* main..================================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
char op;
int i,j,k;

/* -------------------------- initializations ------------------------------ */
textscreen ();
/* ------------------------------------------------------------------------- */

puts ("CALIBRATE A LINEAR SCALE\n");


rdata ();		// read data file
printf ("%d data pairs read\n",nd);

if (nd<2) 
{beep; puts ("Too few data pairs for solution.  Try again."); quit;}

lsq ();			// generate least squares solution

puts ("\nCalibration Equations:\n");
puts ("x=truth value, y=measured value");
puts ("y = A*x + B  or x = (y-B)/A");
printf ("A = %lf\n",a);
printf ("B = %lf\n",b);

op=getkey("\nDo you want to construct a calibration table [Y]/N ? ");
if (op == 'n') goto mexit;

vin ("Table starting truth value",&tstart,"lf",x[0],"");
vin ("Table ending truth value",&tstop,"lf",x[nd-1],"");
agin:;
vin ("Table increment",&tinc,"lf",(tstop-tstart)/nd,"");

if (!((tstop-(tstart+tinc)) < (tstop-tstart)))
{
beep;
puts ("Bad increment.  Try again.");
goto agin;
}

wdata ();		// produce calibration table
printf ("\nYour data is on: %s\n",ofile);

/* post run processing */
mexit:
fcloseall();
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* rdata..----------------------------------------------------------------- */

/* read the data file */

char rdata (void)

{
char temp[81],f;
int i;

if ((fp=fopen(dfile,"rt")) == NULL)
	{printf ("FAILED TO OPEN DATA FILE: %s\n",dfile); quit;}

nd=0;			/* number of data pairs */
f=0;
while ((fgets(temp,sizeof(temp),fp)) != NULL)
	{
	trim(temp);		/* uses library trim */
	if ((strstr(temp,"STARTOFDATA")) != NULL) {f=1; continue;}
	if (!f) continue;
	if ((strstr(temp,"ENDOFDATA")) != NULL) break;
	if (temp[0] != '\0' && temp[0] != ';')
		{
		x[nd]=(dbl)atof(strtok(temp,",\t;"));
		y[nd]=(dbl)atof(strtok(NULL,",\t;"));
		if (++nd == ND)
			{
			printf
			("More than maximum (%d) data pairs in data file\n",ND);
			quit;
			}
		}
	}
do		/* sort data */
{
for (i=0,f=0 ; i<nd-1 ; i++)
{
if (x[i] > x[i+1])
	{
	f=1;
	swap (&x[i],&x[i+1],sizeof(x[i]));
	swap (&y[i],&y[i+1],sizeof(y[i]));
	}
}
} while (f);

fclose (fp);
return 0;
}
/* lsq..------------------------------------------------------------------- */
/* 
form the least square solution for the linear fit to the data
*/

void lsq (void)

{
int i;

sumx=0;
sumx2=0;
sumy=0;
sumxy=0;
for (i=0 ; i<nd ; i++)
{
sumx+=x[i];
sumx2+=x[i]*x[i];
sumy+=y[i];
sumxy+=x[i]*y[i];
}
det=sumx2*nd-sumx*sumx;		// matrix determinant
if (det == 0)
{
puts ("Determinat=0, impossible to proceed.");
quit;
}
a=(nd*sumxy-sumx*sumy)/det;
b=(sumx2*sumy-sumx*sumxy)/det;
return;
}
/* wdata..----------------------------------------------------------------- */

/* write the output file */

void wdata (void)

{
dbl t;

if ((fp=fopen(ofile,"wt")) == NULL)
	{printf ("FAILED TO OPEN OUTPUT FILE: %s\n",ofile); quit;}

fprintf (fp,"Calibration Table\n\n");
fprintf (fp,"First entry is measured, second is truth value\n\n");

for (t=tstart ; t<=tstop ; t+=tinc)
{
fprintf (fp,"%lf <=> %lf\n",t,(t-b)/a);
}

fprintf (fp,"\nFirst entry is truth, second is measured value\n\n");

for (t=tstart ; t<=tstop ; t+=tinc)
{
fprintf (fp,"%lf <=> %lf\n",t,a*t+b);
}

fclose (fp);
}
/* ------------------------------------------------------------------------ */
