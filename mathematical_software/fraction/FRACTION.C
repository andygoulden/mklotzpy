/* FRACTION.C   M. W. Klotz   5/98, 6/03

Rational Fraction Calculator

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
#define MAX(a,b) ((a)>=(b) ? (a) : (b))
#define MIN(a,b) ((a)<=(b) ? (a) : (b))
#define ABS(a) ((a)>=0 ? (a) : -(a))
#define SGN(a) ((a)!=0 ? ((a)>0 ? 1 : -1) : 0)

/* -------------------------- function prototypes ------------------------- */

/* pmark..  functions peculiar to this program */
long gcd (long x, long y);
void pars (char *s, struct fracdat *p);
void parse (char *s, long *i, long *n, long *d);
void fmath (struct fracdat *p);

/* struc..----------- global structure declarations ----------------------- */
struct fracdat
{
long i1,n1,d1;
long i2,n2,d2;
char op;
long i3,n3,d3;
} fd;

/* vmark..------------ global variable declarations ----------------------- */

/* main..====xxxxxxxx====================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i,j,k;
char buf[80],*b;
real x;

/* -------------------------- initializations ------------------------------ */
/* ------------------------------------------------------------------------- */

if (argc > 1)
{
	*buf='\0';
	for (i=1 ; i<=argc ; i++)
		{
		strncat(buf,argv[i],strlen(argv[i]));
		strncat(buf," ",1);
		}
	goto start;
}

puts ("expression syntax: a b/c (+,-,*,\\,g(cd),l(cm)) d e/f");
agin: printf ("expression (e.g. 3 3/4 + 1 1/2) [Enter to quit] ? ");
gets (buf); if (!strlen(buf)) goto mexit;
start:
printf ("%s = ",buf);
if ((b=strpbrk (buf,"+-*\\glGL")) == NULL)
  {puts ("operator not found"); goto agin;}
fd.op=tolower(*b); *b='\0';
parse (buf,&fd.i1,&fd.n1,&fd.d1);
parse (b+1,&fd.i2,&fd.n2,&fd.d2);
fmath (&fd);
if (fd.i3 || !fd.n3) printf ("%ld ",fd.i3);
x=(real)fd.i3+(real)fd.n3/(real)fd.d3;
if (fd.n3) printf ("%ld/%ld = %f",fd.n3,fd.d3,x);
print;
goto agin;

/* post run processing */
mexit:
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* gcd..------------------------------------------------------------------- */

/* return greatest common divisor */

long gcd (long x, long y)

{
long a,b,c;
a=MIN(x,y); b=MAX(x,y); if (a == 0 && b == 0) return 1L;
do {c=b-a*floor(b/a); b=a; a=c;} while (c != 0); return b;
}
/* parse..----------------------------------------------------------------- */

/* parse a fraction expression */

void parse (char *s, long *i, long *n, long *d)

{
char *b,*x;

*i=*n=0; *d=1L;
/* remove leading and trailing blanks */
while (*s == ' ') s++; b=strchr(s,'\0')-1; while (*b == ' ') {*b--='\0';}
if (!strchr(s,'/')) {b=strchr(s,'\0'); *i=strtol(s,&b,10); return;}
if (x=strchr(s,' ')) *i=strtol(s,&x,10); else x=s;
b=strchr(x,'/'); *n=strtol(x,&b,10);
x=b+1; b=strchr(x,'\0'); *d=strtol(x,&b,10);
}
/* fmath..----------------------------------------------------------------- */

/* do the fractional mathematics */

void fmath (struct fracdat *p)

{
long g,a,b,sign=1;

p->i3=0; p->n3=0; p->d3=1;
a=p->d1*p->i1+p->n1; b=p->d2*p->i2+p->n2;
switch (p->op)
{
case '-':
	sign=-1;
case '+':
	p->n3=p->d2*a+sign*p->d1*b;
	p->d3=p->d1*p->d2;
	break;
case '*':
	p->n3=a*b;
	p->d3=p->d1*p->d2;
	break;
case '\\':
	p->n3=a*p->d2;
	p->d3=b*p->d1;
	break;
case 'g':
	p->i3=gcd(p->i1,p->i2);
	return;
case 'l':
	p->i3=p->i1*p->i2/gcd(p->i1,p->i2);
	return;
default:
	puts ("fmath error"); return;
}
/* reduce fraction */
if (p->d3 == 1L) {p->i3=p->n3; p->n3=0;}
if (p->n3 == 0) return;
sign=SGN(p->n3); p->n3=ABS(p->n3);
p->i3=p->n3/p->d3; p->n3-=p->i3*p->d3; p->i3*=sign; if (p->n3 == 0) return;
if ((g=gcd(p->n3,p->d3)) > 1L) {p->n3/=g; p->d3/=g;} p->n3*=sign;
}
/* ------------------------------------------------------------------------ */
