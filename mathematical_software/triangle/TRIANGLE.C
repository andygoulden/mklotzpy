/* TRIANGLE.C   M. W. Klotz  11/98,3/99, 4/11

Solve plane triangles

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
int solve (void);
void disp (void);
void sss (dbl *s1, dbl *s2, dbl *s3, dbl *a1, dbl *a2, dbl *a3);
void asa (dbl *s1, dbl *s2, dbl *s3, dbl *a1, dbl *a2, dbl *a3);
void saa (dbl *s1, dbl *s2, dbl *s3, dbl *a1, dbl *a2, dbl *a3);
void sas (dbl *s1, dbl *s2, dbl *s3, dbl *a1, dbl *a2, dbl *a3);
void ssa ( dbl *s1, dbl *s2, dbl *s3, dbl *a1, dbl *a2, dbl *a3,\
  dbl *s1p, dbl *s2p, dbl *s3p, dbl *a1p, dbl *a2p, dbl *a3p);

/* gmark..-------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* vmark..------------ global variable declarations ----------------------- */
dbl s1,s2,s3,s1p,s2p,s3p;	/* sides */
dbl a1,a2,a3,a1p,a2p,a3p;	/* angles */
dbl s;
int sol;			/* number of solutions */

/* smark..----------- global structure declarations ----------------------- */

/* main..================================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i,j,k;
dbl r3;

/* -------------------------- initializations ------------------------------ */
textscreen ();
/* ------------------------------------------------------------------------- */

s1=s2=s3=0.; a1=a2=a3=0;
puts ("SOLUTION OF PLANE TRIANGLES\n");
puts
 ("Number the sides of your triangle 1-3 and answer the questions to follow.");
puts ("Input whatever data you know; press return if not known.");
puts ("You need three data items (at least one side) to solve a triangle.\n");
k=0;
vin ("side 1",&s1,"lf",0.,""); if (s1) k++;
vin ("side 2",&s2,"lf",0.,""); if (s2) k++;
vin ("side 3",&s3,"lf",0.,""); if (s3) k++;
if (!k) {beep; puts ("\nAT LEAST ONE SIDE MUST BE SPECIFIED"); goto mexit;}
if (k == 3) goto doit;
vin ("angle opposite side 1",&a1,"lf",0.,"deg"); if (a1) k++;
if (k == 3) goto doit;
vin ("angle opposite side 2",&a2,"lf",0.,"deg"); if (a2) k++;
if (k == 3) goto doit;
vin ("angle opposite side 3",&a3,"lf",0.,"deg");
if (a3) k++;
doit:
if (solve ()) {beep; puts ("\nINSUFFICIENT DATA FOR SOLUTION"); goto mexit;}
disp ();

/* post run processing */
mexit:
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* disp..------------------------------------------------------------------ */

/* display solution */

void disp (void)

{
#define x(a,b) (((ABS(a-b))<1.e-6) ? 1 : 0)

printf ("\nside  1 = %g\n",s1);
printf ("side  2 = %g\n",s2);
printf ("side  3 = %g\n",s3);
s=0.5*(s1+s2+s3);
printf ("angle opposite side 1 = %g deg\n",a1);
printf ("angle opposite side 2 = %g deg\n",a2);
printf ("angle opposite side 3 = %g deg\n",a3);
printf ("area of triangle = %g\n",.5*s1*s2*SIND(a3));
printf ("circumscribed circle radius = %g\n",0.5*s1/SIND(a1));
printf ("inscribed circle radius = %g\n",sqrt((s-s1)*(s-s2)*(s-s3)/s));
if (sol == 1) return;
if (x(s1,s1p) && x(s2,s2p) && x(s3,s3p) && x(a1,a1p) && x(a2,a2p) && x(a3,a3p))
 return;
puts ("AN ALTERNATE SOLUTION EXISTS:");
printf ("side  1 = %g\n",s1p);
printf ("side  2 = %g\n",s2p);
printf ("side  3 = %g\n",s3p);
s=0.5*(s1p+s2p+s3p);
printf ("angle opposite side 1 = %g deg\n",a1p);
printf ("angle opposite side 2 = %g deg\n",a2p);
printf ("angle opposite side 3 = %g deg\n",a3p);
printf ("area of triangle = %g\n",.5*s1p*s2p*SIND(a3p));
printf ("circumscribed circle radius = %g\n",0.5*s1p/SIND(a1p));
printf ("inscribed circle radius = %g\n",sqrt((s-s1p)*(s-s2p)*(s-s3p)/s));
}
/* sss..------------------------------------------------------------------- */

void sss (dbl *s1, dbl *s2, dbl *s3, dbl *a1, dbl *a2, dbl *a3)

{
dbl p;

p=.5*(*s1+*s2+*s3);
*a1=2.*ACSD(sqrt(p*(p-*s1)/(*s2**s3)));
*a2=2.*ACSD(sqrt(p*(p-*s2)/(*s1**s3)));
*a3=180.-*a1-*a2;
}
/* asa..------------------------------------------------------------------- */

void asa (dbl *s1, dbl *s2, dbl *s3, dbl *a1, dbl *a2, dbl *a3)

{
*a1=180.-*a2-*a3;
*s2=*s1*SIND(*a2)/SIND(*a1);
*s3=*s1*COSD(*a2)+*s2*COSD(*a1);
}
/* saa..------------------------------------------------------------------- */

void saa (dbl *s1, dbl *s2, dbl *s3, dbl *a1, dbl *a2, dbl *a3)

{
*a2=180.-*a1-*a3;
*s2=*s1*SIND(*a2)/SIND(*a1);
*s3=*s1*COSD(*a2)+*s2*COSD(*a1);
}
/* sas..------------------------------------------------------------------- */

void sas (dbl *s1, dbl *s2, dbl *s3, dbl *a1, dbl *a2, dbl *a3)

{
dbl p;

*s3=sqrt(*s1**s1+*s2**s2-2.**s1**s2*COSD(*a3));
p=.5*(*s1+*s2+*s3);
*a1=2.*ACSD(sqrt(p*(p-*s1)/(*s2**s3)));
*a2=2.*ACSD(sqrt(p*(p-*s2)/(*s1**s3)));
}
/* ssa.-------------------------------------------------------------------- */

void ssa ( dbl *s1, dbl *s2, dbl *s3, dbl *a1, dbl *a2, dbl *a3,\
  dbl *s1p, dbl *s2p, dbl *s3p, dbl *a1p, dbl *a2p, dbl *a3p)

{
*a2=ASND((*s2)*SIND(*a1)/(*s1));
*a3=180.-*a1-*a2;
*s3=*s1*COSD(*a2)+*s2*COSD(*a1);
if (*s2 > *s1 && *a2 != 90.)
	{
	sol=2;
	*s1p=*s1; *s2p=*s2; *a1p=*a1;
	*a2p=180.-*a2;
	*a3p=180.-*a1-*a2p;
	*s3p=*s1*COSD(*a2p)+*s2*COSD(*a1);
	}
}
/* solve..----------------------------------------------------------------- */

/* solve plane triangle */

int solve (void)

{

sol=1;
s1p=s2p=s3p=0.; a1p=a2p=a3p=0.;
if (s1 && s2 && s3) {sss (&s1,&s2,&s3,&a1,&a2,&a3); return 0;}	/* sss */

if (s1 && a2 && a3) {asa (&s1,&s2,&s3,&a1,&a2,&a3); return 0;}	/* asa */
if (s2 && a1 && a3) {asa (&s2,&s3,&s1,&a2,&a3,&a1); return 0;}	/* asa */
if (s3 && a1 && a2) {asa (&s3,&s1,&s2,&a3,&a1,&a2); return 0;}	/* asa */

if (s1 && a1 && a3) {saa (&s1,&s2,&s3,&a1,&a2,&a3); return 0;}	/* saa */
if (s1 && a1 && a2) {saa (&s1,&s3,&s2,&a1,&a3,&a2); return 0;}	/* saa */
if (s2 && a1 && a2) {saa (&s2,&s3,&s1,&a2,&a3,&a1); return 0;}	/* saa */
if (s2 && a2 && a3) {saa (&s2,&s1,&s3,&a2,&a1,&a3); return 0;}	/* saa */
if (s3 && a2 && a3) {saa (&s3,&s1,&s2,&a3,&a1,&a2); return 0;}	/* saa */
if (s3 && a1 && a3) {saa (&s3,&s2,&s1,&a3,&a2,&a1); return 0;}	/* saa */

if (s1 && s2 && a3) {sas (&s1,&s2,&s3,&a1,&a2,&a3); return 0;}	/* sas */
if (s2 && s3 && a1) {sas (&s2,&s3,&s1,&a2,&a3,&a1); return 0;}	/* sas */
if (s1 && s3 && a2) {sas (&s3,&s1,&s2,&a3,&a1,&a2); return 0;}	/* sas */

if (s1 && s2 && a1)
{
ssa (&s1,&s2,&s3,&a1,&a2,&a3,&s1p,&s2p,&s3p,&a1p,&a2p,&a3p);	/* ssa */
return 0;
}
if (s1 && s2 && a2)
{
ssa (&s2,&s1,&s3,&a2,&a1,&a3,&s2p,&s1p,&s3p,&a2p,&a1p,&a3p);	/* ssa */
return 0;
}
if (s2 && s3 && a2)
{
ssa (&s2,&s3,&s1,&a2,&a3,&a1,&s2p,&s3p,&s1p,&a2p,&a3p,&a1p);	/* ssa */
return 0;
}
if (s2 && s3 && a3)
{
ssa (&s3,&s2,&s1,&a3,&a2,&a1,&s3p,&s2p,&s1p,&a3p,&a2p,&a1p);	/* ssa */
return 0;
}
if (s1 && s3 && a3)
{
ssa (&s3,&s1,&s2,&a3,&a1,&a2,&s3p,&s1p,&s2p,&a3p,&a1p,&a2p);	/* ssa */
return 0;
}
if (s1 && s3 && a1)
{
ssa (&s1,&s3,&s2,&a1,&a3,&a2,&s1p,&s3p,&s2p,&a1p,&a3p,&a2p);	/* ssa */
return 0;
}
return 1;		/* no solution found */
}
/* ------------------------------------------------------------------------ */
