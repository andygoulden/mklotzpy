/* MTAPER.C   M. W. Klotz  1/00, 5/03

Measuring conical parts with a V-block and sine bar

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
char getkey (char *prompt);
int kp (dbl x, int k);
void blk81 (dbl stack);

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
dbl A,B,C,D,t,d,L,h;

/* -------------------------- initializations ------------------------------ */
textscreen ();
/* ------------------------------------------------------------------------- */

puts ("CONICAL PART TAPER MEASUREMENT\n");

vin ("V-block included angle (D)",&D,"lf",90.,"deg");
vin ("Sine bar length (L)",&L,"lf",5.,"in");

agin:
puts ("\n1  Compute part included angle given sine bar stack height");
puts ("2  Compute sine bar stack height given part included angle");
puts ("Q  Quit");

op=getkey("(1,2,Q) ? ");
switch (op)
{
	case '1':	/* part angle given stack */
		vin ("Stack Height",&h,"lf",0.9947,"in");
		B=ASND(h/L);
		d=COSD(B)+1/SIND(D/2);
		t=SIND(B)/d;
		A=2*ATAND(t);
		C=ASND(SIND(A/2)/SIND(D/2));
		break;
	case '2':	/* stack given part angle */
		vin ("Part included angle",&A,"lf",9.5,"deg");
		C=ASND(SIND(A/2)/SIND(D/2));
		B=C+A/2;
		h=L*SIND(B);
		break;
	case 27:	/* quit */
	case 'q':
		goto mexit;
	default:
		beep; puts ("NOT A VALID OPTION");
		goto agin;
}
printf ("\nSine bar length = %.4lf in\n",L);
printf ("Sine bar stack height = %.4lf in\n",h);
printf ("Sine bar angle = %.4lf deg\n",B);
printf ("Taper included angle (A) = %.4lf deg\n",A);
printf ("Taper half angle (A/2) = %.4lf deg\n",A/2);
printf ("Angle C = %.4lf deg\n",C);
if (op == '2') blk81 (h);
goto agin;

/* post run processing */
mexit:
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* getkey..---------------------------------------------------------------- */

/* prompt and wait for a character from the user */

char getkey (char *prompt)
{
char c;
printf ("%s",prompt); c=getch(); printf ("\n"); return tolower(c);
}
/* kp..-------------------------------------------------------------------- */

/*
return kth decimal place of a non-integerial number
k=1 yields first place kp(123.406,1) = 4
if x=123.456 then kp (x,k) yields:
k	kp
4	0
3	6
2	5
1	4
0	0	decimal point always returns zero
-1	3
-2	2
-3	1
-4	0
*/

int kp (dbl x, int k)

{
char temp [40],buf[4];
int i;

if (k == 0) return 0;
sprintf (temp,"%-+20.9f",x);
i=0; while (temp[i] != '.') i++;
buf[0]=temp[k+i]; buf[1]='\0'; return atoi(buf);
}
/* blk81..----------------------------------------------------------------- */

void blk81 (dbl stack)

{
int i,j,k,l;
dbl r,t,x;

printf ("\nBlocks from standard 81 gage block set needed\
 to form stack = %.4lf in:\n\n",stack);

k=(int)1.e4*stack;
x=k*1.e-4;

if (x < 0.05)
  {
  beep;
  puts ("required size smaller than smallest available block (0.05)\n");
  goto error;
  }

r=x;			/* remaining size of spacer */

k=kp(r,4);		/* remove tenths */
if (k)
{
t=0.1000+k*0.0001;
r-=t;
printf ("block = %.4lf  remainder = %.4lf\n",t,r);
if (ABS(r) < 1.0e-5) return;
if (r < 0.04999) goto error;
}

k=kp(r,3);		/* remove thousandths */
j=kp(r,2);
if (k)
{
i=j; if (j >= 5) i=j-5;
t=0.100+i*0.01+k*0.001;
r-=t;
printf ("block = %.4lf  remainder = %.4lf\n",t,r);
if (ABS(r) < 1.0e-5) return;
if (r < 0.04999) goto error;
}

k=kp(r,2);		/* remove remainder to right of decimal point */
j=kp(r,1);
if (j || k)
{
t=0.100*j+k*0.01;
r-=t;
printf ("block = %.4lf  remainder = %.4lf\n",t,r);
if (ABS(r) < 1.0e-5) return;
if (r < 0.04999) goto error;
}

l=floor(r+1.0e-5);	/* remove integer portion */
if (l)
{
	for (i=4 ; i>0 ; i--)
	{
	if (i<=l)
		{
		t=i; l-=i;
		r-=t;
		printf ("block = %.4lf  remainder = %.4lf\n",t,r);
		if (ABS(r) < 1.0e-5) return;
		if (r < 0.04999) goto error;
		}
	}
}
if (ABS(r) < 1.0e-5) return;

error: puts ("\nCAN'T CONTINUE\n");
}
/* ------------------------------------------------------------------------ */
