/* XYMWK.C   M. W. Klotz  5,6/99,12/00  (mklotz@alum.mit.edu)

A version of xydisp written to accept coordinates via a data file as opposed to
the hard-wired coordinates used in XYDISP.

Manipulate xy coordinates obtained by successive measurements with a height
gauge on a multiply oriented workpiece.

This program demonstrates my thoughts on how the display for such a manipulation
system should be implemented.

Written for Fred V. Fowler III of Fowler Instruments to be incorporated into a
software package to replace functions once included in a discontinued height
gauge (Trimos Vertical 3).

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
#define beep tone(440,100)
#define bpause beep; pause
#define bpausem beep; pausem
#define quit beep; fcloseall(); exit(1)
#define print printf("\n")
#define FALSE 0
#define TRUE !FALSE
#define cls _clearscreen(0)

/*
global utility functions (can return only one value)
note: no space between function name and parameter list
*/

#define SQR(a) ((a)*(a))
#define CUBE(a) ((a)*(a)*(a))
#define FRAC(a) ((a)-floor(a))		/* or use modf function */
#define MAX(a,b) ((a)>=(b) ? (a) : (b))
#define MIN(a,b) ((a)<=(b) ? (a) : (b))
#define ABS(a) ((a)>=0 ? (a) : -(a))
#define SGN(a) ((a)!=0 ? ((a)>0 ? 1 : -1) : 0)
#define ATOB(a,b) (pow((a),(b)))
#define LOG(a) (log(a))
#define LOG10(a) (log10(a))
#define SIND(a) sin((a)*rpd)
#define COSD(a) cos((a)*rpd)
#define TAND(a) tan((a)*rpd)
#define ASN(a) asin((ABS(a)<1) ? (a) : SGN(a))
#define ACS(a) acos((ABS(a)<1) ? (a) : SGN(a))
#define ASND(a) dpr*asin((ABS(a)<1) ? (a) : SGN(a))
#define ACSD(a) dpr*acos((ABS(a)<1) ? (a) : SGN(a))
#define ATAND(a) dpr*atan(a)
#define ATAN2(s,c) ((s!=0||c!=0) ? atan2((s),(c)) : 0)
#define ATAN2D(s,c) dpr*((s!=0||c!=0) ? atan2((s),(c)) : 0)
#define URAND(a,b) (((b)-(a))*(float)rand()/32767.+(a))
#define RSS2(a,b) sqrt((a)*(a)+(b)*(b))
#define RSS3(a,b,c) sqrt((a)*(a)+(b)*(b)+(c)*(c))
#define RND(a) floor ((a)+.5)

/* -------------------------- function prototypes ------------------------- */


/* graphics routines */
/* 
void box (int x1,int y1,int x2,int y2,int color,int fill);
void circle (int x, int y, int r, int color, int fill);
void custom (void);
void gprint (real x,int r,int col,int color);
void gtext (char *text,int row,int col,int color);
void line (int x1,int y1,int x2,int y2,int col);
void paint (int x,int y,int color,int border);
void rainbow (void);
void set (short x, short y, int color);
void vgascreen (void);
void wbox (double x1,double y1,double x2,double y2,int color,int fill);
void wcircle (double x, double y, double r, int color, int fill);
void wline (double x1,double y1,double x2,double y2,int col);
void wpaint (double x,double y,int color,int border);
void wset (double x, double y, int color);
void warc (dbl xc, dbl yc, dbl x1, dbl y1, dbl x2, dbl y2, int col);
*/
#include <graphics.h>

/* pmark..  prototypes of functions peculiar to this program */

void align (struct coord *s, dbl x1, dbl y1, dbl x2, dbl y2, struct coord *d);
dbl angle (dbl x1, dbl y1, dbl x2, dbl y2, dbl x3, dbl y3);
void bisect (dbl x1, dbl y1, dbl x2, dbl y2, dbl *x3, dbl *y3, dbl *x4,\
  dbl *y4);
void button (void);
void circ3 (dbl x1, dbl y1, dbl x2, dbl y2, dbl x3, dbl y3,\
  dbl *xc, dbl *yc, dbl *r);
void copy (struct coord *s, struct coord *d);
void cross (real x, real y, real del, int col);
void display (int n, int k);
dbl dist (dbl x1, dbl y1, dbl x2, dbl y2);
void distance (struct coord *s);
void dms (dbl a);
void draw (struct coord *s);
void extrema (struct coord *s, int nd);
void hidemenu (void);
int intersect (dbl x1, dbl y1, dbl x2, dbl y2, dbl x3, dbl y3,\
  dbl x4, dbl y4, dbl *xi, dbl *yi);
void loc (dbl x1, dbl y1, dbl d1, dbl x2, dbl y2, dbl d2, dbl *xs,\
  dbl *ys, dbl *xt, dbl *yt);
void mouse (uint a, uint b, uint c, uint d);
void mouseinit (void);
void mselect (struct coord *s, dbl *x, dbl *y);
void pcirc (struct coord *s);
void plotd (struct coord *s, int nd);
void polar (struct coord *s);
void ppnt (struct coord *s, int i, dbl x, dbl y);
void project (dbl x1, dbl y1, dbl x2, dbl y2, dbl d, dbl *x3, dbl *y3);
void pwin (char *str, dbl xw, dbl yw, int col, char cx, char cy);
char rdata (void);
void refer (struct coord *s, dbl xr, dbl yr, struct coord *d);
void rotate (struct coord *s, dbl theta);
void setup (void);
void showmenu (void);
int snapto (struct coord *s);
void tic (char a, real x, real y, real del, int col);
void translate (struct coord *s);
dbl ux (dbl x);
dbl urx (dbl x);
dbl uy (dbl y);
dbl ury (dbl y);
void xalign (struct coord *s);

/* gmark..-------------- global graphics variables ------------------------ */

dbl xmin,xmax,ymin,ymax;		/* screen window limits */
dbl wx,wy;				/* main window size */
dbl mx1,my1,mx2,my2;			/* menu ul,lr corners */
dbl mx,my;				/* menu size */
char menuflag;				/* signals if menu displayed */
char _huge *mstor;			/* storage for menu screen area */
uint ma,mb,mc,md;			/* mouse position storage */

dbl pxmin,pxmax,pymin,pymax;		/* plotting window limits */
dbl mf=0.1;				/* plotting area margin factor */
dbl uxmin,uxmax,uymin,uymax;		/* usable area of plotting window */
dbl scalex,scaley;			/* plotting scale factors */
dbl dminx,dmaxx,dminy,dmaxy;		/* plotting data extremes */
int cp;					/* color of point index */
int cx=4;				/* color of x axis */
int cy=1;				/* color of y axis */
int cr=5;				/* color of radius */
int cang=7;				/* color of angle */

/* vmark..------------ global variable declarations ----------------------- */

dbl pi;			/* 3.1415926535898... */
dbl dpr;		/* degrees/radian */
dbl rpd;		/* radians/degree */
char gp[80];		/* general purpose string buffer */

FILE *fpt;			/* utility file pointer */
char dfile[]="XYMWK.DAT";	/* input data file */
#define NPMAX 50	/* maximum number of measured points */
int np;			/* actual number of measured points */
dbl pcx;		/* pitch circle - x location of center */
dbl pcy;		/* pitch circle - y location of center */
dbl pcr;		/* pitch circle radius */

int nd;			/* number of points to display */
dbl xp,yp;		/* user prompt location */
dbl xm,ym;		/* message location */
int cpdone;		/* used in color coding */
char snapflag=1;	/* snap action flag (0=off, 1=on) */

/* smark..----------- global structure declarations ----------------------- */

struct mousestruc
{
uint a,b,c,d;
int row,col,key;
dbl x,y;
} mo;

/* coordinate structure */
struct coord {
dbl x;		/* x coordinate */
dbl y;		/* y coordinate */
dbl r;		/* radius */
dbl ang;	/* 4-quadrant angle measured positive ccw from x axis (deg)*/
};

struct coord hd[NPMAX];		/* data as read from height gauge */
struct coord hp[NPMAX];		/* transformed [(p)rimed] coordinates */
struct coord ctemp[NPMAX+1];	/* temporary storage */

/* main..================================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i;

/* -------------------------- initializations ------------------------------ */
textscreen ();		/* initialize and clear the screen */
pi=acos(-1.);		/* assures precision matches language capability */
dpr=180./pi; rpd=pi/180.;
/* ------------------------------------------------------------------------- */

rdata();			/* read the data file */
nd=np;
cpdone=2;

copy (&hd[0],&hp[0]);		/* initially, primed = measurement */

setup();			/* setup screen, mouse and parameters */


agind:
mouse (2,0,0,0);		/* turn cursor off */
draw (&hp[0]);			/* do the drawing */
mouse (1,0,0,0);		/* turn cursor on */
showmenu();			/* turn mouse menu on */

agin: button();		/* wait for mouse button press */
switch (mo.key)
{
case 1:			/* left */
	if (menuflag && (mo.x <= mx2) && (mo.y >= my2))
	{
	switch (mo.row)
		{
		case 1:				/* exit */
			goto mexit;
		case 2:
			hidemenu();		/* hide menu */
			break;
		case 3:				/* reference */
			translate(&hp[0]);
			goto agind;
		case 4:				/* align */
			xalign(&hp[0]);
			goto agind;
		case 5:				/* snap */
			snapflag=snapflag^1;
			goto agind;
		case 6:				/* distance */
			distance(&hp[0]);
			goto agind;
		case 7:				/* pitch circle */
			if (np < 3) {beep; goto agind;}
			pcirc(&hp[0]);
			goto agind;
		default:
			hidemenu();
			goto agind;
		}
	}
	else goto agind;
	break;
case 2:			/* right */
	if (menuflag) hidemenu(); else showmenu ();
	break;
case 3:			/* leftright */
	goto mexit;
case 4:			/* middle */
case 5:			/* leftmiddle */
case 6:			/* rightmiddle */
case 7:			/* alldown */
default:
	beep; break;
}
goto agin;

/* post run processing */
mexit:
textscreen();
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* copy..------------------------------------------------------------------ */

/* 
copy the coordinate structure pointed to by s (source) to the coordinate
structure pointed to by d (destination)
*/

void copy (struct coord *s, struct coord *d)

{
int i;

for (i=0 ; i<np ; i++)
	{
	d[i].x=s[i].x;
	d[i].y=s[i].y;
	d[i].r=s[i].r;
	d[i].ang=s[i].ang;
	}
}
/* display..--------------------------------------------------------------- */

/* display the coordinates
n = -1  => display all coordinates
else display the coordinates for the nth point
C indexes arrays starting with zero, so the ith point (indexed starting with
one) is in the (i-1)th array element.
*/

void display (int n, int k)

{
int i,i1,i2;

polar (&hp[0]);			/* express coordinates in polar notation */
if (n < 0) {i1=0; i2=np;} else {i1=n-1; i2=n;}
if (k)
{
printf ("\n%s\n",gp);
printf ("%2s: %8s %8s  %8s %8s  %8s %9s  %10s\n",\
  "PT","hdx","hdy","hpx","hpy","hpr","hpang(d)","hpang(d:m:s)");
}
for (i=i1 ; i<i2 ; i++)
	{
	printf ("%2d: %8.3lf %8.3lf",i+1,hd[i].x,hd[i].y);
	printf ("  %8.3lf %8.3lf",hp[i].x,hp[i].y);
	printf ("  %8.3lf %9.4lf",hp[i].r,hp[i].ang);
	dms(hp[i].ang);		/* print angle in (d:m:s) notation */
	print;
	}
}
/* dms..------------------------------------------------------------------- */

/* print an angle in degree:minute:second format */

void dms (dbl a)

{
int d,m,s,x;
char sgn=' ';
dbl f,z;

z=ABS(a); if (a < 0) sgn='-';
d=floor(z);
x=RND((z-d)*3600.); m=x/60; s=x-m*60;
if (s >= 60) {s-=60; m++;}
if (m >= 60) {m-=60; d++;}
printf ("  (%1c%3d:%02d:%02d)",sgn,d,m,s);
}
/* refer..----------------------------------------------------------------- */

/*
reference the coordinates in the structure pointed to by s (source) to the
point xr,yr and store the adjusted coordinates in the structure pointed to by
d (destination)
*/

void refer (struct coord *s, dbl xr, dbl yr, struct coord *d)

{
int i;

for (i=0 ; i<np ; i++) {d[i].x=s[i].x-xr; d[i].y=s[i].y-yr;}
}
/* polar..----------------------------------------------------------------- */

/*
express the x,y coordinates in the structure pointed to by s (source)
in polar notation and store them in the same structure
*/

void polar (struct coord *s)

{
int i;

for (i=0 ; i<np ; i++)
	{
	s[i].r=RSS2(s[i].x,s[i].y);
	s[i].ang=ATAN2D(s[i].y,s[i].x);	/* four quadrant arctangent */
	}
}
/* circ3..----------------------------------------------------------------- */

/*
find the center and radius of a circle that passes through three points
using the traditional perpendicular bisectors of the chords.
the center location is stored in (xc,yc) and the radius of the circle in r.

note that no error checking is done for the anomalous case in which the three
points are colinear.  final version should include such checking.
can be done by aligning to points 1 and 2; if y coordinate of point 3 is zero
after said alignment, points are colinear.
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
/* project..--------------------------------------------------------------- */

/*
find (x3,y3) a distance d from (x1,y1) on line (x1,y1)-(x2,y2)
d can be longer than the 1-2 distance
*/

void project (dbl x1, dbl y1, dbl x2, dbl y2, dbl d, dbl *x3, dbl *y3)

{
dbl l;

l=RSS2((x2-x1),(y2-y1)); *x3=x1+d*(x2-x1)/l; *y3=y1+d*(y2-y1)/l;
}
/* dist..------------------------------------------------------------------ */

/* return distance between (x1,y1) and (x2,y2) */

dbl dist (dbl x1, dbl y1, dbl x2, dbl y2)

{
return RSS2((x2-x1),(y2-y1));
}
/* angle..----------------------------------------------------------------- */

/* 
return the angle (deg) between
line (x1,y1) - (x2,y2) and line (x2,y2) - (x3,y3)

returns zero if p1=p2 or p2=p3
*/

dbl angle (dbl x1, dbl y1, dbl x2, dbl y2, dbl x3, dbl y3)

{
dbl dot,r12,r23;

r12=dist(x1,y1,x2,y2); r23=dist(x2,y2,x3,y3);
if (r12 == 0. || r23 == 0.) return 0.;
dot=(x1-x2)*(x3-x2)+(y1-y2)*(y3-y2);
return ACSD (dot/(r12*r23));
}
/* align..----------------------------------------------------------------- */

/* 
the coordinates in the (s)ource structure are "aligned" such that the point
(x1,y1) becomes the origin and the point (x2,ye) lies on the x axis.  this is
accomplished by first referencing all coordinates to (x1,y1) and then
calculating the coordinate rotation angle needed to zero the y coordinate of
(x2,y2).  all points are then rotated by the calculated angle and the results
are stored in the (d)estination coordinate structure.
*/

void align (struct coord *s, dbl x1, dbl y1, dbl x2, dbl y2, struct coord *d)

{
dbl theta;		/* rotation angle */

refer (s,x1,y1,ctemp);			/* reference to point x1,y1 */
theta=ATAN2D((y2-y1),(x2-x1));		/* compute rotation angle */
rotate (&ctemp[0],theta);		/* do the rotation */
copy (&ctemp[0],d);			/* store the results */

}
/* rotate..---------------------------------------------------------------- */

/* rotate all the coordinates in (s)ource structure by angle theta (deg) */

void rotate (struct coord *s, dbl theta)

{
int i;
dbl ct,st,x,y;

ct=COSD(theta); st=SIND(theta);
for (i=0 ; i<np ; i++)
	{
	x=ct*s[i].x+st*s[i].y;
	y=-st*s[i].x+ct*s[i].y;
	s[i].x=x;
	s[i].y=y;
	}
}
/* mouse..----------------------------------------------------------------- */

/* assembler call to mouse interrupt */

void mouse (uint a, uint b, uint c, uint d)

{
_asm
	{
	mov	ax,a
	mov	bx,b
	mov	cx,c
	mov	dx,d
	int	33h
	mov	mo.a,ax
	mov	mo.b,bx
	mov	mo.c,cx
	mov	mo.d,dx
	}
}
/* mouseinit..------------------------------------------------------------- */

/* initialize the mouse */

void mouseinit (void)

{
mouse (0,0,0,0);		/* initialize */
mouse (7,0,0,pixx);		/* limit x position */
mouse (8,0,0,pixy);		/* limit y position */
mouse (15,0,8,16);		/* set motion */
mouse (4,0,pixx/2,pixy/2);	/* position mouse */
}
/* button..---------------------------------------------------------------- */

/* process mouse buttons */

void button (void)

{
char c;
uint mmax;
struct _wxycoord xy;

mouse (1,0,0,0);			/* turn cursor on */
waite: mouse (3,0,0,0);			/* wait for a press */
if (kbhit())				/* check keyboard for escape */
	{c=getch(); if (c == 27) {mo.key=3; return;}}
if (mo.b == 0) goto waite;
mmax=mo.b;
recheck: mouse (3,0,0,0);		/* wait for all buttons to be pressed */
if (mo.b > mmax) mmax=mo.b;
if (mo.b != 0) goto recheck;
mo.key=mmax; mo.row=(int)(mo.d/16+1); mo.col=(int)(mo.c/8+1);
xy=_getwindowcoord(mo.c,mo.d);		/* returns structure */
mo.x=xy.wx; mo.y=xy.wy;
}
/* cross..----------------------------------------------------------------- */

/* draw a cross at the indicated point */

void cross (real x, real y, real del, int col)

{tic ('h',x,y,del,col); tic ('v',x,y,del*wx/wy,col);}
/* tic..------------------------------------------------------------------- */

/* make tic marks */

void tic (char a, real x, real y, real del, int col)

{
real t;

_setlinestyle (0xFFFF);
if (a == 'h') {t=del*wx; wline (x-t,y,x+t,y,col);}
if (a == 'v') {t=del*wy; wline (x,y-t,x,y+t,col);}
}
/* showmenu..-------------------------------------------------------------- */

void showmenu (void)

{
int r=1;

mouse (2,0,0,0);			/* turn cursor off */
_getimage_w (mx1,my1,mx2,my2,mstor);	/* save menu area */
wbox (mx1,my1,mx2,my2,0,1);		/* clear menu area */
gtext ("Exit (RL or Esc)",r++,1,6);	/* print menu items */
gtext ("Toggle menu (R)",r++,1,6);
gtext ("Reference",r++,1,6);
gtext ("Align",r++,1,6);
if (snapflag) sprintf (gp,"Snap ON"); else sprintf (gp,"Snap OFF");
gtext (gp,r++,1,6);
gtext ("Distance",r++,1,6);
gtext ("Pitch circle",r++,1,6);
mouse (3,0,0,0);			/* save cursor location */
ma=mo.a; mb=mo.b; mc=mo.c; md=mo.d;
mouse (1,0,0,0);			/* turn cursor on */
menuflag=1;				/* menu is on */
}
/* hidemenu..-------------------------------------------------------------- */

void hidemenu (void)

{
mouse (2,0,0,0);			/* turn cursor off */
_putimage_w (mx1,my1,mstor,_GPSET);	/* restore menu area */
mouse (4,0,mc,md);			/* restore cursor position */
mouse (1,0,0,0);			/* turn cursor on */
menuflag=0;				/* menu is off */
}
/* pwin..------------------------------------------------------------------ */

/*
place a string at a designated WINDOW coordinate location
if the flags cx,cy are set, the string is centered on the coordinates xw,yw
*/

void pwin (char *str, dbl xw, dbl yw, int col, char cx, char cy)

{
short sx,sy;
int ls;
char *wsave, *text;
dbl px=xw,py=yw;

/* allocate memory: 16 pix/row, 8 pix/column */
sy=16;			/* one row */
ls=strlen(str);
sx=8*ls;		/* as many columns as in string */
wsave=(char *)_nmalloc ((size_t)_imagesize(0,0,sx,sy));
text=(char *)_nmalloc ((size_t)_imagesize(0,0,sx,sy));
_getimage (0,0,sx,sy,wsave);		/* save what's there */
_setcolor (0);				/* blank printing area */
_rectangle (_GFILLINTERIOR,0,0,sx,sy);
_settextposition (1,1);			/* print string... */
_settextcolor (col);			/* in color */
_outtext(str);
_getimage (0,0,sx,sy,text);		/* capture string image */
_putimage (0,0,wsave,_GPSET);		/* restore what was there */
if (cx) px=xw-4*(ls-1)*(wx/pixx);	/* centering */
if (cy) py=yw+7*(wy/pixy);
_putimage_w (px,py,text,_GPSET);	/* place string on screen */
_nfree (wsave); _nfree (text);		/* deallocate memory */
}
/* setup..----------------------------------------------------------------- */

/* setup screen and parameters */

void setup (void)

{
xmin=-320; xmax=320;			/* screen window limits */
ymin=-240; ymax=240;
pxmax=-20; pxmin=-300;			/* data plotting area */
pymax=100; pymin=pymax-0.75*(pxmax-pxmin);
xm=pxmin; ym=pymin-10;			/* message location */
xp=pxmin; yp=pymin-50;			/* prompt location */
vgascreen();				/* setup graphics screen */
custom();				/* custom colors */
_setwindow(TRUE,xmin,ymin,xmax,ymax);	/* define window */
mouseinit();				/* initialize mouse */
wx=ABS(xmax-xmin); wy=ABS(ymax-ymin);	/* main window size */
		/* menu setup */
/* menu area dimensions 16 pix/row, 8 pix/column */
mx=0.25*wx;
my=wy*7*16/pixy;		/* six lines in menu */
mx1=xmin; my1=ymax; mx2=mx1+mx; my2=my1-my;	/* menu location */
menuflag=0;
mstor=(char _huge *)malloc ((size_t)_imagesize_w(mx1,my1,mx2,my2));
}
/* plotd..----------------------------------------------------------------- */

/*
plot (first) nd points from the coordinate structure pointed to by (s)ource
into the plotting area defined by pxmin,pxmax,pymin,pymax

routine is generalized and will self-adjust if pxmin,pxmax,pymin,pymax are
changed.

amount of margin within plotting area adjustable by changing value of "mf".

routine ensures that the origin (0,0) will always lie within the usable
plotting area so x,y axes indicators are always visible.
*/

void plotd (struct coord *s, int nd)

{
int i;
dbl dx,dy,x,y,xc,yc;

polar(s);				/* develop polar coordinates */
dx=pxmax-pxmin; dy=pymax-pymin;		/* plotting area dimensions */
uxmin=pxmin+mf*dx; uxmax=pxmax-mf*dx;	/* usable plotting area */
uymin=pymin+mf*dy; uymax=pymax-mf*dy;
extrema (s,nd);				/* find data extrema */
/* calculate scale factors */
scalex=(uxmax-uxmin)/(dmaxx-dminx); scaley=(uymax-uymin)/(dmaxy-dminy);

/* plot scaled and color coded coordinate axes */
x=ux(0.); y=uy(0.);			/* origin location in screen coord. */
_setlinestyle (0xAAAA);			/* dotted line */
wline (x,y,x+mf*dx,y,cx);		/* draw x axis */
sprintf (gp,"x"); pwin (gp,x+0.5*mf*dx,y-0.1*mf*dy,cx,0,0);
wline (x,y,x,y+mf*dy,cy);		/* draw y axis */
sprintf (gp,"y"); pwin (gp,x-0.5*mf*dx,y+0.8*mf*dy,cy,0,0);
_setlinestyle (0xFFFF);			/* solid line */

/* plot data points */
xc=-10; yc=ymax-30;
sprintf (gp,"x"); pwin(gp,xc+55,yc+20,cx,0,0);
sprintf (gp,"y"); pwin(gp,xc+125,yc+20,cy,0,0);
sprintf (gp,"r"); pwin(gp,xc+195,yc+20,cr,0,0);
sprintf (gp,"ang"); pwin(gp,xc+280,yc+20,cang,0,0);
for (i=0 ; i<nd ; i++)		/* plot points into plotting area */
	{
	if (i<nd-1) cp=2; else cp=cpdone;
	cross (ux(s[i].x),uy(s[i].y),0.005,cp);
	sprintf (gp,"%2d",i+1);
	pwin (gp,ux(s[i].x)-.01*dx,uy(s[i].y)-.05*dy,cp,1,1);
	ppnt (s,i,xc,yc);	/* print cart/polar coordinates for ith point */
	yc-=16;
	}
}
/* ppnt..------------------------------------------------------------------ */

/* print the coordinate data for point i of structure s at (x,y) */

void ppnt (struct coord *s, int i, dbl x, dbl y)

{
sprintf (gp,"%2d:",i+1); pwin (gp,x,y,cp,0,0);
sprintf (gp,"%8.3lf",s[i].x); pwin (gp,x+30,y,cx,0,0);
sprintf (gp,"%8.3lf",s[i].y); pwin (gp,x+100,y,cy,0,0);
sprintf (gp,"%8.3lf",s[i].r); pwin (gp,x+170,y,cr,0,0);
sprintf (gp,"%9.4lf",s[i].ang); pwin (gp,x+255,y,cang,0,0);
}
/* extrema..--------------------------------------------------------------- */

/*
find data extrema in first nd points of coordinate structure pointed to by s
*/

void extrema (struct coord *s, int nd)

{
int i;

dmaxx=0.; dminx=0.; dmaxy=0.; dminy=0.;
for (i=0 ; i<nd ; i++)
	{
	if (s[i].x > dmaxx) dmaxx=s[i].x;
	if (s[i].x < dminx) dminx=s[i].x;
	if (s[i].y > dmaxy) dmaxy=s[i].y;
	if (s[i].y < dminy) dminy=s[i].y;
	}
}
/* ux..-------------------------------------------------------------------- */

/* data scaling utility;  data coordinates -> screen coordinates */

dbl ux (dbl x)

{return uxmin+scalex*(x-dminx);}
/* uy..-------------------------------------------------------------------- */

/* data scaling utility; data coordinates -> screen coordinates */

dbl uy (dbl y)

{return uymin+scaley*(y-dminy);}
/* urx..------------------------------------------------------------------- */

/* data scaling utility; screen coordinates -> data coordinates */

dbl urx (dbl x)

{return (x-uxmin)/scalex+dminx;}
/* ury..------------------------------------------------------------------- */

/* data scaling utility; screen coordinates -> data coordinates */

dbl ury (dbl y)

{return (y-uymin)/scaley+dminy;}
/* draw..------------------------------------------------------------------ */

/* draw the figure using nd points of structure s*/

void draw (struct coord *s)

{

cls;
wbox(xmin,ymin,xmax,ymax,15,0);		/* frame working area */
wbox(pxmin,pymin,pxmax,pymax,14,0);	/* frame plotting area */
_setlinestyle (0xFFFF);			/* default line style */
plotd (s,nd);
}
/* translate..------------------------------------------------------------- */

/* 
refer the coordinate system to the data point selected by mouse clicking
*/

void translate (struct coord *s)

{
int k;
dbl x,y;

sprintf (gp,"Select reference point by clicking on it");
pwin (gp,xp,yp,15,0,0);
mselect (s,&x,&y);
refer (s,x,y,&hp[0]);
}
/* mselect..--------------------------------------------------------------- */

/* select a point by clicking on it with the mouse */

void mselect (struct coord *s, dbl *x, dbl *y)

{
int k;

if (snapflag)
	{
	k=snapto(s);
	if (k < 0) {beep; return;}
	*x=s[k].x; *y=s[k].y;
	}
else
	{
	button();
	*x=urx(mo.x);
	*y=ury(mo.y);
	}
}
/* snapto..---------------------------------------------------------------- */

/* 
allow user to select a (data) point by clicking on it

the data point closest to the click location is returned
*/

int snapto (struct coord *s)

{
int i,kmin;
dbl d,dmin;

button ();
dmin=1.e9; kmin=-1;
for (i=0 ; i<np ; i++)
{
d=dist(mo.x,mo.y,ux(s[i].x),uy(s[i].y));
if (d < dmin) {dmin=d; kmin=i;}
}
return kmin;
}
/* xalign..---------------------------------------------------------------- */

/* 
align the coordinates to the two points selected by clicking
*/

void xalign (struct coord *s)

{
int k,l;
dbl x1,y1,x2,y2;

sprintf (gp,"Select origin point by clicking on it");
pwin (gp,xp,yp,15,0,0);
mselect (s,&x1,&y1);
sprintf (gp,"Select alignment point by clicking on it");
pwin (gp,xp,yp,15,0,0);
mselect (s,&x2,&y2);
align (s,x1,y1,x2,y2,&hp[0]);
}
/* distance..-------------------------------------------------------------- */

/* calculate distance between two selected points */

void distance (struct coord *s)

{
int k,l;
dbl x1,y1,x2,y2;

sprintf (gp,"Select first point by clicking on it");
pwin (gp,xp,yp,15,0,0);
mselect (s,&x1,&y1);
sprintf (gp,"Select second point by clicking on it");
pwin (gp,xp,yp,15,0,0);
mselect (s,&x2,&y2);
sprintf (gp,"Distance = %8.3lf",dist(x1,y1,x2,y2));
pwin (gp,xm,ym,14,0,0);
sprintf (gp,"Press a key                                         ");
pwin (gp,xp,yp,15,0,0);
pause;
}
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
		hd[np].x=(dbl)atof(strtok(temp,",\t;"));
		hd[np].y=(dbl)atof(strtok(NULL,",\t;"));
		if (++np == NPMAX)
			{
			printf
			("More than maximum (%d) data items in data file\n",NPMAX);
			quit;
			}
		}
	}

fclose (fpt);
return 0;
}
/* pcirc..----------------------------------------------------------------- */

/* 
find the pitch circle parameters for three selected points
*/

void pcirc (struct coord *s)

{
int k,l;
dbl x1,y1,x2,y2,x3,y3;

sprintf (gp,"Select first of three points by clicking on it");
pwin (gp,xp,yp,15,0,0);
mselect (s,&x1,&y1);
sprintf (gp,"Select second of three points by clicking on it");
pwin (gp,xp,yp,15,0,0);
mselect (s,&x2,&y2);
sprintf (gp,"Select third of three points by clicking on it");
pwin (gp,xp,yp,15,0,0);
mselect (s,&x3,&y3);
circ3 (x1,y1,x2,y2,x3,y3,&pcx,&pcy,&pcr);

/* 
plot the data and the circle center by adding the circle center to the existing
coordinates (temporarily) and redrawing the screen
*/
copy (&hp[0],&ctemp[0]);		/* copy existing data */
ctemp[nd].x=pcx; ctemp[nd].y=pcy;	/* add circle center */
nd++; np++;				/* adjust counts */
polar (&ctemp[0]);			/* develop polar coordinates */
mouse (2,0,0,0);			/* turn cursor off */
draw(&ctemp[0]);			/* redraw the screen */
mouse (1,0,0,0);			/* turn cursor on */
showmenu();				/* turn mouse menu on */
cross (ux(pcx),uy(pcy),0.008,15);	/* white cross on circle center */
nd--; np--;				/* adjust for point that was added */

sprintf (gp,"Circle center at x,y = %8.3lf,%8.3lf",pcx,pcy);
pwin (gp,xm,ym,14,0,0);
sprintf (gp,"Circle radius = %8.3lf",pcr);
pwin (gp,xm,ym-16,14,0,0);
sprintf (gp,"Press a key                                         ");
pwin (gp,xp,yp,15,0,0);
pause;
}
/* ------------------------------------------------------------------------ */
