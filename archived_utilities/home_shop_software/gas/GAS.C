/* GAS.C   M. W. Klotz   3/99

Perfect gas law calculator

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
dbl pin (void);
dbl vlin (void);
dbl nin (void);
dbl tin (void);

/* gmark..-------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* vmark..------------ global variable declarations ----------------------- */
dbl R=8.20545E-2;	/* gas constant liter-atm/(degK-mole) */
dbl A=6.02308E23;	/* Avogadro's number (molecules in a mole of gas) */
dbl c0=273.16;		/* convert Celsius to Kelvin */
dbl psipa=14.6963;	/* psi per atmosphere */
dbl ppa=101.327;	/* kilopascal per atmosphere */
dbl cftpl=0.0353147;	/* cubic feet per liter */
dbl cmpl=0.001;		/* cubic meter per liter */
dbl p,v,n,t,tc,tf;	/* gas law parameters */
char gp[80];		/* general purpose buffer */
char *pt;		/* general purpose pointer */

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

puts ("PERFECT GAS LAW (PV = nRT) CALCULATOR\n");
puts ("P = pressure; V = volume; T = temperature");
puts ("n = number of moles of (assumed perfect) gas");
puts ("R = gas constant = 8.20545E-2 liter-atm/(degK-mole)");
puts ("Avogadro's number = 6.02308E23 = number of molecules in a mole of gas");
puts ("mole of gas = weight (gm) equal to molecular weight (e.g. 32 for O2)");
puts ("A mole of ordinary air weighs ~29 gm.");
puts ("standard conditions = 0 degC (273.16 degK) and 1 atmosphere.");
puts ("One mole of gas occupies 22.4140 liters at standard conditions.");

puts ("\nInput whatever data you know; press return if not known.");
puts ("You must enter three data items to obtain a solution.\n");
k=0;

p=pin(); if (p) k++;
v=vlin(); if (v) k++;
n=nin(); if (n) k++;
if (k == 3) goto doit;
t=tin(); if (t) k++;
if (k == 3) goto doit;

beep; puts ("\nINSUFFICIENT DATA FOR SOLUTION"); goto mexit;

doit:;
if (!p) {p=n*R*t/v; goto disp;}
if (!v) {v=n*R*t/p; goto disp;}
if (!n) {n=(p*v)/(R*t); goto disp;}
if (!t) {t=(p*v)/(n*R); goto disp;}

disp:;
tc=t-c0; tf=9.*tc/5.+32.; print;
printf ("P = %12.4lf atm = %12.4lf psi  = %12.4lf kpas\n",p,p*psipa,p*ppa*0.001);
printf ("V = %12.4lf l   = %12.4lf ft^3 = %12.4lf m^3 \n",v,v*cftpl,v*cmpl);
printf ("T = %12.4lf øK  = %12.4lf øF   = %12.4lf øC  \n",t,tf,tc);
printf ("n = %12.4lf mole\n",n);

/* post run processing */
mexit:
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* pin..------------------------------------------------------------------- */

/* pressure input */

dbl pin (void)

{
printf ("%s","Pressure [atm]osphere, (psi), (kpa)scal ? ");
gets (gp); strlwr (gp); if (strlen(gp) == 0) return 0.;
if (pt=strstr(gp,"psi")) return strtod(gp,&pt)/psipa;
if (pt=strstr(gp,"pas")) return strtod(gp,&pt)/ppa;
pt=&gp[0]+strlen(gp); return strtod (gp,&pt);
}
/* vlin..------------------------------------------------------------------ */

/* volume input */

dbl vlin (void)

{
printf ("%s","Volume [l]iter, (cft), (cm)eter ? ");
gets (gp); strlwr (gp); if (strlen(gp) == 0) return 0.;
if (pt=strstr(gp,"cft")) return strtod(gp,&pt)/cftpl;
if (pt=strstr(gp,"cm")) return strtod(gp,&pt)/cmpl;
pt=&gp[0]+strlen(gp); return strtod (gp,&pt);
}
/* nin..------------------------------------------------------------------- */

/* moles input */

dbl nin (void)

{
printf ("%s","Moles ? ");
gets (gp); strlwr (gp); if (strlen(gp) == 0) return 0.;
pt=&gp[0]+strlen(gp); return strtod (gp,&pt);
}
/* tin..------------------------------------------------------------------- */

/* temperature input */

dbl tin (void)

{
printf ("%s","Temperature [K], (C), (F) ? ");
gets (gp); strlwr (gp); if (strlen(gp) == 0) return 0.;
if (pt=strstr(gp,"c")) {t=strtod(gp,&pt); return t+c0;}
if (pt=strstr(gp,"f")) {t=strtod(gp,&pt); return 5.*(t-32)/9.+c0;}
pt=&gp[0]+strlen(gp); return strtod (gp,&pt);
}
/* ------------------------------------------------------------------------ */
