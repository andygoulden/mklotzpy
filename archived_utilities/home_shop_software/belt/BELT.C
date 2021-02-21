/* BELT.C   M. W. Klotz   3/00

Calculate belt length for arbitrary arrangement of pulleys

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
char rdata (void);

/* gmark..-------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* vmark..------------ global variable declarations ----------------------- */

FILE *fpt;			/* utility file pointer */
char dfile[]="BELT.DAT";	/* input data file */
#define ND 200			/* maximum number of data items */
int nd;				/* actual number of data items */
dbl x[ND];
dbl y[ND];
dbl diam[ND];
int io[ND];

dbl spn[ND];
dbl agl[ND];
dbl wrp[ND];
dbl wang[ND];

/* smark..----------- global structure declarations ----------------------- */

/* main..================================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
char iwn;
int i,j,k;
dbl dx,dy,csq,ctc,spz,tln,arc;

/* -------------------------- initializations ------------------------------ */
textscreen ();
/* ------------------------------------------------------------------------- */

printf ("BELT LENGTH CALCULATION\n");

rdata();		// read data file
printf ("Number of pulleys for which data was read = %d\n",nd);

/* calculate belt spans and angles */

for (i=0,j=0 ; i<nd ; i++)
{
j=i+1; if (j == nd) j=0;
dx=x[j]-x[i]; dy=y[j]-y[i]; csq=dx*dx+dy*dy; ctc=sqrt(csq);
spz=0.5*(diam[i]+diam[j]);
if (spz > ctc || ctc == 0)
	{printf ("pulleys %d and %d overlap!\n",i,j); goto mexit;}
if (io[i] == io[j]) spz=0.5*(diam[i]-diam[j]);
spn[i]=sqrt(csq-spz*spz);
agl[i]=ATAN2(dy,dx)+io[i]*ASN(spz/ctc);
while (agl[i] > TWOPI) agl[i]-=TWOPI;
while (agl[i] < 0) agl[i]+=TWOPI;
}

/* calculate wrap lengths and total length */
iwn=0; tln=0;

for (j=0 ; j<nd ; j++)
{
i=j-1; if (j == 0) i=nd-1;
arc=io[j]*(agl[j]-agl[i]);
if (arc < 0) arc+=TWOPI;
if (arc > PI)
	{
	if (spn[j] > .5*diam[j]*tan(.5*(TWOPI-arc))) {arc-=TWOPI; iwn=1;}
	}
wang[j]=arc;
wrp[j]=arc*0.5*diam[j];
tln+=wrp[j]+spn[j];
}

puts ("\ndata displayed for each pulley is:");
puts ("x, y, diam, flag, wrap angle(deg), wrap length, span to next pulley");
for (i=0 ; i<nd ; i++)
{
printf ("\npulley number %d\n",i+1);
printf ("%.3lf, %.3lf, %.3lf, %d,",x[i],y[i],diam[i],io[i]);
printf (" %.1lf, %.3lf, %.3lf\n",DPR*wang[i],wrp[i],spn[i]);
if (!((i+1) % 5)) pausem;
}
if (iwn) puts ("\nNegative wrap length means belt may go slack!");
printf ("\nTotal belt length = %.3lf\n",tln);

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

if ((fpt=fopen(dfile,"rt")) == NULL)
	{printf ("FAILED TO OPEN DATA FILE: %s\n",dfile); beep; return 1;}

nd=0;			/* number of data entries */
f=0;
while ((fgets(temp,sizeof(temp),fpt)) != NULL)
	{
	trim(temp);		/* uses library trim */
	if ((strstr(temp,"STARTOFDATA")) != NULL) {f=1; continue;}
	if (!f) continue;
	if ((strstr(temp,"ENDOFDATA")) != NULL) break;
	if (temp[0] != '\0' && temp[0] != ';')
		{
		/* decode and store data here */
		x[nd]=(dbl)atof(strtok(temp,",\t;"));
		y[nd]=(dbl)atof(strtok(NULL,",\t;"));
		diam[nd]=(dbl)atof(strtok(NULL,",\t;"));
		io[nd]=(int)atof(strtok(NULL,",\t;"));
		if (++nd == ND)
			{
			printf
			("More than maximum (%d) data items in data file\n",ND);
			quit;
			}
		}
	}

fclose (fpt);
return 0;
}
/* ------------------------------------------------------------------------ */
