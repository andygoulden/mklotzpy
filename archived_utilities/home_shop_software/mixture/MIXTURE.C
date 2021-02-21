/* MIXTURE.C   M. W. Klotz   3/99, 7/02, 4/05, 6/05

Mixture and dilution calculator

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

/* smark..----------- global structure declarations ----------------------- */

/* main..================================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i,j,k;
dbl ca=0,cb=0,cm=0,va=0,vb=0,vm=0;
dbl pa,pb,pm;
dbl qa,qb,qm;

/* -------------------------- initializations ------------------------------ */
textscreen ();
/* ------------------------------------------------------------------------- */

/* 

Fundamental equations used are:

pm = (va*pa + vb*pb) / vm
vm = va + vb

*/

puts ("MIXTURE AND DILUTION CALCULATOR\n");

puts ("Concentrations of both solutions, Ca and Cb, must BOTH be specified.");
puts
("If one solution is a dilutant (e.g., pure water), enter its concentration");
puts ("as zero.\n");

vin ("Ca = concentration of solution a",&ca,"lf",10.,"%");
vin ("Cb = concentration of solution b",&cb,"lf",0.,"%");

puts ("\nInput whatever data you know; press return if not known.");
puts ("You must enter two data items to obtain a solution.\n");

k=0;
vin ("amount of solution a",&va,"lf",0.,""); if (va) k++;
vin ("amount of solution b",&vb,"lf",0.,""); if (vb) k++;
if (k == 2) goto doit;
vin ("amount of mixture",&vm,"lf",0.,""); if (vm) k++;
if (k == 2) goto doit;
vin ("concentration of mixture",&cm,"lf",0.,"%"); if (cm) k++;
if (k == 2) goto doit;

beep; puts ("\nINSUFFICIENT DATA FOR SOLUTION"); goto mexit;

doit:;
pa=.01*ca; pb=.01*cb; pm=.01*cm;

if (va && vb)
{
vm=va+vb;
pm=(va*pa+vb*pb)/vm;
goto disp;
}

if (va && cm)
{
vb=va*(pa-pm)/(pm-pb);
vm=va+vb;
goto disp;
}

if (va && vm)
{
vb=vm-va;
pm=(va*pa+vb*pb)/vm;
goto disp;
}

if (vb && cm)
{
va=vb*(pm-pb)/(pa-pm);
vm=va+vb;
goto disp;
}

if (vb && vm)
{
va=vm-vb;
pm=(va*pa+vb*pb)/vm;
goto disp;
}

if (vm && cm)
{
va=vm*(pm-pb)/(pa-pb);
vb=vm-va;
goto disp;
}

disp:
cm=100.*pm;
printf ("\namount of solution a = %.5g\n",va);
printf ("concentration of solution a = %.5g %%\n",ca);
printf ("amount of solution b = %.5g\n",vb);
printf ("concentration of solution b = %.5g %%\n",cb);
printf ("amount of mixture = %.5g\n",vm);
printf ("concentration of mixture = %.5g %%\n",cm);

/* post run processing */
mexit:
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* ------------------------------------------------------------------------ */
