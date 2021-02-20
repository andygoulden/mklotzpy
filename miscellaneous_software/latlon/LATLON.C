/* LATLON.C   M. W. Klotz   3/02

Given lat and lon of two points on earth, compute great circle distance and
azimuths.

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
dbl caz (dbl lat1, dbl lon1, dbl lat2, dbl lon2);
dbl norm (dbl x, dbl a, dbl b);

/* gmark..-------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* vmark..------------ global variable declarations ----------------------- */

/* smark..----------- global structure declarations ----------------------- */

/* main..================================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i,j,k;
dbl lat1,lon1,lat2,lon2,slat1,clat1,slat2,clat2,cphi,phi,range;

/* -------------------------- initializations ------------------------------ */
/* ------------------------------------------------------------------------- */

puts ("Latitude is measured positive northward from equator.");
puts ("Longitude is measured positive eastward from prime meridian.\n");

agin1: vin ("Latitude of first point",&lat1,"lf",33.7775436,"deg");
if (lat1 > 90. || lat1 < -90.)
{beep; puts ("-90 <= latitude <= 90"); goto agin1;}
vin ("Longitude of first point",&lon1,"lf",-118.3769558,"deg");
lon1=norm (lon1,-180.,180.);
agin2: vin ("Latitude of second point",&lat2,"lf",51.5,"deg");
if (lat2 > 90. || lat2 < -90.)
{beep; puts ("-90 <= latitude <= 90"); goto agin2;}
vin ("Longitude of second point",&lon2,"lf",0.,"deg");
lon2=norm (lon2,-180.,180.);

slat1=SIND(lat1); clat1=COSD(lat1);
slat2=SIND(lat2); clat2=COSD(lat2);
cphi=slat1*slat2+clat1*clat2*COSD(lon2-lon1); phi=ACSD(cphi);
range=REKM*phi*RPD;

print;
printf ("lat1, lon1 = %.2lf, %.2lf\n",lat1,lon1);
printf ("lat2, lon2 = %.2lf, %.2lf\n",lat2,lon2);
printf ("Central angle between locations = %.2lf deg\n",phi);
printf ("Great circle distance = %.2lf km = %.2lf miles\n",range,range*FPKM/5280.);
printf ("Azimuth of point 2 from point 1 = %.2lf deg\n",caz(lat1,-lon1,lat2,-lon2));
printf ("Azimuth of point 1 from point 2 = %.2lf deg\n",caz(lat2,-lon2,lat1,-lon1));

/* post run processing */
mexit:
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* caz..------------------------------------------------------------------- */

/* compute relative azimuth (point 2 wrt point 1) */

dbl caz (dbl lat1, dbl lon1, dbl lat2, dbl lon2)

{
dbl slat1,clat1,slat2,clat2;
dbl A,cosA,sinA,cosb,cosc,az;
if (lat1 == lat2 && lon1 == lon2) return 0;
slat1=SIND(lat1); slat2=SIND(lat2); clat1=COSD(lat1); clat2=COSD(lat2);
cosA=slat1*slat2+clat1*clat2*COSD(ABS(lon2-lon1));
A=ACSD(cosA); sinA=SIND(A);
cosb=(slat2-slat1*cosA)/(clat1*sinA);
az=ACSD(cosb);
if (lon1 == lon2) {if (lat1 <= lat2) az=0.; else az=180.;}
if (lon2 > lon1) az=360.-az;
if (ABS(lon2-lon1) > 180.) az=360.-az;
return az;
}
/* ------------------------------------------------------------------------ */

/* normalize x to be between a and b */

dbl norm (dbl x, dbl a, dbl b)

{
dbl c=b-a;

while (x < a) x+=c;  while (x > b) x-=c; return x;
}

/* ------------------------------------------------------------------------ */
