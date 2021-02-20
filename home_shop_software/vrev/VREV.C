/* VREV.C   M. W. Klotz   11/05

Calculations for various sized holes arranged on bolt circle

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
char rdata (void);
dbl func (dbl R);
dbl decsearch (dbl l, dbl r, dbl val, dbl tol);

/* gmark..-------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* vmark..------------ global variable declarations ----------------------- */
FILE *fpi;			// input file pointer
char dfile[]="VREV.DAT";	// input file
FILE *fpo;		// output file pointer
char ofile[]="VREV.OUT";	//output file

#define NH 100		// maximum number of holes
int nh;			// actual number of holes
dbl dhole [NH];		// storage for hole diameters

dbl space;		// hole-to-hole spacing
dbl bcd;		// bolt circle diameter
dbl hdmin,hdmax;	// minimum/maximum hole diameters
dbl theta,x,y,r,phi,tang;

/* smark..----------- global structure declarations ----------------------- */

/* main..================================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i,j,k;

/* -------------------------- initializations ------------------------------ */
textscreen ();
/* ------------------------------------------------------------------------- */

if (rdata ()) goto mexit;		/* read the data file */
if ((fpo=fopen(ofile,"wt")) == NULL)
	{printf ("FAILED TO OPEN OUTPUT FILE: %s\n",ofile); beep; goto mexit;}

printf ("%d hole diameters read from data file\n\n",nh);

hdmin=1.e9; hdmax=1.e-9;
for (i=0 ; i<nh ; i++)
{
if (dhole[i] < hdmin) hdmin=dhole[i];
if (dhole[i] > hdmax) hdmax=dhole[i];
/* printf ("%d:  %.6lf\n",i,dhole[i]); */
}

printf ("Minimum hole size = %.4lf\n",hdmin);
printf ("Maximum hole size = %.4lf\n",hdmax);


vin ("Desired hole-to-hole spacing",&space,"lf",0.1,"");

x=2.*decsearch (0.01,1000.,0.,0.0001);

printf ("Minimum bolt circle diameter = %.4lf\n",x);


agin:
vin ("Desired bolt circle diameter",&bcd,"lf",x,"");
if (bcd < x)
{
printf ("bcd must be >= %.4lf\n",x);
puts ("Try again, dummy."); beep; goto agin;
}

r=0.5*bcd;
for (i=0,tang=0. ; i<nh ; i++) tang+=2.*ASND(.5*dhole[i]/r);
space=r*RPD*(360.-tang)/nh;

printf ("For a bcd = %.4lf, the hole-to-hole spacing will be %.4lf\n",bcd,space);

fprintf (fpo,"Bolt circle diameter = %.4lf\n",bcd);
fprintf (fpo,"Hole-to-hole spacing = %.4lf\n\n",space);
fprintf (fpo,"N, hole diameter, x, y, angle(deg))\n\n");
phi=DPR*space/r;
for (i=0,theta=0. ; i<nh ; i++)
{
x=r*COSD(theta); y=r*SIND(theta);
fprintf (fpo,"%2d, %.4lf, %+.4lf, %+.4lf, %.4lf\n",i,dhole[i],x,y,theta);
if (i<(nh-1)) theta+=ASND(0.5*dhole[i]/r)+ASND(0.5*dhole[i+1]/r)+phi;
}

#if 0 == 1
theta+=ASND(0.5*dhole[nh-1]/r)+ASND(0.5*dhole[0]/r)+phi;
printf ("theta = %.4lf\n",theta);
#endif

fprintf (fpo,"\nTheoretical minimum stock diameter = %.4lf\n",bcd+hdmax);
fprintf (fpo,"Recommended stock diameter = %.4lf\n",bcd+hdmax+2.*space);

printf ("\nYour data is on: %s\n",ofile);

/* post run processing */
mexit:
fcloseall();
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* rdata..----------------------------------------------------------------- */

/* read the data file */

char rdata (void)

{
char temp[81],f,a;
int i;
dbl l;

if ((fpi=fopen(dfile,"rt")) == NULL)
	{printf ("FAILED TO OPEN DATA FILE: %s\n",dfile); beep; return 1;}

nh=0;			// zero data item counter
f=0;			// zero utility flag

while ((fgets(temp,sizeof(temp),fpi)) != NULL)
	{
	trim(temp);		/* uses library trim */
	if ((strstr(temp,"STARTOFDATA")) != NULL) {f=1; continue;}
	if (!f) continue;
	if ((strstr(temp,"ENDOFDATA")) != NULL) break;
	if (temp[0] != '\0' && temp[0] != ';')
		{
		dhole[nh]=(dbl)atof(strtok(temp,",\t;"));
		if (++nh == NH)
			{
			printf
			("More than maximum (%d) reals in data file\n",NH);
			return 1;
			}
		} // end if(temp[0]...
	} // end while

fclose (fpi);
return 0;
}
/* func..------------------------------------------------------------------ */

/* 
for a bolt circle of radius R, compute the total angle subtended by all the
holes and their associated spacing

returns total angle - 360 = angle excess

*/

dbl func (dbl R)

{
int i;
dbl sum,phi;

phi=DPR*space/R;
for (i=0,sum=0. ; i<nh ; i++) sum+=2.*ASND(.5*dhole[i]/R)+phi;

return sum-360.;
}
/* decsearch..------------------------------------------------------------- */

/* 
search the function 'func' between 'l' and 'r' for the value 'val' with
tolerance 'tol'
*/

dbl decsearch (dbl l, dbl r, dbl val, dbl tol)

{
int i=0;
dbl x,y,xs,xe,dx,d,bx,bd;

xs=l; xe=r; dx=0.1*(r-l); i=0;
more: for (x=xs,bd=1.0e99 ; x<=xe ; x+=dx)
	{y=func(x); d=ABS(y-val); if (d < bd) {bx=x; bd=d;}}
if (++i>200) {beep; goto done;}
#if 0 == 1
printf ("%d: %lf, %lf  %lf,%lf\n",i,x,y,xs,xe);		//debug print
#endif
if (bd > tol) {xs=bx-dx; xe=bx+dx; dx*=0.1; goto more;}
done: return bx;
}
/* ------------------------------------------------------------------------ */
