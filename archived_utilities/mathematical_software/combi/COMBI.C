/* COMBI.C   M. W. Klotz   08/01

Enumeration of all combinations of N things taken M at a time

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
void combo (void);

/* gmark..-------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* vmark..------------ global variable declarations ----------------------- */
#define nmax 52
#define mmax 52
int n,m;
long ncom;		/* number of combinations */
char c[nmax];		/* used in enumeration */
char alpha []=".ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
FILE *fp;
char ofile[]="COMBI.OUT";

/* smark..----------- global structure declarations ----------------------- */

/* main..================================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i,j,k;

/* -------------------------- initializations ------------------------------ */
/* ------------------------------------------------------------------------- */

if ((fp=fopen(ofile,"wt")) == NULL)
	{printf ("FAILED TO OPEN OUTPUT FILE: %s\n",ofile); quit;}

printf ("Combinations of N(<=%d) things taken M(<=%d) at a time\n\n",nmax,mmax);

agin1: vin ("Number of things",&n,"d",6.,"");
if (n>nmax) {beep; printf ("N <= %d\n",nmax); goto agin1;}
agin2: vin ("Taken m at a time",&m,"d",4.,"");
if (m>mmax) {beep; printf ("M <= %d\n",mmax); goto agin2;}
if (m>=n) {beep; printf ("M < %d\n",n); goto agin2;}

fprintf (fp,"%d things taken %d at a time\n",n,m);

for (i=1 ; i<=m ; i++) c[i]=1;

for (i=m+1,ncom=1 ; i<=n ; i++) ncom*=i;
for (i=1 ; i<=n-m ; i++) ncom/=i;
fprintf (fp,"Number of combinations = %ld\n",ncom);


for (i=1 ; i<=ncom ; i++)
{
fprintf (fp,"%d: ",i);
combo();
for (j=1 ; j<=n ; j++) {if (c[j] != 0) fprintf (fp,"%c",alpha[j]);}
fprintf (fp,"\n");
}

/* post run processing */
mexit:
fcloseall();
printf ("\nYour data is on: %s\n",ofile);
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* ------------------------------------------------------------------------ */
void combo (void)

{
int i,j,nj,i1,j1,jp,k1,k2;

for (j=1 ; j<=n-1 ; j++)
{
nj=n-j;
if (c[n] != c[nj]) {j1=j; break;}
}
if (fmod((dbl)m,2) != 0) goto modd;

/* m is even */

if (c[n] != 1) {k1=n-j1; k2=k1+1; goto comp;}
if (fmod((dbl)j1,2) == 0) goto c120;

/* scan from right to left */

scan: jp=n-j1-1;
for (i=1 ; i<=jp ; i++)
{
i1=jp+2-i;
if (c[i1] == 0) continue;
if (c[i1-1] == 1) {k1=i1-1; k2=n-j1; goto comp;}
  else {k1=i1-1; k2=n+1-j1; goto comp;}
}
k1=1; k2=n+1-m; goto comp;

/* m is odd */

modd: if (c[n] == 1) goto c110;
k2=n-j1-1; if (k2 == 0) {k1=1; k2=n+1-m; goto comp;}
if ((c[k2+1] == 1) && (c[k2] == 1)) {k1=n; goto comp;}
k1=k2+1; goto comp;
c110: if (fmod((dbl)j1,2) == 0) goto scan;
c120: k1=n-j1; k2=MIN(k1+2,n);

/* complement two bits to obtain next combination */

comp: c[k1]=1-c[k1]; c[k2]=1-c[k2]; return;
}
/* ------------------------------------------------------------------------ */
