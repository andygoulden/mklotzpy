/* SCALC.C   M. W. Klotz   9/01, 2/03, 3/03, 4/03, 2/06, 10/06

Specialized calculators

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
/* #include <vector.h> */

/* graphics routines */
/* #include <graphics.h> */

/* file management routines */
/* #include <file.h> */

/* pmark..  functions peculiar to this program */
void ratio (void);
void parallel (void);
void rss (void);
void rms (void);
void mean (void);
dbl median (int n,dbl y[]);
void dms (void);
void deg (void);
void interp (void);
void scale (void);

/* gmark..-------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* vmark..------------ global variable declarations ----------------------- */

/* smark..----------- global structure declarations ----------------------- */

/* main..================================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
char op;
int i,j,k;
dbl sn;

/* -------------------------- initializations ------------------------------ */
textscreen ();
/* ------------------------------------------------------------------------- */

puts ("A COLLECTION OF SPECIALIZED CALCULATORS");

agin: print;
puts ("A Ratio (a/b = c/d)");
puts ("B 1/Z = 1/x1 + 1/x2 + ...");
puts ("C rss = sqrt (x1*x1 + x2*x2 + ...)");
puts ("D rms = sqrt [(x1*x1 + x2*x2 + ...)/n]");
puts ("E max, min, median, mean and standard deviation");
puts ("F deg:min:sec -> fractional degrees");
puts ("G fractional degrees -> deg:min:sec");
puts ("H screw number -> major diameter");
puts ("I interpolation");
puts ("J scaling");
puts ("Q/Esc quit");

print;
op=getkey("(A-J,Q) ? ");
print;
switch (op)
{
	case 'a':
	ratio ();
	break;

	case 'b':
	parallel ();
	break;

	case 'c':
	rss ();
	break;

	case 'd':
	rms ();
	break;

	case 'e':
	mean();
	break;

	case 'f':
	dms();
	break;

	case 'g':
	deg();
	break;

	case 'h':
	vin ("Screw number",&sn,"lf",0.,"");
	printf ("Screw diameter = %.4lf in\n",0.06+sn*0.013);
	break;

	case 'i':
	interp ();
	break;

	case 'j':
	scale ();
	break;

	case 27:
	case 'q':
	goto mexit;


}
goto agin;

/* post run processing */
mexit:
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* ratio..----------------------------------------------------------------- */

void ratio (void)

{
int k;
dbl a,b,c,d;

puts ("solves the ratio: a/b = c/d");
puts ("enter values for any three of a,b,c,d");
puts ("press return for unknown value");


agin:
k=0;
vin ("a",&a,"lf",0.,"");
if (a) k++;
if (k==3) goto done;
vin ("b",&b,"lf",0.,"");
if (b) k++;
vin ("c",&c,"lf",0.,"");
if (c) k++;
if (k==3) goto done;
vin ("d",&d,"lf",0.,"");
if (d) k++;
if (k==3) goto done;

puts ("\nInsufficient data - try again\n"); goto agin;

done:
if (a && b && c) {printf ("d = %g\n",c*b/a); return;}
if (a && b && d) {printf ("c = %g\n",d*a/b); return;}
if (a && c && d) {printf ("b = %g\n",a*d/c); return;}
if (b && c && d) {printf ("a = %g\n",b*c/d); return;}


}
/* parallel..-------------------------------------------------------------- */

void parallel (void)

{
dbl x,sum;

puts ("1/z = 1/x1 + 1/x2 + ...");
puts ("negative values of x may be used to remove a previously entered value");
puts ("press return when done");

sum=0;
agin: vin ("x",&x,"lf",0.,"");
if (x == 0.) return;
sum+=1/x;
printf ("z = %g\n",1/sum);
goto agin;
}
/* rss..------------------------------------------------------------------- */

void rss (void)

{
dbl x,sum;

puts ("rss = sqrt (x1^2 + x2^2 + ...)");
puts ("negative values of x may be used to remove a previously entered value");
puts ("press return when done");

sum=0;
agin: vin ("x",&x,"lf",0.,"");
if (x == 0.) return;
sum+=x*ABS(x);
printf ("rss = %g\n",sqrt(sum));
goto agin;
}
/* mean..------------------------------------------------------------------ */

void mean (void)

{
int n;
dbl x,sum,sum2,mean,sd,xmax,xmin;
dbl y[1000];

puts ("press return when done");
n=0; sum=sum2=sd=0; xmin=1.e98; xmax=-xmin;

agin: vin ("x",&x,"lf",0.,"");
if (x == 0.) return;
if (x > xmax) xmax=x;
if (x < xmin) xmin=x;
n++; y[n]=x; sum+=x; sum2+=x*x; mean=sum/n;
if (n>1) sd=sqrt((sum2-n*mean*mean)/(n-1));
printf ("max value = %g, min value = %g\n",xmax,xmin);
printf ("median = %g, mean = %g, standard deviation = %g\n",median(n,y),mean,sd);
goto agin;
}
/* median..---------------------------------------------------------------- */

/* 
find median by partitioning array such that y(l)...y(k-1) <= y(k)
and y(k+1)...y(r) >= y(k)
k=(n+1)\2
this is essentially a quicksort partition of the array
around the (n+1)/2 value in the array
*/

dbl median (int n,dbl y[])

{
int il,ir,i,j,k;
dbl v,t;

il=1; ir=n; k=(n+1)/2;
do
{
v=y[ir]; i=il-1; j=ir;
	do
	{
	do {i++;} while (y[i]<v);
	do {j--;} while (y[j]>v);
	t=y[i]; y[i]=y[j]; y[j]=t;
	} while (j>i);
y[j]=y[i]; y[i]=y[ir]; y[ir]=t;
if (i>=k) ir=i-1;
if (i<=k) il=i+1;
}
while (ir>il);
return y[k];
}
/* rms..------------------------------------------------------------------- */

void rms (void)

{
int n;
dbl x,sum;

puts ("rms = sqrt [(x1^2 + x2^2 + ...)/n]");
puts ("negative values of x may be used to remove a previously entered value");
puts ("press return when done");

sum=0; n=0;
agin: vin ("x",&x,"lf",0.,"");
if (x == 0.) return;
n++; sum+=x*ABS(x);
printf ("rms = %g\n",sqrt(sum/n));
goto agin;
}
/* dms..------------------------------------------------------------------- */

void dms (void)

{
char sgn;
dbl d,m,s,fd;

vin ("degrees",&d,"lf",0.,"deg");
vin ("minutes",&m,"lf",0.,"min");
vin ("seconds",&s,"lf",0.,"sec");

if (d < 0) {d=-d; sgn=-1;} else sgn=1;
while (s > 60.) {s-=60.; m+=1;}
while (m > 60.) {m-=60.; d+=1;}

fd=sgn*(d+m/60.+s/3600.);

printf ("\n%g:%g:%g d:m:s = %g deg\n",d,m,s,fd);
}
/* deg..------------------------------------------------------------------- */

void deg (void)

{
char sgn;
int deg,min,sec;
dbl theta;

vin ("angle",&theta,"lf",0.,"deg");

if (theta < 0) {sgn=-1; theta=-theta;} else sgn=1;

deg=floor(theta);
min=floor((theta-(dbl)deg)*60.);
sec=RND((theta-(dbl)deg-(dbl)min/60.)*3600.);

if (sec == 60) {min++; sec-=60;}

printf ("\n%g deg = %d:%d:%d d:m:s\n",sgn*theta,sgn*deg,min,sec);
printf ("error = %g deg\n",deg+min/60.+sec/3600.-theta);
}
/* interp..---------------------------------------------------------------- */

void interp (void)

{
char op;
dbl a;			// slope
dbl b;			// intercept
dbl X1,Y1,X2,Y2;
dbl x,y;

agin:
vin ("X1",&X1,"lf",2.3,"");
vin ("Y1",&Y1,"lf",log(X1),"");
vin ("X2",&X2,"lf",2.5,"");
vin ("Y2",&Y2,"lf",log(X2),"");

a=(Y2-Y1)/(X2-X1); b=Y1-a*X1;

vin ("x",&x,"lf",2.4,"");
y=a*x+b;

printf ("Interpolated value = %.6lf\n",y);

op=getkey("Another interpolation (Y/[N])? ");
if (op == 'y') goto agin;
}
/* scale..----------------------------------------------------------------- */

void scale (void)

{
char op;
dbl draw,real,sf,xd,xr;


vin ("Length of object on drawing",&draw,"lf",0.25,"in");
vin ("Length of real object",&real,"lf",12.,"in");

sf=draw/real;

printf ("Scale = %.4lf in/in = %.4lf in/ft\n",sf,sf*12);


agin:
vin ("Length of object on drawing (zero to quit)",&xd,"lf",0.1,"in");
if (xd == 0) return;
xr=xd/sf; printf ("Length of real object = %.4lf in\n",xr); goto agin;
}
/* ------------------------------------------------------------------------ */
