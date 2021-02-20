/* CAM.C   M. W. Klotz   11/98,4/01

Cam design

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

void cam (void);
void cross (real x, real y, real del, int col);
void tic (char a, real x, real y, real del, int col);
void pwin (char *str, dbl xw, dbl yw, int col, char cx, char cy);

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
char ofile[]="CAM.DAT";		/* output file */


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
textscreen ();
/* ------------------------------------------------------------------------- */

if ((fp=fopen(ofile,"wt")) == NULL)
	{printf ("FAILED TO OPEN OUTPUT FILE: %s\n",ofile); quit;}
cam ();

/* post run processing */
mexit:
fcloseall ();
printf ("\nYour cam data is on: %s\n",ofile);
exit(0);			/* normal exit */
}
/* ======================================================================== */
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
/* cam..------------------------------------------------------------------- */

/* cam design */

void cam (void)

{
int ctype,rtype;
dbl rroll,rbase,risea,risel,rised,beta,betal,betaa,betad,rise,astep;
dbl pamax,scale,rbs,ocam,xl,yl,xc,yc,xt,yt,xp,yp,vt,vp,ba,bl,bd,th,pa;
dbl ang,theta,tob,d,vc,vmax,vcmax,smax,rob,rcam,pcam,tcam,xcam,ycam;
dbl ap,ac,at,amax;

puts ("CAM DESIGN\n");
puts ("TYPE OF FOLLOWER MOTION:");
puts ("  1 - STRAIGHT LINE (Infinite acceleration parabolically compensated)");
puts ("  2 - PARABOLIC (Smallest acceleration but produces impact shocks)");
puts ("  3 - SIMPLE HARMONIC (Smooth motion - acceleration causes vibration)");
puts ("  4 - CYCLOIDAL (RECOMMENDED - No shocks, low vibration and wear)");
vin ("Type desired",&ctype,"d",4.,"");

puts ("\nTYPE OF CAM FOLLOWER:");
puts ("  1 - FLAT-FACED");
puts ("  2 - ROLLER");
vin ("Type desired",&rtype,"d",1.,"");
if (rtype == 2) vin ("Radius of roller",&rroll,"lf",0.25,""); else rroll=0.0;

puts ("\nBase circle radius is the radius of the part of the cam that provides");
puts ("the minimum follower extension (the 'lowest' part of the cam)");
vin ("Base circle radius",&rbase,"lf",1.625,"");

if (ctype == 1)
{
puts ("\nConstant velocity cams have infinite start/end-of-stroke");
puts ("accelerations that cause unacceptable impact loads and wear.");
puts ("This is avoided by using constant acceleration and deceleration");
puts ("(PARABOLIC motion) at the beginning and end of rise.");
vin ("Allowable cam rise during acceleration",&risea,"lf",0.1,"");
vin ("Cam rise during linear motion portion of stroke",&risel,"lf",1.0,"");
vin ("Allowable cam rise during deceleration",&rised,"lf",0.15,"");
betal=120./(1.+2.*(risea+rised)/risel);
vin ("Cam rotation during which linear motion occurs",&betal,"lf",betal,"deg");
betaa=2.*betal*risea/risel; betad=2.*betal*rised/risel;
beta=betaa+betal+betad; rise=risea+risel+rised;
printf ("Cam will rise %.4lf during %.4lf deg of rotation\n",rise,beta);
}
else
{
puts ("\nCam rise is the distance the follower travels from the point where");
puts ("it touches the base circle to the point where it touches the highest");
puts ("point of the cam.");
vin ("Cam rise",&rise,"lf",1.25,"");
puts ("\nCam rotation angle is the angular arc of the cam during which");
puts ("the follower rise occurs.");
vin ("Cam rotation angle",&beta,"lf",120.0,"deg");
}
print;
vin ("Angular step size for constructing cam",&astep,"lf",1.0,"deg");
if (ctype == 1) fprintf (fp,"Cam Motion = Straight Line\n");
if (ctype == 2) fprintf (fp,"Cam Motion = Parabolic\n");
if (ctype == 3) fprintf (fp,"Cam Motion = Simple Harmonic\n");
if (ctype == 4) fprintf (fp,"Cam Motion = Cycloidal\n");
if (rtype == 1) fprintf (fp,"Follower Type = Flat-Faced\n");
if (rtype == 2) fprintf (fp,"Follower Type = Roller\n");
if (rtype == 2) fprintf (fp,"Follower Roller Radius = %.4lf\n",rroll);
fprintf (fp,"Base Circle Radius = %.4lf\n",rbase);
fprintf (fp,"Cam Rise (Stroke) = %.4lf\n",rise);
fprintf (fp,"Cam Rotation Angle = %.4lf deg\n",beta);
fprintf (fp,"\nAngle is measured from highest point of cam.\n");
fprintf (fp,"Radius & x,y coordinates measured from center of cam rotation.\n");
fprintf (fp,"\n     Angle      Radius     X-coord     Y-coord\n");

pamax=0;			/* maximum pressure angle */
scale=200./(rbase+rise);	/* scales drawing */
rbs=rbase*scale;		/* base radius in pixels */
ocam=scale*(2.*rbase+rise);	/* overall length of cam in pixels */
rob=rise/(beta*RPD);
xmin=-(320-.5*ocam+rbs);
xmax=640+xmin;
ymin=-240;
ymax=240;
vgascreen();				/* setup graphics screen */
custom();				/* custom colors */
_setwindow(TRUE,xmin,ymin,xmax,ymax);	/* define window */
wx=ABS(xmax-xmin); wy=ABS(ymax-ymin);
wcircle (0,0,rbs,12,0);			/* base circle */
cross (0,0,.05,12);
xl=rbs*COSD(beta); yl=rbs*SIND(beta);
wline (0,0,xl,yl,15); wline (0,0,xl,-yl,15);	/* lines to start of cam */
xt=xmin; yt=ymin; vt=ymin;
for (ang=beta ; ang>=0. ; ang-=astep)
{
theta=beta-ang; tob=theta/beta;
switch (ctype)
{
	case 1:
		ba=betaa*RPD; bl=betal*RPD; bd=betad*RPD; th=theta*RPD;
		if (theta <= betaa)
			{
			d=risea*SQR(th/ba);
			vc=2.*risea*th/(ba*ba);
			ac=2.*risea/(ba*ba);
			}
		else if (theta >= (betaa+betal))
			{
			th=(theta-betaa-betal)*RPD;
			d=rise-rised*SQR((bd-th)/bd);
			vc=risel/bl-2.*rised*th/(bd*bd);
			ac=-2.*rised/(bd*bd);
			}
		else
			{
			d=risea+risel*(theta-betaa)/betal;
			vc=risel/bl;
			ac=0;
			}
		vmax=risel/bl;
		amax=2.*MAX(risea/(ba*ba),rised/(bd*bd));
		break;
	case 2:
		if (tob <= 0.5)
			{
			d=2.*rise*tob*tob;
			vc=4.*rob*tob;
			ac=4.*rob/beta;
			}
		else
			{
			d=rise*(1.-2.*SQR(1.-tob));
			vc=4*rob*(1.-tob);
			ac=-4.*rob/beta;
			}
		amax=4.*rob/beta;
		vmax=2.*rob;
		break;
	case 3:
		d=.5*rise*(1.-cos(PI*tob));
		vc=PIO2*rob*sin(PI*tob);
		ac=PIO2*rob*PI*cos(PI*tob)/beta;
		amax=PIO2*rob*PI/beta;
		vmax=PIO2*rob;
		break;
	case 4:
		d=rise*(tob-sin(TWOPI*tob)/TWOPI);
		vc=rob*(1.-cos(TWOPI*tob));
		ac=rob*TWOPI*sin(TWOPI*tob)/beta;
		amax=rob*TWOPI/beta;
		vmax=2.*rob;
		break;
}
rcam=RSS2((rbase+d+rroll),vc)-rroll;
pcam=atan(vc/(rbase+d+rroll));
if (pcam > pamax) {pamax=pcam; vcmax=vc; smax=d;}
tcam=ang*RPD-pcam; xcam=rcam*cos(tcam); ycam=rcam*sin(tcam);
xc=scale*xcam; yc=scale*ycam;
fprintf (fp,"%10.4lf  %10.4lf  %10.4lf  %10.4lf\n",ang,rcam,xcam,ycam);
wline (xl,yl,xc,yc,14); wline(xl,-yl,xc,-yc,14); xl=xc; yl=yc;
xp=theta*640/beta+xmin; yp=d*480/rise+ymin;	/* cam rise */
if (ang==beta) xt=xp;
wline (xt,yt,xp,yp,10);
vp=vc*480/vmax+ymin;		/* converted velocity */
if (ang==beta) vt=vp;
wline (xt,vt,xp,vp,11);
ap=ac*240/amax;			/* acceleration */
if (ang==beta) at=ap;
wline (xt,at,xp,ap,13);
xt=xp; yt=yp; vt=vp; at=ap;
}
pwin ("Press a key", xmin+10,0.,15,0,0);
pause;
textscreen ();
printf ("The maximum pressure angle was %.4lf deg\n",pamax*DPR);
puts ("\nMaximum pressure angle should be less than 35 deg. to limit side");
puts ("thrust on the follower.  Pressure angle can be adjusted by modifying");
puts ("the base radius.  For the cam just specified, the following holds:");

printf ("\nPressure Angle (deg)   Base Radius\n");
for (pa=15 ; pa<=45 ; pa+=2.5)
{
rbase=(vcmax/TAND(pa))-smax-rroll;
printf ("%20.4lf   %11.4lf\n",pa,rbase);
}
}
/* ------------------------------------------------------------------------ */
