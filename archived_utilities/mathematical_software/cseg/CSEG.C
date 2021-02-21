/* CSEG.C   M. W. Klotz   11/98

Circular segment computations

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
dbl height,radius,chord,angle,arc,area,z;
dbl h,t1,t2,dt,best,tbest,hbest;

/* -------------------------- initializations ------------------------------ */
textscreen ();
/* ------------------------------------------------------------------------- */

puts ("CIRCULAR SEGMENT CALCULATIONS\n");

puts ("Input whatever data you know; press return if not known.");
puts ("You must enter two data items to obtain a solution.\n");
k=0;
vin ("Radius of segment",&radius,"lf",0.,""); if (radius) k++;
vin ("Segment included angle",&angle,"lf",0.,"deg"); if (angle) k++;
if (k == 2) goto doit;
vin ("Chord of segment",&chord,"lf",0.,""); if (chord) k++;
if (k == 2) goto doit;
vin ("Height of segment (sagitta)",&height,"lf",0.,""); if (height) k++;
if (k == 2) goto doit;
vin ("Arc length",&arc,"lf",0.,""); if (arc) k++;
if (k == 2) goto doit;

beep; puts ("\nINSUFFICIENT DATA FOR SOLUTION"); goto mexit;


doit:;

if (radius && angle)
{
z=radius*COSD(0.5*angle);
height=radius-z;
chord=2.*radius*SIND(0.5*angle);
arc=radius*angle*RPD;
goto disp;
}

if (radius && chord)
{
angle=2.*ASND(0.5*chord/radius);
z=radius*COSD(0.5*angle);
height=radius-z;
arc=radius*angle*RPD;
goto disp;
}

if (radius && height)
{
z=radius-height;
angle=2.*ACSD(z/radius);
chord=2.*radius*SIND(0.5*angle);
arc=radius*angle*RPD;
goto disp;
}

if (radius && arc)
{
angle=DPR*arc/radius;
z=radius*COSD(0.5*angle);
height=radius-z;
chord=2.*radius*SIND(0.5*angle);
goto disp;
}

if (angle && chord)
{
radius=0.5*chord/SIND(0.5*angle);
z=radius*COSD(0.5*angle);
height=radius-z;
arc=radius*angle*RPD;
goto disp;
}

if (angle && height)
{
radius=height/(1.-COSD(0.5*angle));
z=radius-height;
chord=2.*radius*SIND(0.5*angle);
arc=radius*angle*RPD;
goto disp;
}

if (angle && arc)
{
radius=DPR*arc/angle;
z=radius*COSD(0.5*angle);
height=radius-z;
chord=2.*radius*SIND(0.5*angle);
goto disp;
}

if (chord && height)
{
radius=(4.*height*height+chord*chord)/(8.*height);
z=radius-height;
angle=2.*ACSD(z/radius);
arc=radius*angle*RPD;
goto disp;
}

if (chord && arc)
{
h=0; t1=1.; t2=180.; dt=1.; hbest=1.e6; k=0;
while (ABS(h-chord) > 1.0e-6 && k < 6)
	{
	for (angle=t1 ; angle<=t2 ; angle+=dt)
		{
		radius=DPR*arc/angle;
		h=2.*radius*SIND(0.5*angle);
		if (ABS(h-chord) < hbest) {hbest=ABS(h-chord); tbest=angle;}
		}
	t1=tbest-dt; t2=tbest+dt; dt*=0.1; k++;
}
angle=tbest;
radius=DPR*arc/angle;
height=radius*(1.-COSD(0.5*angle));
goto disp;
}

if (height && arc)
{
h=0; t1=1.; t2=180.; dt=1.; hbest=1.e6; k=0;
while (ABS(h-height) > 1.0e-6 && k < 6)
	{
	for (angle=t1 ; angle<=t2 ; angle+=dt)
		{
		radius=DPR*arc/angle;
		h=radius*(1.-COSD(0.5*angle));
		if (ABS(h-height) < hbest) {hbest=ABS(h-height); tbest=angle;}
		}
	t1=tbest-dt; t2=tbest+dt; dt*=0.1; k++;
}
angle=tbest;
radius=DPR*arc/angle;
chord=2.*radius*SIND(0.5*angle);
goto disp;
}

disp:
printf ("\nRadius           = %.4lf\n",radius);
printf ("Angle            = %.4lf deg\n",angle);
printf ("Chord            = %.4lf\n",chord);
printf ("Height (sagitta) = %.4lf\n",height);
printf ("Arc Length       = %.4lf\n",arc);
area=PI*SQR(radius)*(angle/360.)-0.5*(radius-height)*chord;
printf ("Area             = %.4lf\n",area);

/* post run processing */
mexit:
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* ------------------------------------------------------------------------ */
