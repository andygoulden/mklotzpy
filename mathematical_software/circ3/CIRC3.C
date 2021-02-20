/* CIRC3.C   M. W. Klotz   7/99

Find radius of circle passing through three points given distances between
points.

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

void bisect (dbl x1, dbl y1, dbl x2, dbl y2, dbl *x3, dbl *y3, dbl *x4,\
  dbl *y4);
void circ3 (dbl x1, dbl y1, dbl x2, dbl y2, dbl x3, dbl y3,\
  dbl *xc, dbl *yc, dbl *r);
int intersect (dbl x1, dbl y1, dbl x2, dbl y2, dbl x3, dbl y3,\
  dbl x4, dbl y4, dbl *xi, dbl *yi);
void loc (dbl x1, dbl y1, dbl d1, dbl x2, dbl y2, dbl d2, dbl *xs,\
  dbl *ys, dbl *xt, dbl *yt);

/* gmark..-------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* vmark..------------ global variable declarations ----------------------- */

dbl d12,d13,d23;

/* smark..----------- global structure declarations ----------------------- */

/* main..================================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i,j,k;
dbl x1,y1,x2,y2,x3,y3,xs,ys,xt,yt,xc,yc,r,dmax;

/* -------------------------- initializations ------------------------------ */
textscreen ();
/* ------------------------------------------------------------------------- */

puts ("CIRCLE THROUGH THREE POINTS\n");

puts ("Number the three points 1-3 (in any order).");
puts ("Enter lengths between points as specified below:\n");

agin:
vin ("length 1-2",&d12,"lf",2.,"");
vin ("length 1-3",&d13,"lf",1.1,"");
vin ("length 2-3",&d23,"lf",1.1,"");

dmax=-1.; dmax=MAX(dmax,d12); dmax=MAX(dmax,d13); dmax=MAX(dmax,d23);
if (dmax == d13) {d13=d12; d12=dmax;}
if (dmax == d23) {d23=d12; d12=dmax;}

if ((d13+d23) <= d12)
{
puts ("NO SOLUTION FOR THOSE DIMENSIONS - CHECK YOUR MEASUREMENTS");
goto agin;
}

/*
vpr ("d12",&d12,"lf\n"); vpr ("d13",&d13,"lf\n"); vpr ("d23",&d23,"lf\n");
*/

x1=0; y1=0; x2=d12; y2=0;
loc (x1,y1,d13,x2,y2,d23,&xs,&ys,&xt,&yt);

/* vpr ("xs",&xs,"lf\n"); vpr ("ys",&ys,"lf\n"); */

circ3 (x1,y1,x2,y2,xs,ys,&xc,&yc,&r);

printf ("RADIUS = %lf\n",r);
printf ("DIAMETER = %lf\n",2.*r);

/* post run processing */
mexit:
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* circ3..----------------------------------------------------------------- */

/*
find the center and radius of a circle that passes through three points
using the traditional perpendicular bisectors of the chords.
the center location is stored in (xc,yc) and the radius of the circle in r.

note that no error checking is done for the anomalous case in which the three
points are colinear.
*/

void circ3 (dbl x1, dbl y1, dbl x2, dbl y2, dbl x3, dbl y3,\
dbl *xc, dbl *yc, dbl *r)

{
dbl xa,ya,xb,yb,xd,yd,xe,ye;

bisect (x1,y1,x2,y2,&xa,&ya,&xb,&yb);
bisect (x2,y2,x3,y3,&xd,&yd,&xe,&ye);
intersect (xa,ya,xb,yb,xd,yd,xe,ye,xc,yc);
*r=RSS2((x1-*xc),(y1-*yc));
}
/* bisect..---------------------------------------------------------------- */

/*
the line (x3,y3)-(x4,y4) is the perpendicular bisector of (x1,y1)-(x2,y2)

no error checking for anomalous case where p1=p2.
that case should be trapped during the data input phase.
*/

void bisect (dbl x1, dbl y1, dbl x2, dbl y2, dbl *x3, dbl *y3, dbl *x4,\
dbl *y4)

{
dbl d;

d=RSS2((x2-x1),(y2-y1)); loc (x1,y1,d,x2,y2,d,x3,y3,x4,y4);
}
/* intersect..------------------------------------------------------------- */

/* 
find the intersection point (xi,yi) of the two lines:
(x1,y1) - (x2,y2) and (x3,y3) - (x4,y4)
returns:
0	if lines parallel
1	otherwise
*/

int intersect (dbl x1, dbl y1, dbl x2, dbl y2, dbl x3, dbl y3,\
dbl x4, dbl y4, dbl *xi, dbl *yi)

{
dbl d;

d=(y2-y1)*(x4-x3)-(y4-y3)*(x2-x1);
if (d == 0.0) {*xi=0.; *yi=0.; return 0;}

*xi=((x1*y2-x2*y1)*(x4-x3)-(x3*y4-x4*y3)*(x2-x1))/d;
*yi=((x1*y2-x2*y1)*(y4-y3)-(x3*y4-x4*y3)*(y2-y1))/d;
return 1;
}
/* loc..------------------------------------------------------------------- */

/*
find point at given distances from two other points

DERIVATION OF THESE EQUATIONS IS IN MATHEMATICS REFERENCE FILE

X1,Y1	=	FIRST POINT
D1	=	DISTANCE FROM FIRST POINT
X2,Y2	=	SECOND POINT
D2	=	DISTANCE FROM SECOND POINT
XS,YS	=	LOCATION OF POINT THAT SATISFIES:
(XS-X1)^2+(YS-Y1)^2 = D1^2  and  (XS-X2)^2+(YS-Y2)^2 = D2^2
XT,YT	=	ALTERNATE LOCATION OF POINT THAT SATISFIES ABOVE
*/

void loc (dbl x1, dbl y1, dbl d1, dbl x2, dbl y2, dbl d2, dbl *xs,\
dbl *ys, dbl *xt, dbl *yt)

{
dbl d3,p,a1,a2,a3,beta,phi;

d3=RSS2((x2-x1),(y2-y1));
p=.5*(d1+d2+d3);
a1=2.*ACSD(sqrt(p*(p-d1)/(d2*d3)));
a3=2.*ACSD(sqrt(p*(p-d3)/(d1*d2)));
a2=180.-a1-a3;
beta=ATAN2D((y2-y1),(x2-x1));
phi=beta-a2;
*xs=x1+d1*COSD(phi); *ys=y1+d1*SIND(phi);
*xt=x1+d1*COSD(phi+2.*a2); *yt=y1+d1*SIND(phi+2.*a2);
}
/* ------------------------------------------------------------------------ */
