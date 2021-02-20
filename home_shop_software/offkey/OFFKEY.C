/* OFFKEY.C   M. W. Klotz  1/03

Calculations for offset keys

Written for Ronnie Shultz (Backcreekeng@aol.com)

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
void rel (int i, int j);
void arel (int j, int i);
void intersect (dbl x1, dbl y1, dbl x2, dbl y2, dbl x3, dbl y3, dbl x4, dbl y4,\
dbl *xi, dbl *yi);

/* gmark..-------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* vmark..------------ global variable declarations ----------------------- */

/* smark..----------- global structure declarations ----------------------- */

struct point {
dbl x;
dbl y;
} b[8],a[10];

/* main..================================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i;
dbl d,h,k,x,y,m,n,r,q,theta,ct,st,x1,y1,x2,y2,x3,y3,x4,y4,maxang;

/* -------------------------- initializations ------------------------------ */
/* ------------------------------------------------------------------------- */

puts ("OFFSET KEY CALCULATIONS\n");

puts ("Shaft Diameter   Recommended Key Size\n");
puts ("0.3125-0.4375    0.0625-0.1250");
puts ("0.4375-0.5000    0.0625-0.1562");
puts ("0.5000-0.8750    0.0938-0.3125");
puts ("0.8750-1.1250    0.1250-0.4375");
puts ("1.1250-1.1500    0.1562-0.5000");
puts ("1.5000-2.0000    0.1875-0.5000");
print;
vin ("Shaft diameter",&d,"lf",1.0,"");
vin ("Key width",&k,"lf",0.5,"");
vin ("Key height",&h,"lf",0.5,"");

x=sqrt(d*d-k*k); y=0.5*(d-x); m=d-(y+0.5*h); n=m+h; q=d-m; r=h-q;

print;
printf ("Shaft diameter (D)         = %.4lf\n",d);
printf ("Key width (K)              = %.4lf\n",k);
printf ("Key height (H)             = %.4lf\n",h);
printf ("Keyseat depth (M)          = %.4lf\n",m);
printf ("Keyway depth (N)           = %.4lf\n",n);
printf ("Depth of cut in shaft (Q)  = %.4lf\n",q);
printf ("Depth of cut in hub (R)    = %.4lf\n",r);
print;
puts("Keyseat depth measured bottom of keyseat to opposite side of shaft on cl.");
puts("Keyway depth measured top of hub keyway to opposide side of hole on cl.");
print;

vin ("Depth of cut in shaft (Q)",&q,"%.4lf",q,"");
r=h-q;
maxang=2*ASND(k/d);
agin:
printf ("Maximum possible rotation angle = %.4lf deg\n",maxang);
vin ("Key rotation angle (theta)",&theta,"lf",5.,"deg");
theta=ABS(theta);
if (theta > maxang) {beep; puts ("ANGLE TOO BIG.  TRY AGAIN."); goto agin;}
ct=COSD(theta); st=SIND(theta);
print;

b[0].x=0; b[0].y=0.5*d-q;
b[1].x=0.5*k; b[1].y=b[0].y;
b[2].x=b[1].x; b[2].y=0.5*sqrt(d*d-k*k);
b[3].x=b[1].x; b[3].y=0.5*d+r;
b[4].x=0; b[4].y=b[3].y;
b[5].x=-b[3].x; b[5].y=b[3].y;
b[6].x=-b[2].x; b[6].y=b[2].y;
b[7].x=-b[1].x; b[7].y=b[1].y;

puts ("x,y locations of points (b)efore => (a)fter rotation\n");
for (i=0 ; i<8 ; i++)
{

if (i>1 && i<7)
{
a[i].x=ct*b[i].x+st*b[i].y;
a[i].y=-st*b[i].x+ct*b[i].y;
}
else {a[i].x=b[i].x; a[i].y=b[i].y;}

printf ("b[%d] = %.4lf, %.4lf => a[%d] = %.4lf, %.4lf\n",i,b[i].x,b[i].y,\
i,a[i].x,a[i].y);
}

x1=b[6].x; y1=b[6].y;		// point 8
x2=b[2].x; y2=b[2].y;
x3=a[6].x; y3=a[6].y;
x4=a[5].x; y4=a[5].y;
intersect (x1,y1,x2,y2,x3,y3,x4,y4,&a[8].x,&a[8].y);

x1=b[6].x; y1=b[6].y;		// point 9
x2=b[2].x; y2=b[2].y;
x3=a[2].x; y3=a[2].y;
x4=a[3].x; y4=a[3].y;
intersect (x1,y1,x2,y2,x3,y3,x4,y4,&a[9].x,&a[9].y);

for (i=8 ; i<10 ; i++) printf ("a[%d] = %.4lf, %.4lf\n",i,a[i].x,a[i].y);

puts ("\nrelative locations\n");
rel(6,6); rel(2,2); rel(3,7); rel (5,7); rel(8,6); rel (9,2); arel (5,8);

/* post run processing */
mexit:
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* rel..------------------------------------------------------------------- */

/* print relative spacing */

void rel (int j, int i)

{
dbl x,y,r;

x=a[j].x-b[i].x;
y=a[j].y-b[i].y;
r=sqrt(x*x+y*y);

printf ("a[%d] wrt b[%d]:  ",j,i);
printf ("x sep = %.4lf, y sep = %.4lf, sep = %.4lf\n",x,y,r);
}
/* arel..------------------------------------------------------------------ */

/* print relative spacing */

void arel (int j, int i)

{
dbl x,y,r;

x=a[j].x-a[i].x;
y=a[j].y-a[i].y;
r=sqrt(x*x+y*y);

printf ("a[%d] wrt a[%d]:  ",j,i);
printf ("x sep = %.4lf, y sep = %.4lf, sep = %.4lf\n",x,y,r);
}
/* intersect..------------------------------------------------------------- */

/* intersection of two straight line segments */

void intersect (dbl x1, dbl y1, dbl x2, dbl y2, dbl x3, dbl y3, dbl x4, dbl y4,\
dbl *xi, dbl *yi)

{
dbl d;
d=(y2-y1)*(x4-x3)-(y4-y3)*(x2-x1);
*xi=((x1*y2-x2*y1)*(x4-x3)-(x3*y4-x4*y3)*(x2-x1))/d;
*yi=((x1*y2-x2*y1)*(y4-y3)-(x3*y4-x4*y3)*(y2-y1))/d;
}
/* ------------------------------------------------------------------------ */
