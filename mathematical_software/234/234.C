/* 234.C   M. W. Klotz   2/01

Solve quadratic, cubic and quartic equations with real coefficients.

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
void psol (struct eq *p);
void quadratic (struct eq *p);
void cubic (struct eq *s);
void quartic (struct eq *s);

/* gmark..-------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* vmark..------------ global variable declarations ----------------------- */
int order;		// order of equation to solve

/* smark..----------- global structure declarations ----------------------- */

struct eq {
dbl c[5];		// coefficients
dbl rr[4],ir[4];	// real and imaginary components of roots
} e1,e2;

struct eq *ep;

/* main..================================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i,j,k;

/* -------------------------- initializations ------------------------------ */
textscreen ();
/* ------------------------------------------------------------------------- */

agin:
vin ("Order of equation to solve (2,3,4)",&order,"d",2.,"");
if (order<2 || order>4) {beep; puts ("NOT A VALID OPTION"); goto agin;}
print;

ep=&e1;
for (i=order ; i>=0 ; i--)
{
printf ("For order %d term, ",i);
vin ("input coefficient",&(ep->c[i]),"lf",0.,"");
}

switch (order)
{
	case 2:
		quadratic (&e1);
		psol (&e1);
		break;
	case 3:
		cubic (&e1);
		psol (&e1);
		break;
	case 4:
		quartic (&e1);
		psol (&e1);
		break;
}


/* post run processing */
mexit:
fcloseall();
exit(0);			/* normal exit */
}
/* ======================================================================== */

/* psol..------------------------------------------------------------------ */

/* print the solution */

void psol (struct eq *p)

{
int i;

puts ("\nSolutions to:");

for (i=order ; i>=0 ; i--)
{
if (i!=0) printf (" %+.4lf * x^%1d",p->c[i],i);
else printf (" %+.4lf",p->c[i]);
}
printf (" = 0\nare:\n");

for (i=0 ; i<order ; i++)
{
printf ("real:  %lf  imaginary:  %lf\n",p->rr[i],p->ir[i]);
}
}
/* quadratic..------------------------------------------------------------- */

/* solve quadratic equation */

void quadratic (struct eq *p)

{
dbl a,b,c,d,e;

a=p->c[2]; b=p->c[1]; c=p->c[0]; d=b*b-4.0*a*c; e=sqrt(ABS(d));

if (d>=0)		// two real roots
{
p->rr[0]=(-b+e)/(2.*a); p->ir[0]=0.;
p->rr[1]=(-b-e)/(2.*a); p->ir[1]=0.;
}
else			// two complex unequal roots
{
p->rr[0]=-b/(2.*a); p->ir[0]=e/(2.*a);
p->rr[1]=-b/(2.*a); p->ir[1]=-e/(2.*a);
}
}
/* cubic..----------------------------------------------------------------- */

void cubic (struct eq *s)

{
dbl a,b,d,x,p,q,r,phi,t,aa,bb,u,o3;

x=s->c[3]; p=(s->c[2])/x; q=(s->c[1])/x; r=(s->c[0])/x;
a=(3.*q-p*p)/3.; b=(2.*p*p*p-9.*p*q+27.*r)/27.;
d=b*b/4.+a*a*a/27.; u=p/3.; o3=1./3.;

if (d<0)	//three real and unequal roots
{
phi=ACSD(-0.5*b/sqrt(-a*a*a/27.));
t=2.*sqrt(-a/3.); x=phi/3.;
s->rr[0]=t*COSD(x)-u; s->ir[0]=0.;
s->rr[1]=t*COSD(x+120.)-u; s->ir[1]=0.;
s->rr[2]=t*COSD(x+240.)-u; s->ir[2]=0.;
return;
}
else		// one real and two conjugate roots
		// or three real roots of which two are equal
{
x=sqrt(d); 
t=-0.5*b+x; aa=SGN(t)*pow(ABS(t),o3);
t=-0.5*b-x; bb=SGN(t)*pow(ABS(t),o3);
s->rr[0]=aa+bb-u; s->ir[0]=0.;
s->rr[1]=-0.5*(aa+bb)-u; s->ir[1]=0.5*(aa-bb)*sqrt(3.);
s->rr[2]=-0.5*(aa+bb)-u; s->ir[2]=-0.5*(aa-bb)*sqrt(3.);
return;
}
}
/* quartic..--------------------------------------------------------------- */

/* solve quartic equation */

void quartic (struct eq *s)

{
int i,k;
dbl x,a,b,c,d,R,D,E,y,rr,dd,ee;


x=s->c[4]; a=(s->c[3])/x; b=(s->c[2])/x; c=(s->c[1])/x; d=(s->c[0]/x);
ep=&e2;
ep->c[3]=1.;
ep->c[2]=-b;
ep->c[1]=a*c-4*d;
ep->c[0]=-a*a*d+4.*b*d-c*c;

cubic (ep);		// solve resultant cubic
/* psol (ep);		// debug */


/*
find a (real) solution to the cubic that makes rr (= R*R) >=0 so R is real
haven't explored the case(s) where R might be imaginary
*/
k=-1;
agin: k++;
if (k>2) {puts("no usable real solution to resultant cubic"); return;}
if (ep->ir[k]) goto agin;
y=ep->rr[k]; rr=(a*a/4.-b+y);
if (rr < 0.) goto agin;

R=sqrt(ABS(rr));

if (R==0)
{
dd=(0.75*a*a-2.*b+2*sqrt(y*y-4.*d));
ee=(0.75*a*a-2.*b-2*sqrt(y*y-4.*d));
}
else
{

dd=(0.75*a*a-rr-2.*b+(4.*a*b-8.*c-a*a*a)/(4.*R));
ee=(0.75*a*a-rr-2.*b-(4.*a*b-8.*c-a*a*a)/(4.*R));
}
D=sqrt(ABS(dd));
E=sqrt(ABS(ee));

for (i=0 ; i<4 ; i++) s->rr[i]=-0.25*a;

if (rr >= 0.)
{
s->rr[0]+=0.5*R;
s->rr[1]+=0.5*R;
s->rr[2]-=0.5*R;
s->rr[3]-=0.5*R;
}
else
{
s->ir[0]+=0.5*R;
s->ir[1]+=0.5*R;
s->ir[2]-=0.5*R;
s->ir[3]-=0.5*R;
}

if (dd>=0)
{
s->rr[0]+=0.5*D;
s->rr[1]-=0.5*D;
}
else
{
s->ir[0]+=0.5*D;
s->ir[1]-=0.5*D;
}

if (ee>=0)
{
s->rr[2]+=0.5*E;
s->rr[3]-=0.5*E;
}
else
{
s->ir[2]+=0.5*E;
s->ir[3]-=0.5*E;
}
}
/* ------------------------------------------------------------------------ */
