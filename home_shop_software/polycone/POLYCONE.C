/* POLYCONE.C   M. W. Klotz   7/03

Conical forms with regular polygon cross section

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
#include <vector.h>

/* graphics routines */
/* #include <graphics.h> */

/* file management routines */
/* #include <file.h> */

/* pmark..  functions peculiar to this program */

/* gmark..-------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* vmark..------------ global variable declarations ----------------------- */

int ns;			// number of sides
real c;			// chord length
real hc;		// half-chord length
real d;			// diameter of circumscribed base circle
real r;			// radius of circumscribed base circle
real phi;		// central half angle
real s;			// sagitta
real a;			// radius - sagitta
real barea;		// area of base

real h;			// cone height

real b;			// facet height
real e;			// edge length
real gamma;		// base angle
real delta;		// tip angle
real thetab;		// face angle
real thetae;		// edge angle
real beta;		// external angle between two faces
real farea;		// facet area

real volume;		// volume of cone

real v1[3],v2[3];

/* smark..----------- global structure declarations ----------------------- */

/* main..================================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i,j,k;
real st,ct,sp,cp;

/* -------------------------- initializations ------------------------------ */
textscreen ();
/* ------------------------------------------------------------------------- */

vin ("Number of polygon sides",&ns,"d",4.,"");
vin ("Length of polygon side",&c,"f",6.,"");
vin ("Cone height",&h,"f",10.,"");

hc=0.5*c;
phi=180./ns;
r=hc/SIND(phi); d=2.*r;
a=r*COSD(phi); s=r-a;
barea=ns*a*hc;

puts ("\nPOLYCONE BASE:");
printf ("Diameter / radius of circumscribed circle = %.4f / %.4f\n",d,r);
printf ("Length of perpendicular to side from center = %.4f\n",a);
printf ("Area of base = %.4f\n",barea);

b=RSS2(h,a);
e=RSS2(h,r);
gamma=ACSD(hc/e);
delta=2.*(90.-gamma);
thetab=ACSD(a/b);
thetae=ACSD(r/e);
farea=0.5*b*hc;

//calculation of external angle between two faces
st=SIND(thetab); ct=COSD(thetab);
v1[0]=a*st*st; v1[1]=0.; v1[2]=a*st*ct;
sp=SIND(2.*phi); cp=COSD(2.*phi);
v2[0]=cp*v1[0]-sp*v1[1]; v2[1]=sp*v1[0]+cp*v1[1]; v2[2]=v1[2];
beta=vang(v1,v2);

puts ("\nPOLYCONE TRIANGULAR FACE FACETS:");
printf ("Facet height = %.4f\n",b);
printf ("Edge length = %.4f\n",e);
printf ("Base angle = %.4f deg\n",gamma);
printf ("Tip angle = %.4f deg\n",delta);
printf ("Angle of face wrt horizontal = %.4f deg\n",thetab);
printf ("Angle of edge wrt horizontal = %.4f deg\n",thetae);
printf ("Exterior angle between perpendiculars to two adjacent faces = \
%.4f deg\n",beta);
printf ("Interior angle between two adjacent faces = \
%.4f deg\n",180.-beta);
printf ("Area of single facet = %.4f\n",farea);

volume=barea*h/3.;

puts ("\nCOMPLETE POLYCONE:");
printf ("Total area = %.4f\n",barea+ns*farea);
printf ("Total volume = %.4f\n",volume);

/* post run processing */
mexit:
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* ------------------------------------------------------------------------ */
