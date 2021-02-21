/* EGG.C   M. W. Klotz   10/00, 08/01, 7/02, 1/03

Incremental cutting of a mathematically specified profile on the lathe
This version uses no input data file.  Part radii are generated in the
subroutine findr.

This adaption generates the profile of an egg.

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
#define beep tone(440,100)
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
#define DABS(a,b) (ABS((a)-(b)))
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

/* constant declarations - all 'defines' - no executable code */
#include <constant.h>

/* -------------------------- function prototypes ------------------------- */

/*
Minimize the number of these included since they contribute to exectuable
size.
*/

/*
global utility functions
void delay (clock_t wait);
char getkey (char *prompt);
void ptext (char *text, int row, int col, int fgd, int bkg);
void textscreen (void);
void tone (int frequency, long duration);
void kbdset (uint kbtog);
*/
#include <util.h>

/*
generalized io functions
void vin (char *prompt, void *x, char *format, dbl def, char *udef);
void vpr (char *label, void *x, char *format);
char cf (char *format);
int places (void *x, char *format);
void apr (char *label, void *x, char *format, int n1, int n2);
*/
/* #include <io.h> */

/*
simplified string tools
char *sex (char *d, char *s, int p, int n);
char *sdel (char *s, char c);
char *ssi (char *s, char *p, char *r, int n);
void sdp (char *s);
*/
/* #include <str.h> */

/* string manipulation */
/* #include <strings.h> */

/* data input routines */
/* #include <input.h> */

/* printing data */
/* #include <output.h> */

/* vector utilities */
/*
# define VMAG(a) sqrt((*a)*(*a)+(*(a+1))*(*(a+1))+(*(a+2))*(*(a+2)))
# define VDOT(a,b) ((*a)*(*b)+(*(a+1))*(*(b+1))+(*(a+2))*(*(b+2)))
real vadd (real *pt,real *p1,real *p2);
real vang (real *a,real* b);
real vcross (real *pc,real *p1,real *p2);
real vdot (real *p1,real *p2);
void veq (real *pt,real *p1);
real vmag (real *a);
void vnorm (real *p);
void vprint (FILE *fp,char *m,real *v);
real vsub (real *pt,real *p1,real *p2);
void vzero (real *p);
*/
/* #include <vector.h> */

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

/* file management routines */
/* #include <file.h> */

/* pmark..  functions peculiar to this program */

void savescreen (void);
void restorescreen (void);
void setup (void);
void cross (real x, real y, real del, int col);
void tic (char a, real x, real y, real del, int col);
void draw (void);
void mouse (uint a, uint b, uint c, uint d);
void mouseinit (void);
void button (void);
void showmenu (void);
void hidemenu (void);
void pwin (char *str, dbl xw, dbl yw, int col, char cx, char cy);
void loc (dbl x1, dbl y1, dbl d1, dbl x2, dbl y2, dbl d2, dbl *xs,\
dbl *ys, dbl *xt, dbl *yt);
int intersect (dbl x1, dbl y1, dbl x2, dbl y2, dbl x3, dbl y3,\
dbl x4, dbl y4, dbl *xi, dbl *yi);
void bisect (dbl x1, dbl y1, dbl x2, dbl y2, dbl *x3, dbl *y3, dbl *x4,\
dbl *y4);
void circ3 (dbl x1, dbl y1, dbl x2, dbl y2, dbl x3, dbl y3,\
dbl *xc, dbl *yc, dbl *r);
dbl angle (dbl x1, dbl y1, dbl x2, dbl y2, dbl xc, dbl yc);
void project (dbl x1, dbl y1, dbl x2, dbl y2, dbl d, dbl *x3, dbl *y3);
void spline (void);
dbl seval (dbl z);
real findr (real x);
real lin (real x1, real y1, real x2, real y2, real x);
void init (void);
real hcircle (real r, real t);
dbl egg (dbl x, dbl a, dbl b, dbl k);

/* gmark..-------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */
#if 0 == 1
#define rgb(b,g,r) (0x3f3f3fl & ((long)(b)<<16 | (g)<<8 | (r)))
struct videoconfig scrn;	/* defined in graph.h */
int pixx,pixy;			/* number of pixels in graphic screen */
int pcc;			/* storage for previous color */
long rbow [16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned char fullmask[]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
unsigned char halfmask[]={0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55};
#endif

dbl xmin,xmax,ymin,ymax;	/* screen window limits */
dbl wx,wy;

/* vmark..------------ global variable declarations ----------------------- */

uint sbuffer[2000];			/* screen buffer */
uint cursorloc,cursormode,pagesave,kbflag;	/* screen saving */
char gp[80];				/* general purpose buffer */
dbl mx1,my1,mx2,my2;			/* menu ul,lr corners */
dbl mx,my;				/* menu size */
char menuflag;				/* signals if menu displayed */
char _huge *mstor;			/* storage for menu screen area */
uint ma,mb,mc,md;			/* mouse position storage */

#define NP 100			/* maximum number of spline points */
dbl xsp[NP],ysp[NP];		/* storage for spline data */
dbl d[NP],u[NP],w[NP],p[NP];	/* working variables */
int nspline;			/* actual number of spline points */

real ds;			/* stock diameter (in) */
real rs;			/* stock radius (in) */
real wt;			/* tool width (in) */
real dxd;			/* axial stock cutting increment (in) */
int nseg;			/* number of spline segments */
int seg=-1;			/* index of current spline segment */
#define NS 100			/* maximum number of spline segments */
int segs[NS];			/* spline segment start index */
int segf[NS];			/* spline segment finish index */
char fname[]="EGG.OUT";	/* output file */
FILE *fp;

real xbegin,rbegin,xend,rend,oal;

/* smark..----------- global structure declarations ----------------------- */

struct mousestruc
{
uint a,b,c,d;
int row,col,key;
dbl x,y;
} mo;

/* main..====xxxxxxxx====================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i;

/* -------------------------- initializations ------------------------------ */
savescreen ();
/* ------------------------------------------------------------------------- */

init();			/* initialize */

if ((fp=fopen(fname,"wt")) == NULL)
	{printf ("FAILED TO OPEN OUTPUT FILE: %s",fname); quit;}

setup();		/* setup screen, mouse and parameters */
agind: draw();		/* do the drawing */
showmenu();		/* turn mouse menu on */

agin: button();		/* wait for mouse button press */
switch (mo.key)
{
case 1:			/* left */
	if (menuflag && (mo.x <= mx2) && (mo.y >= my2))
	{
	switch (mo.row)
		{
		case 1:
			goto mexit;
		case 2:
			hidemenu();
			break;
		case 3:
			hidemenu();
			mouse (2,0,0,0);	/* turn cursor off */
			goto agind;
		default:
			hidemenu();
			goto dis;
		}
	}
	else
	{
	/* display window, row/column, pixel location */
	dis: gtext ("               ",2,60,15);
	sprintf (gp,"%+6.3f,%+6.3f",mo.x,mo.y); gtext (gp,2,60,15);
#if 0 == 1
	gtext ("               ",3,60,15);
	sprintf (gp,"%6d,%6d",mo.row,mo.col); gtext (gp,3,60,15);
	gtext ("               ",4,60,15);
	sprintf (gp,"%6d,%6d",mo.c,mo.d); gtext (gp,4,60,15);
#endif
	}
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
restorescreen ();
printf ("data written to: %s\n",fname);
fcloseall ();
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* savescreen..------------------------------------------------------------ */

/* save the screen */
/* can't figure out how to address global variable sbuffer so awkward loop
structure using local temporary buffer results */

void savescreen (void)

{
#define TBSIZE 80
int i,l;
uint tbuffer[TBSIZE],js;

_asm
	{
	mov	ah,15			/* read video state */
	int	10h			/* bh = display page */
	xor	bl,bl
	xchg	bh,bl			/* bh = 0, bl = display page */
	mov	pagesave,bx
	}

for (i=0,js=0,l=0 ; i<(2000/TBSIZE) ; i++,js+=2*TBSIZE,l+=TBSIZE)
{
_asm
	{
	push	ds
	push	es
	push	si
	push	di

	mov	ax,pagesave
	mov	cx,4096
	mul	cx			/* ax = screen offset */
	add	ax,js
	mov	si,ax

	mov	di,0
	mov	cx,TBSIZE

	mov	ax,0xb800		/* screen segment */
	mov	ds,ax

ss1:	lodsw
	mov	tbuffer[di],ax
	inc	di
	inc	di
	loop	ss1

	mov	ah,2
	int	16h
	mov	kbflag,ax

	pop	di
	pop	si
	pop	es
	pop	ds
	}
memcpy (&sbuffer[l],&tbuffer[0],2*TBSIZE);
}
_asm					/* save cursor location, mode */
	{
	mov	ah,3
	mov	bx,pagesave
	int	10h
	mov	cursorloc,dx
	mov	cursormode,cx
	}
}
/* restorescreen..--------------------------------------------------------- */

/* restore the screen */
/* can't figure out how to address global variable sbuffer so awkward loop
structure using local temporary buffer results */

void restorescreen (void)

{
#define TBSIZE 80
int i,l;
uint tbuffer[TBSIZE],js;

_asm
	{
	mov	ax,pagesave
	mov	ah,5
	int	10h
	}
for (i=0,js=0,l=0 ; i<(2000/TBSIZE) ; i++,js+=2*TBSIZE,l+=TBSIZE)
{
memcpy (&tbuffer[0],&sbuffer[l],2*TBSIZE);
_asm
	{
	push	ds
	push	es
	push	si
	push	di

	mov	ax,pagesave
	mov	cx,4096
	mul	cx			/* ax = screen offset */
	add	ax,js
	mov	di,ax

	mov	si,0
	mov	cx,TBSIZE

	mov	ax,0xb800		/* screen segment */
	mov	es,ax

rs1:	mov	ax,tbuffer[si]
	stosw
	inc	si
	inc	si
	loop	rs1

	pop	di
	pop	si
	pop	es
	pop	ds
	}

}
_asm				/* restore cursor location, mode */
	{
	mov	ah,1
	mov	cx,cursormode
	int	10h
	mov	ah,2
	mov	bx,pagesave
	xchg	bh,bl
	mov	dx,cursorloc
	int	10h
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
gtext ("Redraw",r++,1,6);
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

/* find point at given distances from two other points */
/* 
DERIVATION OF THESE EQUATIONS IS IN MATHEMATICS REFERENCE FILE

SUB LOC (X1,Y1,D1,X2,Y2,D2,XS,YS,XT,YT) STATIC
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
/* circ3..----------------------------------------------------------------- */

/*
find the center and radius of a circle that passes through three points
using the traditional perpendicular bisectors of the chords
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
*/

void bisect (dbl x1, dbl y1, dbl x2, dbl y2, dbl *x3, dbl *y3, dbl *x4,\
dbl *y4)

{
dbl d;

d=RSS2((x2-x1),(y2-y1)); loc (x1,y1,d,x2,y2,d,x3,y3,x4,y4);
}
/* angle..----------------------------------------------------------------- */

/* 
return angle (deg) between two lines
(xc,yc) is the intersection point of the lines
*/

dbl angle (dbl x1, dbl y1, dbl x2, dbl y2, dbl xc, dbl yc)

{
dbl xa,ya,xb,yb,da,db;

xa=x1-xc; ya=y1-yc; da=RSS2(xa,ya); xb=x2-xc; yb=y2-yc; db=RSS2(xb,yb);
return ACSD((xa*xb+ya*yb)/(da*db));
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
/* spline..---------------------------------------------------------------- */

/* construct the cubic spline approximation */

void spline (void)

{
int i;

for (i=1 ; i<nspline-1 ; i++) d[i]=2.*(xsp[i+1]-xsp[i-1]);
for (i=0 ; i<nspline-1 ; i++) u[i]=xsp[i+1]-xsp[i];
for (i=1 ; i<nspline-1 ; i++)
  w[i]=6.*((ysp[i+1]-ysp[i])/u[i]-(ysp[i]-ysp[i-1])/u[i-1]);
p[0]=0.; p[nspline-1]=0.;
for (i=1 ; i<nspline-2 ; i++) {w[i+1]-=w[i]*u[i]/d[i]; d[i+1]-=u[i]*u[i]/d[i];}
for (i=nspline-2 ; i>0 ; i--) p[i]=(w[i]-u[i]*p[i+1])/d[i];
}
/* seval..----------------------------------------------------------------- */

/* evaluate the cubic spline interpolation at the point z */

dbl seval (dbl z)

{
#define f(a) ((a)*(a)*(a)-(a))
int i;
dbl t;

if (z < xsp[0] || z > xsp[nspline-1])
  {puts ("z out of range in seval"); return 0.0;}
for (i=0 ; i<nspline-1 ; i++) {if (z <= xsp[i+1]) {break;}}
t=(z-xsp[i])/u[i];
return t*ysp[i+1]+(1.-t)*ysp[i]+u[i]*u[i]*(f(t)*p[i+1]+f(1.-t)*p[i])/6.0;
}
/* lin..------------------------------------------------------------------- */

/* linear interpolation */

real lin (real x1, real y1, real x2, real y2, real x)

{
real a,b;
a=(y2-y1)/(x2-x1); b=.5*(y1+y2-a*(x1+x2)); return a*x+b;
}
/* setup..----------------------------------------------------------------- */

/* setup screen and parameters

TRY TO ADHERE TO:

CENTER OF SCREEN IS (0,0)
ANGLES MEASURED IN DEGREES POSITIVE CCW FROM X AXIS
SCREEN RATIO (XMAX-XMIN)/(YMAX-YMIN) = 640/480 = 1.3333333
LENGTHS LABELED AB SHOULD BE LABELED IN ALPHABETICAL ORDER

*/

void setup (void)

{
int i;

rs=0.5*ds;

/* note that x=0 corresponds to the end of the stock */
xmin=-xend; xmax=1.1*wt; ymax=2*ds; ymin=-ymax;

wx=DABS(xmax,xmin); wy=DABS(ymax,ymin);
if (wx > 4*wy/3)
{
ymax=3*wx/8;
ymin=-ymax;
}
else
{
wx=4*wy/3;
xmin=xmax-wx;
}

vgascreen();				/* setup graphics screen */
custom();				/* custom colors */
_setwindow(TRUE,xmin,ymin,xmax,ymax);	/* define window */
mouseinit();				/* initialize mouse */
wx=DABS(xmax,xmin); wy=DABS(ymax,ymin);
		/* menu setup */
/* menu area dimensions 16 pix/row, 8 pix/column */
mx=0.25*wx;
my=wy*3*16/pixy;		/* three lines in menu */
mx1=xmin; my1=ymax; mx2=mx1+mx; my2=my1-my;	/* menu location */
menuflag=0;
mstor=(char _huge *)malloc ((size_t)_imagesize_w(mx1,my1,mx2,my2));
}
/* draw..------------------------------------------------------------------ */

/* draw the figure */

void draw (void)

{
int i,il;
real x,xl,xr,xm,r,rl,rr,rm;

cls;
wbox(xmin,ymin,xmax,ymax,15,0);		/* frame working area */
_setlinestyle (0xAAAA);			/* dotted line */
wline(xmin,0,xmax,0,15);		/* x axis */
wline (xmin,rs,xmax,rs,12);		/* stock outline */
wline (xmin,-rs,xmax,-rs,12);		/* stock outline */
wline (0,-rs,0,1.5*rs,12);		/* end of stock */
_setlinestyle (0xFFFF);
wline (-oal,-rs,-oal,rs,15);		//mark end of piece
pwin ("x=0",0,1.5*rs,2,1,1);		/* label origin */
sprintf (gp,"stock diam = %.3f in",ds);
pwin (gp,xmin+0.05*wx,3.0*rs,2,0,0);
sprintf (gp,"tool width = %.3f in",wt);
pwin (gp,xmin+0.05*wx,2.8*rs,2,0,0);
sprintf (gp,"axial step = %.3f in",dxd);
pwin (gp,xmin+0.05*wx,2.6*rs,2,0,0);

/* draw the profile */
for (x=-dxd,xl=-xbegin,rl=rbegin ; x>=-xend ; x-=dxd)
{r=findr(-x); wline (xl,rl,x,r,15); wline (xl,-rl,x,-r,15); xl=x; rl=r;}

fprintf (fp,"stock diameter = %.3f in\n",ds);
fprintf (fp,"tool width = %.3f in\n",wt);
fprintf (fp,"axial cutting increment = %.3f in\n",dxd);
fprintf (fp,"start with left side of tool against end of stock\n\n");
fprintf (fp,"c=compound rest setting (axial feed)\n");
fprintf (fp,"d=diameter of resulting cut\n");
fprintf (fp,"doc=depth of cut\n\n");
fprintf (fp,"   i       c     doc       d\n\n");

/* calculate the cut depths; xl/xr=location of left/right/middle edge of tool */
for (xl=0,i=1 ; xl>=-xend ; xl-=dxd,i++)
{
xr=xl+wt; xm=xl+0.5*wt;
rl=findr(-xl);
if (xr < 0) rr=findr(-xr); else rr=0; r=MAX(rl,rr);
if (xm < 0) rm=findr(-xm); else rm=0; r=MAX(r,rm);
if (r < rs)
{
wbox (xl,-r,xr,-(r+rs),14,0);
if ((i-il) > 1) fprintf (fp,"  ...\n");
fprintf (fp,"%4d   %5.3f   %5.3f   %5.3f\n",i,-xl,rs-r,2*r);
il=i;
}
}
}
/* hcircle..--------------------------------------------------------------- */

/* return height of circle of radius r at distance t from right edge */

real hcircle (real r, real t)

{return sqrt (r*r-(r-t)*(r-t));}

/* init..------------------------------------------------------------------ */

void init (void)

{
ds=1.0;				//stock diameter
rs=0.5*ds;			//stock radius
wt=1./16.;			//tool tip width
dxd=0.01;			//axial stock cutting increment
oal=1.75*ds;			//overall length of piece
xbegin=0; rbegin=0;		//right end values
xend=oal+0.125; rend=0.1*rs;	//left end values of plot
return;
}
/* findr..----------------------------------------------------------------- */

/* find the radius of the profile at distance x from outboard end of stock */

real findr (real x)

{
dbl a;		// semi-major axis
dbl b;		// semi-minor axis
dbl k;		// asymmetry control factor

b=rs; a=1.5*b; k=0.6;

if (x < 2.*a) return egg (a-x,a,b,k);
else return rend;


}
/* egg..------------------------------------------------------------------- */

/* 
given x and the egg semi-axes, a and b, and the asymmetry, k, return the y
coordinate of the egg shape
*/

dbl egg (dbl x, dbl a, dbl b, dbl k)

{
return b*sqrt((1-x*x/(a*a))/(1-k*x));
}
/* ------------------------------------------------------------------------ */
