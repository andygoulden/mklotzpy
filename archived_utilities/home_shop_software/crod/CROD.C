/* CROD.C   M. W. Klotz   4/01, 2/05

Piston gudgeon position as function of crank angle, throw and connecting rod
length.  Written at the suggestion of Tom Roach (troach@sn.apc.org) in
South Africa.

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
#define beep printf ("\a")
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
*/
#include <graphics.h>

/* file management routines */
/* #include <file.h> */

/* pmark..  functions peculiar to this program */

void savescreen (void);
void restorescreen (void);
void setup (void);
void tic (char a, real x, real y, real del, int col);
void graph (void);
void mouse (uint a, uint b, uint c, uint d);
void mouseinit (void);
void button (void);
void showmenu (void);
void hidemenu (void);
void pwin (char *str, dbl wx, dbl wy, int col);
void wdata (void);

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

FILE *fp;			/* utility file pointer */
char ofile[]="CROD.OUT";	/* output file */
uint sbuffer[2000];			/* screen buffer */
uint cursorloc,cursormode,pagesave,kbflag;	/* screen saving */
char gp[80];				/* general purpose buffer */
dbl mx1,my1,mx2,my2;			/* menu ul,lr corners */
dbl mx,my;				/* menu size */
char menuflag;				/* signals if menu displayed */
char _huge *mstor;			/* storage for menu screen area */
uint ma,mb,mc,md;			/* mouse position storage */

dbl throw;				/* crank throw */
dbl conl;				/* connecting rod length */
dbl theta;				/* crank angle */
dbl dtheta;				/* crank angle increment */
dbl st,ct;

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

puts ("Units of measurement don't matter but must be consistent");
vin ("Connecting rod length",&conl,"lf",2.4,"");
puts ("Radius measured from crank center to connecting rod driver center");
vin ("Crank radius",&throw,"lf",0.6,"");
vin ("Angular increment",&dtheta,"lf",5.0,"deg");

setup();		/* setup screen, mouse and parameters */
agind: graph();		/* do the graphing */
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
	sprintf (gp,"%+6.2f,%+6.2f",mo.x,mo.y); gtext (gp,2,60,15);
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
wdata ();
printf ("\nYour data is on: %s\n",ofile);
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
/* setup..----------------------------------------------------------------- */

/* setup screen and parameters

TRY TO ADHERE TO:

CENTER OF SCREEN IS (0,0)
ANGLES MEASURED IN DEGREES POSITIVE CCW FROM X AXIS
SCREEN RATIO (XMAX-XMIN)/(YMAX-YMIN) = 640/480 = 1.3333333

*/

void setup (void)

{
xmin=-20.; xmax=190.;			/* screen window limits */
ymin=-0.25*throw; ymax=1.2*(throw+conl);
vgascreen();				/* setup graphics screen */
custom();				/* custom colors */
_setwindow(TRUE,xmin,ymin,xmax,ymax);	/* define window */
mouseinit();				/* initialize mouse */
wx=ABS(xmax-xmin); wy=ABS(ymax-ymin);
		/* menu setup */
/* menu area dimension
 16 pix/row, 8 pix/column */
mx=0.25*wx;
my=wy*2*16/pixy;		/* two lines in menu */
mx1=xmin; my1=ymax; mx2=mx1+mx; my2=my1-my;	/* menu location */
menuflag=0;
mstor=(char _huge *)malloc ((size_t)_imagesize_w(mx1,my1,mx2,my2));
		/* data setup */
}
/* graph..---------------------------------------------------------------- */

/* graph the figure */

void graph (void)

{
char buf [10];
int i,r;
dbl t,z,tl,zl;

cls;
wbox(xmin,ymin,xmax,ymax,15,0);		/* frame working area */
_setlinestyle (0xAAAA);			/* dotted line */
wline(xmin,0,xmax,0,15);		/* x axis */
wline(0,ymin,0,ymax,15);		/* y axis */

for (theta=0. ; theta <=180.0 ; theta+=15.)
	{wline (theta,0.,theta,ymax,15);}
wline (0.,conl+throw,180.,conl+throw,15);
wline (0.,conl-throw,180.,conl-throw,15);
wline (0.,conl-2.*throw,180.,conl-2.*throw,15);
_setlinestyle (0xFFFF);			/* solid line */

z=-.002*(ymax-ymin);

pwin ("0",0.,z,7);		/* tic labels */
pwin ("45",45.-3.,z,7);
pwin ("90",90.-3.,z,7);
pwin ("135",135.-4.,z,7);
pwin ("180",180.-4.,z,7);

sprintf (buf,"%.1f",conl+throw);
pwin (buf,-15.,conl+throw,7);
sprintf (buf,"%3.1f",conl-throw);
pwin (buf,-15.,conl-throw,7);
sprintf (buf,"%3.1f",conl-2.*throw);
pwin (buf,-15.,conl-2.*throw,7);

r=16;
gtext ("gudgeon pin position measured relative to crank center",r++,11,14);
tl=0.; zl=throw+conl;
for (theta=0. ; theta <=180.0 ; theta+=dtheta)
{
st=SIND(theta); ct=COSD(theta);
z=throw*ct+sqrt(conl*conl-throw*throw*st*st);
wline (tl,zl,theta,z,14);
tl=theta; zl=z;
}
gtext ("gudgeon pin position measured relative to tdc g pin position",r++,11,12);
tl=0.; zl=0.;
for (theta=0. ; theta <=180.0 ; theta+=dtheta)
{
st=SIND(theta); ct=COSD(theta);
z=throw+conl-(throw*ct+sqrt(conl*conl-throw*throw*st*st));
wline (tl,zl,theta,z,12);
tl=theta; zl=z;
}
gtext ("left click on any point for coordinates",r++,11,15);
}
/* pwin..------------------------------------------------------------------ */

/* place a string at a designated WINDOW coordinate location */

void pwin (char *str, dbl wx, dbl wy, int col)

{
short sx,sy;
char *wsave, *text;

/* allocate memory: 16 pix/row, 8 pix/column */
sy=16;			/* one row */
sx=8*strlen(str);	/* as many columns as in string */
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
_putimage_w (wx,wy,text,_GPSET);	/* place string on screen */
_nfree (wsave); _nfree (text);		/* deallocate memory */
}
/* wdata..----------------------------------------------------------------- */

/* write the output file */

void wdata (void)

{
dbl x;

if ((fp=fopen(ofile,"wt")) == NULL)
	{printf ("FAILED TO OPEN OUTPUT FILE: %s\n",ofile); quit;}

fprintf (fp,"Gudgeon Pin Position\n");
fprintf (fp,"Connecting rod length = %.4lf\n",conl);
fprintf (fp,"Crank throw = %.4lf\n",throw);
fprintf (fp,"Angular increment = %.4lf deg\n\n",dtheta);
fprintf (fp,"A = crank angle (deg) (0=tdc,180=bdc)\n");
fprintf (fp,"X = gudgeon pin position (measured relative to crank center)\n");
fprintf (fp,"Z = gudgeon pin position (measured relative to tdc)\n\n");
fprintf (fp,"%9s%9s%9s\n\n","A","X","Z");

for (theta=0. ; theta <=180.0 ; theta+=dtheta)
{
st=SIND(theta); ct=COSD(theta);
x=throw*ct+sqrt(conl*conl-throw*throw*st*st);
fprintf (fp,"%9.3lf%9.3lf%9.3lf\n",theta,x,throw+conl-x);
}

fclose (fp);
}
/* ------------------------------------------------------------------------ */
