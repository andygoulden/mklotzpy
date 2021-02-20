/* ATMOS76.C   M. W. Klotz  2/00

Compute the properties of the 1976 standard atmosphere to 86 km
Adapted from a Fortran 90 program by:
Ralph Carmichael, Public Domain Aeronautical Software

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
void atmos (dbl alt);

/* gmark..-------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* vmark..------------ global variable declarations ----------------------- */
#define FPKM 3280.839895	/* ft/km */
dbl alt;	/* altitude (km) */
dbl dratio;	/* density ratio wrt sea level (nd) */
dbl pratio;	/* pressure ratio wrt sea level (nd) */
dbl tratio;	/* temperature ratio wrt sea level (nd) */
dbl rearth=6369.0;	/* earth radius (km) */
dbl gmr=34.163195;	/* gas constant */
#define NTAB 8		/* maximum entries in tables */
dbl htab[NTAB]={0.0, 11.0, 20.0, 32.0, 47.0, 51.0, 71.0, 84.852};
dbl ttab[NTAB]={288.15, 216.65, 216.65, 228.65, 270.65, 270.65, 214.65, 186.946};
dbl ptab[NTAB]={1.0, 2.233611e-1, 5.403295e-2, 8.5666784e-3, 1.0945601e-3,\
            6.6063531e-4, 3.9046834e-5, 3.68501e-6};
dbl gtab[NTAB]={-6.5, 0.0, 1.0, 2.8, 0.0, -2.8, -2.0, 0.0};
dbl t0=288.15;		/* sea level temperature (K) */
dbl p0=101325;		/* sea level pressure (N/m^2) */
dbl d0=1.225;		/* sea level density (kg/m^3) */
dbl ta,pa,da,tc,tf,vs;

/* smark..----------- global structure declarations ----------------------- */

/* main..================================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i,j,k;

/* -------------------------- initializations ------------------------------ */
textscreen();
/* ------------------------------------------------------------------------- */

puts ("ATMOSPHERIC COMPUTATIONS (valid 0 to 86 km)");

puts ("1 km = 3280.84 ft");
vin ("Altitude",&alt,"lf",1000.,"ft");
atmos(alt/FPKM);
printf ("\nAltitude = %.4E km = %.4E mi = %.4E ft\n",alt/FPKM,alt/5280,alt);
printf ("Temperature ratio (wrt sl) = %.4E\n",tratio);
printf ("Pressure ratio (wrt sl) = %.4E\n",pratio);
printf ("Density ratio (wrt sl) = %.4E\n",dratio);

puts ("\nFor sea level values of temperature, pressure, density of:");
printf ("%.4E K, %.4E N/m^2, %.4E kg/m^3\n\n",t0,p0,d0);

ta=tratio*t0; pa=pratio*p0; da=dratio*d0;

tc=ta-273.18; tf=1.8*tc+32.;
printf ("Temperature = %.4lf K = %.4lf C = %.4lf F \n",ta,tc,tf);
printf ("Pressure = %.4E N/m^2 = %.4E psi\n",pa,pa/6894.76);
printf ("Density = %.4E kg/m^3 = %.4E slug/ft^3\n",da,da/515.379);


vs=1116.45*sqrt((tf+459.67)/518.67)*3600/5280;
printf ("Speed of sound = %.4lf kph = %.4lf mph\n",vs*1.60934,vs);

/* post run processing */
mexit:
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* atmos..----------------------------------------------------------------- */

/* atmospheric computations */

void atmos (dbl alt)

{
int i,k;
dbl h,tgrad,tbase,deltah,tlocal;

h=alt*rearth/(alt+rearth);  /* convert geometric to geopotential altitude */

for (i=0 ; i<NTAB-1 ; i++) {if (h >= htab[i] && h < htab[i+1]) {k=i; break;}}

tgrad=gtab[k];
tbase=ttab[k];
deltah=h-htab[k];
tlocal=tbase+tgrad*deltah;
tratio=tlocal/ttab[0];		/* temperature ratio */
/* pressure ratio */
if (tgrad == 0.0) pratio=ptab[k]*exp(-gmr*deltah/tbase);
else pratio=ptab[k]*pow(tbase/tlocal,gmr/tgrad);
dratio=pratio/tratio;		/* density ratio */

}
/* ------------------------------------------------------------------------ */
