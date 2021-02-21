/* DOOMSDAY.C   M. W. Klotz   5/01

Implements "Doomsday" algorithm for determining day-of-the-week from date.

*/

/* head..-------------------- header files -------------------------------- */

/* Unneeded headers can be left in place - the header files consist mainly of
'defines' for the compiler and do not add to the size of the executable code.
*/

/* turns off assert macros when not debugging */
/* #define NDEBUG */

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
typedef double dbl;			/* 8 byte,  15 digit, 10^+/-308 */
typedef float real;			/* 4 byte   7 digit, 10^+/-38 */
typedef unsigned long ulong;		/* 4 byte   0 to 4,294,967,295 */
typedef unsigned int uint;		/* 2 byte   0 to 65535 */
typedef unsigned char uchar;		/* 1 byte   0 to 255 */

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
#define ABS(a) ((a)>=0 ? (a) : -(a))

/* constant declarations - all 'defines' - no executable code */
#include <constant.h>

/* -------------------------- function prototypes ------------------------- */

/* Minimize the number of these included since they contribute to exectuable
size. */

/* global utility functions */
/* note: no space between function name and parameter list */
#include <util.h>

/* string manipulation */
/* #include <strings.h> */

/* data input routines */
/* #include <input.h> */

/* vector utilities */
/* #include <vector.h> */

/* printing data */
/* #include <output.h> */

/* graphics routines */
/* #include <graphics.h> */

/* file management routines */
/* #include <file.h> */

/* pmark..  functions peculiar to this program */
long jdbc (int xm, int xd, int xy);
unjdbc (long jd, struct unjulian *p);
void unjp (struct unjulian *p);
void gin (void);
void dtinit (struct datetime *s);
void iinput (char prompt[],int defalt,int *idata);

/* --------------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* struc..----------- global structure declarations ----------------------- */

struct datetime {
int month;		/* local date and time */
int day;
int year;
int hour;
int minute;
int second;
int dw;			/* day of week number (SUN=0) */
char dow [4];		/* day of week string */
int isdst;		/* daylight savings flag */
int dyear;		/* days since 1 January (0-365) */
int umonth;		/* universal date and time */
int uday;
int uyear;
int uhour;
int uminute;
int usecond;
real lt;
real ut;		/* universal time in fractional hours */
dbl jd;			/* julian day number */
};

struct datetime dt;
struct tm *today,*gmt;		/* pointers to library structures */

struct unjulian {
long jd;	/* julian day number */
int day;	/* day of the month - [1,31] */
int month;	/* months since January - [0,11] */
int year;	/* year */
int wday;	/* days since Sunday - [0,6] */
int yday;	/* days since January 1 - [0,365] */
};

struct unjulian unj;

/* vmark..------------ global variable declarations ----------------------- */

char days [7] [4] = {"SUN","MON","TUE","WED","THU","FRI","SAT"};
int m,d,y;
long d1,d2,diff;
long ltime;			/* time_t type defined as long in time.h */

int cdoom[4] = {2,0,5,3};	/* century year doomsdays */

/* main..====xxxxxxxx====================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i,j,k,century,yc,dc,q,r,s,doom,mdoom,dd;
long x;
char op;
struct unjulian *p=&unj;

/* -------------------------- initializations ------------------------------ */
textscreen();
kbdset (0xbf);		/* lower case */
/* ------------------------------------------------------------------------- */

dtinit(&dt);		/* initialize the structure */

/* gmt and ltime initialized via dtinit */
printf ("Greenwich Mean Time:\t\t\t%s",asctime(gmt));
printf ("Local Time:\t\t\t\t%s\n",ctime(&ltime));

puts ("input date:");
gin (); x=jdbc (m,d,y);
unjdbc (x,&unj);
print;
unjp (&unj);
print;

/* calculate doomsday for input year */

century=y/100; yc=y % 100;
dc=cdoom[century % 4];
q=yc/12; r=yc-12*q; s=r/4;
doom=(dc+q+r+s) % 7;
printf ("doomsday for %d = %d %s\n",100*century,dc,days[dc]);
printf ("doomsday for %d = %d %s\n",y,doom,days[doom]);

/* obtain doomsdate in month m */

if (m % 2 == 0 && m > 2) mdoom=m;
else
{
if (m == 3) mdoom=7;
if (m == 5) mdoom=9;
if (m == 9) mdoom=5;
if (m == 7) mdoom=11;
if (m == 11) mdoom=7;
}
if ((y % 4 == 0) || (y % 400 == 0))
{
if (m == 1) mdoom=25;
if (m == 2) mdoom=29;
}
else
{
if (m == 1) mdoom=31;
if (m == 2) mdoom=28;
}
printf ("doomsdate for month %d = %d\n",m,mdoom);

/* do the mod 7 arithmetic for the day in question */

dd=(doom+d-mdoom) % 7; while (dd < 0) dd+=7;
printf ("%d/%d/%d = %s\n",m,d,y,days[dd]);

/* post run processing */
mexit:
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* jdbc..------------------------------------------------------------------ */

/*
compute julian day number (days since 1/1/4713 BC) from month, day, year

The Gregorian calendar is assumed provided the Julian day number falls on or
after the value in the variable 'transition', otherwise the Julian calendar is
used.  'transition' is initialized to 2361222 (or September 14, 1752) as in
the UK and USA.

Algorithm is valid from 4713 B.C. to 19,999 A.D.

Years prior to 1 AD should be entered as negative numbers.  Since there was no
year 0, the program provides an internal correction so that the year before
1 AD (input as -1) becomes 0 and 2 BC (input as -2) becomes the year -1.

Calling the routine with  x=jdbc (1,1,-4713);  should produce x=0.
Some other check cases:
x=jdbc (4,14,1998);  x=2450918
x=jdbc (9,10,-2699); x=735866
12/28/-1 => TUE 362 1721420
1/1/1 => SAT 0 1721424
12/7/1941 => SUN 340 2430336
*/

long jdbc (int xm, int xd, int xy)

{
long transition=2361222L;	/* transition date to gregorian calendar */
long julian,gregorian,jd;
int ay,y,dy,m;
y=xy; 
if (y < 0) y++;			/* adjust for BC */
m=xm-1;				/* months since january */

dy=m/12; y+=dy; m-=dy*12;
/* assert ((m >= -11) && (m <= 11)); */
if (m < 0) {m+= 12; y--;}
/* assert ((m >= 0) && (m <= 11)); */
if (m < 2) {m+=1+12; y--;} else m++;

ay=y+4716;
julian=((1461*(long)ay) >> 2)+(153*(m+1)/5)+(long)xd-1524;
gregorian=julian+2-y/100+y/400-y/4000;
if (gregorian >= transition) jd=gregorian;  else jd=julian;
return jd;

}
/* unjdbc..---------------------------------------------------------------- */

/*
fill a structure with the month,day,year, etc. corresponding to an input
julian date 
*/

unjdbc (long jd, struct unjulian *p)
{
long transition=2361222L;	/* transition date to gregorian calendar */
long aa,ab,a,b,d,ee;
int ay,em,m,y;

p->day=0; p->month=0; p->year=0; p->wday=0; p-> yday=0;
p->jd=jd;

if (jd < transition)			 /* Julian Calendar */
	a=jd;
	else				/* Gregorian Calendar */ 
	{
	aa=jd-1721120L;
	ab=31*(aa/1460969L); aa=aa % 1460969L;
	ab=ab+3*(aa/146097L); aa=aa % 146097L;
	if (aa == 146096L) ab+=3;  else ab=ab+aa/36524L;
	a=jd+(ab-2);
	}
b=a+1524;
ay=(int)((20*b-2442)/7305);
d=(1461*(long)(ay)) >> 2;
ee=b-d;
em=(int)(10000L*ee/306001L);

p->day=(int)(ee-306001L*em/10000L);

m=em-1; if (m > 12) m-=12;
p->month=m;

if (m > 2) y=ay-4716;  else y=ay-4715;
p->year=y;

p->wday=(jd+1) % 7;

ay=p->year; if (ay <= 0) ay--;
p->yday=jd-jdbc(1,1,ay);
}
/* unjp..------------------------------------------------------------------ */

/* print unjulian results */

void unjp (struct unjulian *p)

{
printf ("julian day number = %ld\n",p->jd);
printf ("month = %d\n",p->month);
printf ("day   = %d\n",p->day);
printf ("year  = %d\n",p->year);
printf ("wday  = %d  %s\n",p->wday,days[p->wday]);
printf ("day-of-year = %d\n",p->yday+1);
}
/* gin..------------------------------------------------------------------- */

/* input gregorian date */

void gin (void)

{
struct datetime *p=&dt;

iinput ("month",p->month,&m);
iinput ("day",p->day,&d);
iinput ("year",p->year,&y);
}
/* dtinit..---------------------------------------------------------------- */

/* initialize date and time array */

void dtinit (struct datetime *s)
{

time(&ltime);		/* get current system time as long integer */

today=localtime(&ltime);	/* convert time from integer to structure */
s->month=today->tm_mon+1;
s->day=today->tm_mday;
s->year=today->tm_year;
if (s->year>50) s->year+=1900; else s->year+=2000;
s->hour=today->tm_hour;
s->minute=today->tm_min;
s->second=today->tm_sec;
s->dw=today->tm_wday;
strncpy (s->dow,days[s->dw],4);
s->isdst=today->tm_isdst;
s->dyear=today->tm_yday;

gmt=gmtime(&ltime);		/* convert time from integer to structure */
s->umonth=gmt->tm_mon+1;
s->uday=gmt->tm_mday;
s->uyear=gmt->tm_year;
if (s->uyear>50) s->uyear+=1900; else s->uyear+=2000;
s->uhour=gmt->tm_hour;
s->uminute=gmt->tm_min;
s->usecond=gmt->tm_sec;
s->ut=s->uhour+s->uminute/60.0+s->usecond/3600.0;
s->jd=jdbc(s->umonth,s->uday,s->uyear);
}
/* iinput..---------------------------------------------------------------- */

/* get integer input from user with prompt and default value */

void iinput (char prompt[],int defalt,int *idata)
{
char string[80];
fprintf (_out,"%s [%d] ? ",prompt,defalt);
fgets(string,sizeof (string),_in);
if (sscanf (string,"%d",idata) != 1) *idata=defalt;
}
/* ------------------------------------------------------------------------ */
/* 

 beginning of Julian dates
       0    -4713  1  1 Mon   0
       1    -4713  1  2 Tue   1
       2    -4713  1  3 Wed   2
       3    -4713  1  4 Thu   3

 Julian calendar repeats every 4 years, at 365*4+1 = 1461 day intervals
  260424    -4000  1  1 Thu   0
   1461        4
  261885    -3996  1  1 Tue   0
   1461        4
  263346    -3992  1  1 Sun   0

 Julian dates near 1 AD (there was no year 0)
 1721420       -1 12 28 Tue 362
 1721421       -1 12 29 Wed 363
 1721422       -1 12 30 Thu 364
 1721423       -1 12 31 Fri 365
 1721424        1  1  1 Sat   0
 1721425        1  1  2 Sun   1
 1721426        1  1  3 Mon   2

	usual transition dates are:

	2299161 = October 5/15, 1582,       as in Rome, or
	2361222 = September 3/14, 1752,     as in the United Kingdom 
	                                    and the Colonies (default)

 Julian dates near the transition...
          
 2361220       1752  9  1  Tue  244 
 2361221       1752  9  2  Wed  245 
 2361222       1752  9 14  Thu  246 
 2361223       1752  9 15  Fri  247 
T2299161                                ; as in Rome
 2299159     1582 10  3 Wed 275
 2299160     1582 10  4 Thu 276
 2299161     1582 10 15 Fri 277
 2299162     1582 10 16 Sat 278

 Apart from the 4000 year correction, the Gregorian calendar repeats 
 every 400 years, or at ((365*4+1)*25-1)*4+1 = 146097 day intervals
 2302526     1592  1  1 Wed   0
 146097      400
 2448623     1992  1  1 Wed   0
 146097      400
 2594720     2392  1  1 Wed   0
 146097      400
 2740817     2792  1  1 Wed   0

 Including the 4000 year correction, the Gregorian calendar repeats 
 at (((365*4+1)*25-1)*4+1)*10-1 = 1460969 day intervals
 2302526     1592  1  1 Wed   0
1460969     4000
 3763495     5592  1  1 Tue   0
1460969     4000
 5224464     9592  1  1 Mon   0
1460969     4000
 6685433    13592  1  1 Sun   0

	The USNO time service reports the Modified Julian Date (MJD),
	which is figured by subtracting a constant (2,400,000.5) from the
	Julian Date as of the preceding midnight.

    For example, the MJD for 7 Jul 92 was 48806
                              offset    2400000.5
                     JD at midnight was 2448806.5
                         JD at noon was 2448807

 2448807     1992  7  3 Fri 184

 checks on day-of-week calculation...
 "black Tuesday" 
 2425914     1929 10 29 Tue 301
 "a day that will live in infamy"
 2430336     1941 12  7 Sun 340
 "black Monday" 
 2447088     1987 10 19 Mon 291
*/
