/* 3WIRE.C   M. W. Klotz   3/01, 3/04, 6,7/05

Three wire thread measurement calculations.

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
char op,met,pd;
int i,j,k;
dbl ta,s,p,w,a,b,e,x,t,q,m,ed,z,tpi,md;
dbl mpi=25.4;

/* -------------------------- initializations ------------------------------ */
textscreen ();
/* ------------------------------------------------------------------------- */

puts ("3 WIRE THREAD MEASUREMENTS (60 degree threads)\n");


op=getkey("(M)etric or [I]mperial threads?");
if (op == 'm') met=1; else met=0;

ta=60.; a=0.5*ta;	// thread angle
if (met)
{vin ("Thread pitch",&tpi,"lf",1.,"mm"); p=tpi/mpi;}
else
{vin ("Thread pitch",&tpi,"lf",20.,"tpi"); p=1./tpi;}


puts ("\nMost common threads are single start.");
vin ("Number of starts",&s,"lf",1.,"");
t=0.5*p;		// width of thread in axial plane at diameter e
w=0.5*p/COSD(a);	// wire diameter

printf ("\nBest wire diameter to use = %.4lf mm = %.4lf in\n",w*mpi,w);

if (met)
{vin ("Wire diameter used",&w,"%.4lf",w*mpi,"mm"); w/=mpi;}
else
vin ("Wire diameter used",&w,"%.4lf",w,"in");


z=(3./16.)*p/TAND(a);  // distance of pitch line below p/8 flat
print;
if (!met)
{
printf ("For American National Thread form, subtract %.4lf in from\n",2.*z);
puts ("major diameter (assumes p/8 flat on crest) to obtain pitch diameter.\n");
}

if (met)
{vin ("Major diameter of thread",&md,"lf",6.0,"mm"); md/=mpi;}
else
vin ("Major diameter of thread",&md,"lf",0.25,"in");

puts ("\npd = pitch diameter, mow = measurement over wires");
op=getkey("Calculate pd from mow (p) or mow from pd [m]?");
if (op == 'p') pd=1; else pd=0;

ed=md-2.*z;		// pitch diameter

e=ed;
b=p*s/(PI*e);		//tangent of lead angle at pitch diameter
q=TAND(a);
x=-t/q+w*(1.+1./SIND(a)+0.5*b*b*COSD(a)/q);
/* m=e+x; */

if (!pd)
{
if (met)
{vin ("Pitch diameter of thread",&e,"%.4lf",ed*mpi,"mm"); e/=mpi;}
else
vin ("Pitch diameter of thread",&e,"%.4lf",ed,"in");
}
else
{
if (met)
{vin ("Measurement over wires",&m,"%.4lf",(e+x)*mpi,"mm"); m/=mpi;}
else
vin ("Measurement over wires",&m,"%.4lf",e+x,"in");
}

b=p*s/(PI*e);		//tangent of lead angle at pitch diameter
q=TAND(a);
x=-t/q+w*(1.+1./SIND(a)+0.5*b*b*COSD(a)/q);
/* m=e+x; */

if (!pd)
printf ("\nMeasurement over wires = %.4lf mm = %.4lf in\n",(e+x)*mpi,e+x);
else
printf ("\nPitch diameter = %.4lf mm = %.4lf in\n",(m-x)*mpi,m-x);

/* post run processing */
mexit:
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* ------------------------------------------------------------------------ */
