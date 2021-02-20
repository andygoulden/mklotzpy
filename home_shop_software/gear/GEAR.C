/* GEAR.C   M. W. Klotz   4/99, 2/05

Gear Calculations

Adapted from an article "Spur Gears and Pinions" by John A. Cooper
in "Machinist's Workshop", 4/99, Volume 12, Number 2, pg.8

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
FILE *fp;			/* utility file pointer */
char ofile[]="GEAR.OUT";	/* output file */
#define NG 2		/* number of gears */
dbl N[NG];		/* teeth on gear */
dbl P;			/* diametral pitch */
dbl phi;		/* pressure angle (deg) */
dbl gratio;		/* gear ratio */
dbl OD[NG];		/* outside diameter */
dbl A[NG];		/* addendum */
dbl D[NG];		/* dedendum */
dbl W[NG];		/* whole depth */
dbl CP[NG];		/* circular pitch */
dbl T[NG];		/* tooth thickness */
dbl PD[NG];		/* pitch diameter */
dbl RB[NG];		/* base circle radius */
dbl RP[NG];		/* tooth profile radius */
dbl CD;			/* center distance */

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

puts ("GEAR CALCULATIONS\n");

if ((fp=fopen(ofile,"wt")) == NULL)
	{printf ("FAILED TO OPEN OUTPUT FILE: %s\n",ofile); quit;}

vin ("Number of teeth on gear 1",&N[0],"lf",45.,"");
vin ("Number of teeth on gear 2",&N[1],"lf",20.,"");
vin ("Diametral Pitch (25.4/mod)",&P,"lf",20.,"");
vin ("Pressure Angle",&phi,"lf",20.,"deg");

gratio=MAX(N[0],N[1])/MIN(N[0],N[1]);
for (i=0 ; i<NG ; i++)
{
OD[i]=(N[i]+2.)/P;
A[i]=1.0/P;
D[i]=1.200/P;
W[i]=2.200/P;
CP[i]=PI/P;
T[i]=0.48*CP[i];
PD[i]=N[i]/P;
RB[i]=0.5*PD[i]*COSD(phi);
RP[i]=0.5*PD[i]*SIND(phi);
}
CD=0.5*(PD[0]+PD[1]);

fprintf (fp,"\nFOR BOTH GEARS:\n");
fprintf (fp,"Gear Ratio = %.6lf:1\n",gratio);
fprintf (fp,"Diametral Pitch = %.4lf\n",P);
fprintf (fp,"Pressure Angle = %.4lf deg\n",phi);
fprintf (fp,"Center Distance = %.4lf in = %.4lf mm\n",CD,CD*25.4);

for (i=0 ; i<NG ; i++)
{
fprintf (fp,"\nFOR GEAR %d:\n",i+1);
fprintf (fp,"Number of teeth = %.0lf\n",N[i]);
fprintf (fp,"Outside Diameter = %.4lf in = %.4lf mm\n",OD[i],OD[i]*25.4);
fprintf (fp,"Addendum = %.4lf in = %.4lf mm\n",A[i],A[i]*25.4);
fprintf (fp,"Dedendum = %.4lf in = %.4lf mm\n",D[i],D[i]*25.4);
fprintf (fp,"Whole Depth = %.4lf in = %.4lf mm\n",W[i],W[i]*25.4);
fprintf (fp,"Circular Pitch = %.4lf in = %.4lf mm\n",CP[i],CP[i]*25.4);
fprintf (fp,"Tooth Thickness = %.4lf in = %.4lf mm\n",T[i],T[i]*25.4);
fprintf (fp,"Pitch Diameter = %.4lf in = %.4lf mm\n",PD[i],PD[i]*25.4);
fprintf (fp,"Base Circle Radius = %.4lf in = %.4lf mm\n",RB[i],RB[i]*25.4);
fprintf (fp,"Tooth Profile Radius = %.4lf in = %.4lf mm\n",RP[i],RP[i]*25.4);
}

printf ("\nYour data is on: %s\n",ofile);

/* post run processing */
mexit:
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* ------------------------------------------------------------------------ */
