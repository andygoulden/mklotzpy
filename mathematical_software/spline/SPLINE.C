/* SPLINE.C   M. W. Klotz   11/98

Cubic spline fit to a set of points (Sedgewick "Algorithms" pg. 545)

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

char rdata (void);
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
void spline (void);
dbl seval (dbl z);

/* gmark..-------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

dbl xmin,xmax,ymin,ymax;	/* screen window limits */
dbl wx,wy;

/* vmark..------------ global variable declarations ----------------------- */

char gp[80];				/* general purpose buffer */
dbl mx1,my1,mx2,my2;			/* menu ul,lr corners */
dbl mx,my;				/* menu size */
char menuflag;				/* signals if menu displayed */
char _huge *mstor;			/* storage for menu screen area */
uint ma,mb,mc,md;			/* mouse position storage */

FILE *fp;			/* general purpose file pointer */
char dfile[]="SPLINE.DAT";	/* spline data file */
#define NP 400			/* maximum number of spline points */
dbl x[NP],y[NP];		/* storage for spline data */
dbl d[NP],u[NP],w[NP],p[NP];	/* working variables */
int nspline;			/* actual number of spline points */

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
/* -------------------------- initializations ------------------------------ */
savescreen ();
/* ------------------------------------------------------------------------- */
rdata();		/* read data file */
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
	/* display window location */
	dis: gtext ("                           ",2,50,15);
	sprintf (gp,"x = %+6.2f, y = %+6.2f",mo.x,mo.y); gtext (gp,2,50,15);
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
exit(0);			/* normal exit */
}
/* ======================================================================== */
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
dbl f=0.2,dx,dy;

xmin=1e10; xmax=-xmin;
for (i=0 ; i<nspline ; i++) {xmin=MIN(xmin,x[i]); xmax=MAX(xmax,x[i]);}
ymin=1e10; ymax=-ymin;
for (i=0 ; i<nspline ; i++) {ymin=MIN(ymin,y[i]); ymax=MAX(ymax,y[i]);}
dx=ABS(xmax-xmin); dy=ABS(ymax-ymin);
xmin-=f*dx; xmax+=f*dx; ymin-=f*dy; ymax+=f*dy;
vgascreen();				/* setup graphics screen */
custom();				/* custom colors */
_setwindow(TRUE,xmin,ymin,xmax,ymax);	/* define window */
mouseinit();				/* initialize mouse */
wx=ABS(xmax-xmin); wy=ABS(ymax-ymin);
		/* menu setup */
/* menu area dimensions 16 pix/row, 8 pix/column */
mx=0.25*wx;
my=wy*3*16/pixy;		/* three lines in menu */
mx1=xmin; my1=ymax; mx2=mx1+mx; my2=my1-my;	/* menu location */
menuflag=0;
mstor=(char _huge *)malloc ((size_t)_imagesize_w(mx1,my1,mx2,my2));
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
/* spline..---------------------------------------------------------------- */

/* construct the cubic spline approximation */

void spline (void)

{
int i;

for (i=1 ; i<nspline-1 ; i++) d[i]=2.*(x[i+1]-x[i-1]);
for (i=0 ; i<nspline-1 ; i++) u[i]=x[i+1]-x[i];
for (i=1 ; i<nspline-1 ; i++) w[i]=6.*((y[i+1]-y[i])/u[i]-(y[i]-y[i-1])/u[i-1]);
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

if (z < x[0] || z > x[nspline-1]) {puts ("z out of range in seval"); return 0.0;}
for (i=0 ; i<nspline-1 ; i++) {if (z <= x[i+1]) {break;}}
t=(z-x[i])/u[i];
return t*y[i+1]+(1.-t)*y[i]+u[i]*u[i]*(f(t)*p[i+1]+f(1.-t)*p[i])/6.0;
}
/* draw..------------------------------------------------------------------ */

/* draw the figure */

void draw (void)

{
int i,nt;
dbl t,dt;

cls;
wbox(xmin,ymin,xmax,ymax,15,0);		/* frame working area */
_setlinestyle (0xAAAA);			/* dotted line */
wline(xmin,0,xmax,0,15);		/* x axis */
wline(0,ymin,0,ymax,15);		/* y axis */
_setlinestyle (0xFFFF);
spline ();
nt=40; t=x[0]; dt=(x[nspline-1]-t)/nt;
_setcolor (14); _moveto_w(t,seval(t));
while (t<=x[nspline-1]) {_lineto_w(t,seval(t)); t+=dt;}
for (i=0 ; i<nspline ; i++) cross (x[i],y[i],.005,15);
}
/* rdata..----------------------------------------------------------------- */

/* read the data file */

char rdata (void)

{
char temp[81],f;
int i;

if ((fp=fopen(dfile,"rt")) == NULL)
	{printf ("FAILED TO OPEN DATA FILE: %s\n",dfile); beep; return 1;}

nspline=0;			/* number of data entries */
f=0;
while ((fgets(temp,sizeof(temp),fp)) != NULL)
	{
	trim(temp);		/* uses library trim */
	if ((strstr(temp,"STARTOFDATA")) != NULL) {f=1; continue;}
	if (!f) continue;
	if ((strstr(temp,"ENDOFDATA")) != NULL) break;
	if (temp[0] != '\0' && temp[0] != ';')
		{
		/* decode and store data here */
		x[nspline]=atof(strtok(temp,",;"));
		y[nspline]=atof(strtok(NULL,";\n"));
		if (++nspline == NP)
			{
			printf
			("More than maximum (%d) data items in data file\n",NP);
			quit;
			}
		}
	}
do
{
for (i=0,f=0 ; i<nspline-1 ; i++)
{
if (x[i] > x[i+1])
  {swap (&x[i],&x[i+1],sizeof(x[i])); swap (&y[i],&y[i+1],sizeof(y[i])); f=1;}
}
} while (f);
fclose (fp);
return 0;
}
/* ------------------------------------------------------------------------ */
