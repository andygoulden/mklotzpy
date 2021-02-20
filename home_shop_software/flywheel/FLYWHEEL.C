/* FLYWHEEL.C   M. W. Klotz   4/99, 5/04

Calculations for tapered spoke flywheels

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
void psol (void);

/* gmark..-------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* vmark..------------ global variable declarations ----------------------- */
FILE *fp;			/* utility file pointer */
char ofile[]="FLYWHEEL.OUT";	/* output file */
int nspoke;		/* number of spokes */
dbl r1,r2;		/* radii where holes are located */
dbl s1,s2,d1,d2;	/* radii, diameter of holes */
dbl offset;		/* spoke to outer hole center distance */
dbl theta1,theta2,w;
dbl phi,phil,phis,phif,dphi,qmin,p,q;
dbl x,sep,spoke1,spoke2;

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

puts ("TAPERED (OR STRAIGHT) SPOKE FLYWHEEL CALCULATIONS\n");

if ((fp=fopen(ofile,"wt")) == NULL)
	{printf ("FAILED TO OPEN OUTPUT FILE: %s\n",ofile); quit;}

vin ("Number of spokes",&nspoke,"d",6.,"");
vin ("Radius on which inner holes are located",&r1,"lf",0.440,"in");
vin ("Radius on which outer holes are located",&r2,"lf",1.367,"in");
vin ("Diameter of inner holes",&d1,"lf",0.1875,"in");
vin ("Diameter of outer holes",&d2,"lf",0.1875,"in");
agin:
theta2=0; offset=0;
puts ("YOU MUST SPECIFY EITHER THE OUTER HOLE OFFSET OR ITS OFFSET ANGLE.");
puts ("Note: Set offset = 0 and theta2 = 7 deg for demo.");
vin ("Offset from spoke CL to outer hole center",&offset,"lf",0.,"in");
if (offset == 0.)
{
vin ("Angle from spoke CL to outer hole center",&theta2,"lf",0.,"deg");
if (theta2 == 0.)
	{
	beep; puts ("Failed to specify one or the other.  Try again.");
	goto agin;
	}
}

s1=0.5*d1; s2=0.5*d2;		/* radii of holes */
theta1=180./(dbl)nspoke;
if (theta2 == 0.) theta2=ASND(offset/r2); else offset=r2*SIND(theta2);
w=r2-r1+s1+s2;

phil=0; phis=0; phif=20; dphi=1;
iter: qmin=1.E9;
	for (p=phis ; p<=phif ; p+=dphi)
	{
	q=ABS(r1*SIND(theta1+p)-s1-(r2*SIND(theta2+p)-s2));
	if (q < qmin) {qmin=q; phi=p;}
	}
q=r1*SIND(theta1+phi)-s1-(r2*SIND(theta2+phi)-s2);
if (ABS(phi-phil) > 0.01)
	{phil=phi; phis=phi-dphi; phif=phi+dphi; dphi/=10.; goto iter;}

x=r1*SIND(theta1+phi)-s1;
sep=2*(theta1-theta2);
spoke1=2*(r1*SIND(theta1)-s1);
spoke2=2*(r2*SIND(theta2)-s2);
psol ();

fprintf (fp,"\n***** NEAREST INTEGRAL ANGLE SOLUTION *****\n\n");
theta2=RND(theta2); phi=RND(phi);
r1=r2*(SIND(theta2+phi)+(s1-s2)/r2)/SIND(theta1+phi);
w=r2-r1+s1+s2;
x=r1*SIND(theta1+phi)-s1;
sep=2*(theta1-theta2);
offset=r2*SIND(theta2);
spoke1=2*(r1*SIND(theta1)-s1);
spoke2=2*(r2*SIND(theta2)-s2);
psol ();

printf ("\nYour data is on: %s\n",ofile);

/* post run processing */
mexit:
fcloseall();
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* psol..------------------------------------------------------------------ */

/* display a solution */

void psol (void)

{
int k;
dbl t;

fprintf (fp,"Number of spokes = %d\n",nspoke);
fprintf (fp,"Radius on which inner holes are located (R1) = %.3lf in\n",r1);
fprintf (fp,"Radius on which outer holes are located (R2) = %.3lf in\n",r2);
fprintf (fp,"Diameter of inner holes (2*r1) = %.3lf in\n",d1);
fprintf (fp,"Diameter of outer holes (2*r2) = %.3lf in\n",d2);
fprintf (fp,"Distance from spoke CL to outer hole center (d2) = %.3lf in\n",offset);
fprintf (fp,"\n");
fprintf (fp,"Angle from spoke CL to inner hole center (theta1) = %.3lf deg\n",theta1);
fprintf (fp,"Angle from spoke CL to outer hole center (theta2) = %.3lf deg\n",theta2);
fprintf (fp,"Angle btw two outer holes in same web space = %.3lf deg\n",sep);
fprintf (fp,"Inner spoke width = %.3lf in\n",spoke1);
fprintf (fp,"Outer spoke width = %.3lf in\n",spoke2);
fprintf (fp,"Minimum radial web length (W) = %.3lf in\n",w);
fprintf (fp,"\n");
fprintf (fp,\
  "Assuming that a spoke CL is initially aligned with the mill y axis,\n");
fprintf (fp,\
  "the rotary table must be rotated (phi=) %.3lf deg to bring the spoke edge\n",phi);
fprintf (fp,\
  "parallel to the mill y axis.\n");
fprintf (fp,\
  "The table must then be offset by (x=) %.3lf in plus half the cutter diameter.\n",x);
fprintf (fp,"\n");
fprintf (fp,\
  "The rotary table settings (deg) for the inner holes are:\n");

t=theta1;
while (t < 360.) {fprintf (fp,"%.3lf\n",t); t+=2*theta1;}
fprintf (fp,"\n");

fprintf (fp,\
  "The rotary table settings (deg) for the outer holes are:\n");

t=theta2; k=0;
while (t < 360.) 
	{fprintf (fp,"%.3lf\n",t); if (k) t+=2*theta2; else t+=sep; k=k^1;}
fprintf (fp,"\n");
}
/* ------------------------------------------------------------------------ */
