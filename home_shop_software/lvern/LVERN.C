/* LVERN.C   M. W. Klotz   11/98

Linear vernier computations

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
#define FRAC(a) ((a)-floor(a))		/* or use modf function */
	/* see also split() in C:\CCC\GOODIES.TXT */
#define MAX(a,b) ((a)>=(b) ? (a) : (b))
#define MIN(a,b) ((a)<=(b) ? (a) : (b))
#define ABS(a) ((a)>=0 ? (a) : -(a))
#define SGN(a) ((a)!=0 ? ((a)>0 ? 1 : -1) : 0)

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
dbl dfinput (char *prompt, dbl def);
void frac (dbl q);
char *trim (char *s);

/* gmark..-------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* vmark..------------ global variable declarations ----------------------- */
dbl dm,del,xlm,xnv,tdv,xlv,dv;
int nm,nv;

/* smark..----------- global structure declarations ----------------------- */

/* main..================================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i,j,k;
dbl x;

/* -------------------------- initializations ------------------------------ */
textscreen ();
/* ------------------------------------------------------------------------- */

puts ("VERNIER DESIGN");

dm=dfinput ("distance spanned by one major division on main scale",1.0);
vin ("number of subdivisions of one major division on main scale",&nm,"d",8.,"");
del=dfinput ("distance measured by one division on vernier scale",1./32.);
frac (del);
xlm=dm/nm; xnv=xlm/del; nv=floor(xnv);
if (nv != xnv)
  {puts ("\nnumber of vernier subdivisions not an integer"); goto mexit;}
printf ("\ndistance spanned by one division on main scale = %.4lf\n",xlm);
frac (xlm);
printf ("number of subdivisions required on vernier scale = %d\n",nv);
tdv=dm-xlm;
printf ("\na typical vernier scale would span a distance of %.4lf\n",tdv);
vin ("distance spanned by vernier scale",&tdv,"lf",tdv,"");

xlv=tdv/nv;
printf ("\ndistance spanned by one division on vernier scale = %.4lf\n",xlv);
frac (xlv);

/* post run processing */
mexit:
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* dfinput..--------------------------------------------------------------- */

/* input a number in either decimal or fractional form */

dbl dfinput (char *prompt, dbl def)

{
char temp[81],*k,*m;
dbl v;

printf ("\ndefault = %.6lf\n",def);
printf ("{[d].dd or [d-]d/d (e.g. 1.5 or 1-1/2)}\n");
printf ("%s ? ",prompt);
gets (temp);
if (!strlen(temp)) return def;
if (k=strchr(temp,'/'))
{
if (m=strchr(temp,'-')) {v=atof(strtok(temp,"-")); m++;} else {v=0; m=temp;}
v+=atof(trim(strtok(m,"/")))/atof(trim(strtok(NULL,"/")));
}
else v=atof(temp);
return v;
}
/* trim..------------------------------------------------------------------ */

/* trim leading and trailing white space from a string */
/* returns pointer to trimmed string */

char *trim (char *s)
{
int l;
char *b;

l=strlen(s); l--;		/* truncate trailing white space */
while (isspace(*(s+l))) {l--;}
*(s+l+1)='\0';
b=s;				/* find first non-whitespace character */
while (isspace(*b)) {b++;}
return b;
}
/* frac..------------------------------------------------------------------ */

/* 
CONVERT DECIMAL FRACTIONS TO RATIONAL FRACTIONS USING THE ALGORITHM:
 An=Xn*An-1+An-2 WHERE:  X=INT(1/Fn-1) : Fn=FRAC(1/Fn-1)
*/

void frac (dbl q)

{
dbl f,a1,a2,an,x,z;

f=q; a1=1; a2=0;
while (ABS(f) > 1.e-6) {z=1./f; x=floor(z); f=z-x; an=x*a1+a2; a2=a1; a1=an;}
z=q*an; a1=0; while (z > an) {z-=an; a1++;}
if (a1) printf ("Nearest Rational Fraction = %.0lf-%.0lf/%.0lf\n",a1,z,an);
else printf ("Nearest Rational Fraction = %.0lf/%.0lf\n",z,an);
}
/* ------------------------------------------------------------------------ */
