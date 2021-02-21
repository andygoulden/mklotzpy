/* SUN.C   M. W. Klotz   12/97, 2/99, 6/01, 11/03

solar calculations

*/

/* -------------------------- header files -------------------------------- */

/* turns off assert macros when not debugging */
/* #define NDEBUG */

#include <ctype.h>
#include <graph.h>
#include <float.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
/* 
#include <assert.h>
#include <conio.h>
#include <dos.h>
#include <malloc.h> 
#include <search.h>
*/

/* def..----------------------- defines -------------------------------------*/

/* type synonyms */
typedef float real;			/* 4 byte   7 digit, 10^+/-38 */
typedef unsigned long ulong;		/* 4 byte   0 to 4,294,967,295 */
typedef unsigned int uint;		/* 2 byte   0 to 65535 */
typedef unsigned char uchar;		/* 1 byte   0 to 255 */
typedef double dbl;			/* 8 byte,  15 digit, 10^+/-308 */

/* macro declarations */
#define _in stdin			/* makes redirection easier */
#define _out stdout
#define pause getch()
#define pausem getkey("press a key\n")
#define bpause tone (440,100); getch()
#define bpausem tone (440,100); getkey("press a key\n")
#define beep tone(440,100)
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

/* constant declarations */
#include <constant.h>

/* global utility functions */
/* note: no space between function name and parameter list */
#include <util.h>

/* -------------------------- function prototypes ------------------------- */

/* data input routines */
/* #include <input.h> */

/* vector utilities */
/* #include <vector.h> */

/* string manipulation */
/* #include <strings.h> */

/* printing data */
/* #include <output.h> */

/* graphics routines */
/* #include <graphics.h> */

/* file management routines */
/* #include <file.h> */

/* pmark..  functions peculiar to this program */

void suncomp (struct sundat *s);
void sunprint (struct sundat *s);
int tnorm (struct sundat *s);
void pstpdt (dbl ut);
void hms (char *label, dbl t);
void dms (dbl a);
void menu (void);
void dtinit (struct datetime *s);
double jdnum (int month, int day, int year, real ut);
void csep (real x, char format[]);
void sd (struct sundial *p);
void sdp (struct sundial *p);
void iinput (char prompt[],int defalt,int *idata);
void rinput (char prompt[],float defalt,float *data);
double normab (double x, double a, double b);
void info (void);
char rdata (void);
dbl caz (dbl lat1, dbl lon1, dbl lat2, dbl lon2);

/* --------------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* struc..----------- global structure declarations ----------------------- */

struct sundat {
int month;
int day;
int year;
dbl ut;		/* universal time (hr) */
dbl lat;	/* latitude (deg) */
dbl lon;	/* longitude (deg) */
dbl ra;		/* right ascension (hr) */
dbl dec;	/* declination (deg) */
dbl alt;	/* altitude of sun (deg) */
dbl az;		/* azimuth of sun (deg) */
dbl dist;	/* distance (au) */
dbl eqt;	/* equation of time (sec) */
dbl gmst;	/* greenwich mean siderial time (hr) */
dbl gast;	/* greenwich apparent siderial time (hr) */
dbl ghaa;	/* greenwich hour angle of Aries (deg) */
dbl lhas;	/* local hour angle of sun (deg) */
dbl ghas;	/* greenwich hour angle of sun (deg) */
dbl sd;		/* semi-diameter of sun (deg) */
dbl hparx;	/* horizontal parallax (deg) */
dbl aparx;	/* altitude parallax (deg) */
dbl utrise;	/* ut of sunrise (hr) */
dbl utset;	/* ut of sunset (hr) */
dbl ost;	/* standard time GMT offset (hr) */
dbl odt;	/* daylight saving time GMT offset (hr) */
};

struct sundat s1;

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
dbl jd;			/* julian date */
};

struct datetime dt;
struct tm *today,*gmt;		/* pointers to library structures */

struct sundial {
real alt;		/* altitude of sun (deg) */
real az;		/* azimuth of sun (deg) */
real phip;		/* perpendicular to plane - azimuth (deg) */
real thetap;		/* perpendicular to plane - elevation (deg) */
real phig;		/* gnomon azimuth (deg) */
real thetag;		/* gnomon elevation (deg) */
real lgnomon;		/* gnomon length (units arbitrary) */
/*
IF E,N,U are unit vectors along East(x), North(y) and Up(z)
 and P is a unit vector normal to the plane, then:

ALPHA = ANGLE OF SHADOW wrt N x P
BETA = ANGLE OF SHADOW wrt P x E
GAMMA = ANGLE OF SHADOW wrt P (90 deg for horizontal plane)
*/
real alpha;		/* shadow angle wrt N x P (deg) */
real beta;		/* shadow angle wrt P x E (deg) */
real gamma;		/* shadow angle wrt P (deg) */
real ratio;		/* ratio of shadow length/gnomon length (nd) */
};

struct sundial dial;

/* vmark..------------ global variable declarations ----------------------- */

FILE *fp;			/* utility file pointer */
char dfile[]="SUN.DAT";		/* input data file */
#define ND 5			/* maximum number of data items */
int nd;				/* actual number of data items */
time_t ltime;	/* time_t type defined as long in time.h */
int mo[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};

/* main..====xxxxxxxx====================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i,j,k,m,d,y,dmin;
real x;
char op;
dbl xmin,xmax,uts,azover,azlast,ulast,ut0,emin;
struct sundial *di=&dial;

/* -------------------------- initializations ------------------------------ */
textscreen();
dtinit(&dt);		/* initialize time structure */
if (rdata ()) goto mexit;		/* read the data file */
/* ------------------------------------------------------------------------- */

magin: menu ();
agin: op=getkey ("Option ?");

switch (op)
{
case 'a':	/* current time */
	s1.month=dt.month;
	s1.day=dt.day;
	s1.year=dt.year;
	s1.ut=dt.ut;
	break;
case 'b':	/* input time */
	iinput ("month",dt.month,&s1.month);
	iinput ("day",dt.day,&s1.day);
	iinput ("year",dt.year,&s1.year);
	rinput ("ut",dt.ut,&x); s1.ut=(dbl)x;
	break;
case 'e':	/* eqt minimum */
	iinput ("month",dt.month,&s1.month);
	iinput ("year",dt.year,&s1.year);
	s1.ut=12.+s1.lon/15.; emin=1.e9; dmin=0;
	for (i=1 ; i<=mo[s1.month] ; i++)
	  {
	  s1.day=i; suncomp (&s1);
	  if (ABS(s1.eqt) < emin) {emin=ABS(s1.eqt); dmin=s1.day;}
	  }
	s1.day=dmin; suncomp (&s1);
	break;
case 'f':	/* fall equinox */
	iinput ("year",dt.year,&s1.year);
	s1.month=9;
	s1.day=19;
	xmin=1.e9;
	for (s1.ut=12.,i=1; i<=4*24 ; i++,s1.ut++)
	{
	suncomp (&s1);
	if (ABS(s1.dec) < xmin)
		{xmin=ABS(s1.dec); m=s1.month; d=s1.day; y=s1.year; uts=s1.ut;}
	}
	s1.month=m; s1.day=d; s1.year=y; s1.ut=uts;
	break;
case ' ':
case 'h':	/* display menu */
	goto magin;
case 'i':	/* display information */
	info ();
	goto magin;
case 'm':	/* meridian transit */
	iinput ("month",dt.month,&s1.month);
	iinput ("day",dt.day,&s1.day);
	iinput ("year",dt.year,&s1.year);
	azover=(s1.lat > 0.) ? 180. : 0.;
	ut0=s1.ut=12.+s1.lon/15.;
	suncomp (&s1); azlast=1.e10;
	while (ABS(s1.az-azover) > .001)
		{
		if (azlast > 1.e9)
			{
			azlast=s1.az; ulast=s1.ut;
			s1.ut=ut0-s1.eqt/3600.;
			}
		else
			{
			s1.ut=ut0+(azover-azlast)*(s1.ut-ulast)/(s1.az-azlast);
			azlast=s1.az; ulast=s1.ut;
			}
		suncomp(&s1);
		}
	break;
case 27:
case 'q':	/* quit */
	goto mexit;
case 's':	/* summer solstice */
	iinput ("year",dt.year,&s1.year);
	s1.month=6;
	s1.day=19;
	xmax=-1.e9;
	for (s1.ut=12.,i=1; i<=4*24 ; i++,s1.ut++)
	{
	suncomp (&s1);
	if (s1.dec > xmax)
		{xmax=s1.dec; m=s1.month; d=s1.day; y=s1.year; uts=s1.ut;}
	}
	s1.month=m; s1.day=d; s1.year=y; s1.ut=uts;
	break;
case 'v':	/* vernal equinox */
	iinput ("year",dt.year,&s1.year);
	s1.month=3;
	s1.day=19;
	xmin=1.e9;
	for (s1.ut=12.,i=1; i<=4*24 ; i++,s1.ut++)
	{
	suncomp (&s1);
	if (ABS(s1.dec) < xmin)
		{xmin=ABS(s1.dec); m=s1.month; d=s1.day; y=s1.year; uts=s1.ut;}
	}
	s1.month=m; s1.day=d; s1.year=y; s1.ut=uts;
	break;
case 'w':	/* winter solstice */
	iinput ("year",dt.year,&s1.year);
	s1.month=12;
	s1.day=19;
	xmin=1.e9;
	for (s1.ut=12.,i=1; i<=4*24 ; i++,s1.ut++)
	{
	suncomp (&s1);
	if (s1.dec < xmin)
		{xmin=s1.dec; m=s1.month; d=s1.day; y=s1.year; uts=s1.ut;}
	}
	s1.month=m; s1.day=d; s1.year=y; s1.ut=uts;
	break;
default:
	beep; goto magin;
}
suncomp (&s1);
/* cls; */
sunprint (&s1);
if (s1.alt > 0)
{
puts ("for a horizontal sundial with a vertical gnomon:");
puts ("P = normal to sundial plane | dial time = clock time + EQT");
di->alt=s1.alt; di->az=s1.az; di->lgnomon=1.;
di->phip=0.; di->thetap=90.;
di->phig=0.; di->thetag=90.;
sd (di); sdp (di);
puts ("for a vertical (south facing) sundial with a horizontal gnomon:");
di->phip=180.; di->thetap=0.;
di->phig=180.; di->thetag=0.;
sd (di); sdp (di);
}

goto agin;

/* post run processing */
mexit:
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* suncomp..--------------------------------------------------------------- */

/* COMPUTE PARAMETERS FOR SUN */

void suncomp (struct sundat *s)

{
int i;
dbl G,L,C,N,V,M,J,t1,t2;
dbl sinl,singml,singl,cosg,sing,tsun,rsun,psun,x,ra,dec,dist;
dbl ut,et,xs1,xs2,xjed0,jd,mjd,jed,tcap,tlc,gmst,omega,eqe,gast,ghaa;
dbl last,sunml,eqt,slat,clat,lha,sd,cd,slha,clha,alt,xn,xd,az,tss,xm,xl;
dbl alpha,sdel,cdel,cosh,utrs[2],hh;
int ts1,ts2,ts3,ts4,nn;

/*
G=MEAN ANOMALY OF SUN
L=MEAN LONGITUDE OF SUN
C=MEAN LONGITUDE OF MOON
N=LONGITUDE OF ASCENDING NODE OF MOON'S ORBIT
V=MEAN ANOMALY OF VENUS
M=MEAN ANOMALY OF MARS
J=MEAN ANOMALY OF JUPITER
*/

tnorm(s);
ut=s->ut;
slat=SIND(s->lat); clat=COSD(s->lat);
et=ut+49.3/3600.;
ts1=(int)(275*s->month/9); ts2=(int)((s->month+9)/12);
ts3=(int)(s->year % 4); ts4=(int)(1+(ts3+2)/3);
nn=ts1-ts2*ts4+s->day-30;
xs1=100.*s->year+s->month-190002.5, xs2=.5*(1-SGN(xs1));
xjed0=367.*s->year-(int)(.25*7.*(s->year+ts2))+ts1+s->day+1721013.5+xs2;
jd=xjed0+ut/24.;		/* julian date */
mjd=jd-2400000.5;		/* mean julian date */
jed=xjed0+et/24.;		/* julian ephemeris date */
tcap=(jed-2415020.)/36525.;	/* time from 1900 epoch (julian centuries) */
tlc=nn+ut/24.;			/* time from jan. 0 of current year (days) */
gmst=normab (6.67170278+.0657098232*(xjed0-2433282.5)+1.0027379093*ut,0.,24.);
omega=normab (372.1133-.0529539*(jd-2433282.5),0.,360.);
eqe=-0.00029*SIND(omega);	/* equation of the equinoxes (hr) */
gast=normab (gmst+eqe,0.,24.);	/* greenwich apparent siderial time (hr) */
ghaa=15.*gast;			/* greenwich hour angle of aries (deg) */
last=normab (gast-s->lon/15.,0.,24.);
				/* local apparent siderial time (hr) */
sunml=normab (279.697+.98564734*(jd-2415020.),0.,360.);
				/* sun mean longitude (deg) */
x=sunml*RPD;

/* 
sundial time = clock time + eqt
eqt = true sun - fictitious sun
if eqt>0 the true sun leads the fictitious sun and the sundial reads fast
*/

eqt=-97.8*sin(x)-431.3*cos(x)+596.6*sin(2.*x)-1.9*cos(2.*x)+4.*sin(3.*x)+ \
	19.3*cos(3.*x)-12.7*sin(4.*x);	/* equation of time (sec) */

t1=tcap, t2=t1*t1;

G=RPD*normab(358.475833+35999.04975*t1-.00015*t2,0.,360.);
L=RPD*normab(279.696678+36000.76892*t1+.000303*t2,0.,360.);
C=RPD*normab(270.434164+480960.*t1+307.883142*t1-.001133*t2,0.,360.);
N=RPD*normab(259.183275-1800.*t1-134.142008*t1+.002078*t2,0.,360.);
V=RPD*normab(212.603219+58320.*t1+197.803875*t1+.001286*t2,0.,360.);
M=RPD*normab(319.529425+19080.*t1+59.8585*t1+.000181*t2,0.,360.);
J=RPD*normab(225.444651+2880.*t1+154.906654*t1,0.,360.);

sinl=sin(L), singml=sin(G-L), singl=sin(G+L);
cosg=cos(G), sing=sin(G);
tsun=.39793*sinl+9.998999E-03*singml+.003334*singl-.000208*t1*sinl;
tsun=tsun+.000042*sin(2.*G+L)-.00004*cos(L)-.000039*sin(N-L);
tsun=tsun-.00003*t1*singml-.000014*sin(2.*G-L)-.00001*cos(G-L-J) \
	-.00001*t1*singl;
rsun=1.000421-.033503*cosg-.00014*cos(2.*G)+.000084*t1*cosg-.000033*sin(G-J) \
	+.000027*sin(2.*(G-V));
psun=-.041295*sin(2.*L)+.032116*sing-.001038*sin(G-2.*L)-.000346*sin(G+2.*L);
psun=psun-.000095-.00008*t1*sing-.000079*sin(N)+.000068*sin(2.*G);
psun=psun+.000046*t1*sin(2.*L)+.00003*sin(C-L)-.000025*cos(G-J);
psun=psun+.000024*sin(4.*G-8.*M+3.*J)-.000019*sin(G-V)-.000017*cos(2.*(G-V));

x=psun/sqrt(rsun-tsun*tsun);
ra=normab ((L*DPR+ASND(x))/15.,0.,24.);	/* right ascension (hr) */
dec=ASND(tsun/sqrt(rsun));		/* declination (deg) */
dist=sqrt(rsun);			/* dist (au) */

lha=15.*(gast-ra)-s->lon;		/* local hour angle (deg) */
sd=SIND(dec); cd=COSD(dec); slha=SIND(lha); clha=COSD(lha);
alt=ASND(slat*sd+clat*cd*clha);		/* altitude of sun (deg) */
/*note: ghas = longitude of sun (measured positive westwards) */
s->ghas=normab (ghaa-ra*15.,0.,360.);	/* greenwich ha of sun (deg) */
s->lhas=normab (s->ghas-s->lon,0.,360.);	/* local ha of sun (deg) */
az=caz(s->lat,s->lon,dec,s->ghas);	/* azimuth of sun (deg) */
s->sd=959.63/(3600.*dist);		/* semi-diameter of sun (deg) */
s->hparx=8.793999*s->sd/959.63;		/* horizontal parallax (deg) */
s->aparx=ASND(SIND(s->hparx)*COSD(alt));	/* altitude parallax (deg) */

s->ra=ra; s->dec=dec; s->dist=dist; s->gast=gast; s->gmst=gmst;
s->ghaa=ghaa; s->alt=alt; s->az=az; s->eqt=eqt;

/* sunrise/set calculations */
for (i=0 ; i<2 ; i++)
{
tss=nn+(6.+12.*i+s->lon/15.)/24.;
xm=normab(.9856*tss-3.251,0.,360.);
xl=normab(xm+1.916*SIND(xm)+.02*SIND(2.*xm)+282.565,0.,360.);
alpha=ATAND(.91746*TAND(xl));
while (ABS(alpha-xl) >= 10) alpha+=180.;
sdel=.39782*SIND(xl); cdel=sqrt(1.-SQR(sdel));
cosh=(COSD(90.+5./6.)-sdel*slat)/(cdel*clat);
if (ABS(cosh) > 1.) {utrs[i]=1.e10; continue;}
hh=ACSD(cosh); if (i == 0) hh=360.-hh;
utrs[i]=normab((hh+alpha)/15.-.06571*tss-6.62,0.,24.)+s->lon/15.;
}
s->utrise=utrs[0]; s->utset=utrs[1];
}

/* sunprint..-------------------------------------------------------------- */

/* PRINT SUN DATA */

void sunprint (struct sundat *s)

{
dbl d,p;
printf ("%d/%d/%d  ",s->month,s->day,s->year); hms ("UT",s->ut); pstpdt (s->ut);
printf ("\nRA   = %-.4f hr = %-.4fø = ",s->ra,normab (s->ra*15.,0.,360.));
dms(normab(s->ra*15.,0.,360.));
printf ("\nDEC  = %-.4fø = ",s->dec); dms(s->dec);
printf ("\nDIST = %-.4f au = ",s->dist);
csep ((real)s->dist*AU*FPM/5280.,"%-.0f miles");
printf ("\nALT  = %-.4fø = ",s->alt); dms(s->alt);
printf ("\nZEN  = %-.4fø = ",90.-s->alt); dms(90.-s->alt);
printf ("\nAZ   = %-.4fø = ",s->az); dms(s->az);
printf ("\nSD   = %-.4fø = ",s->sd); dms(s->sd);
printf ("\nGMST = %-.4f hr",s->gmst);
printf ("\nGAST = %-.4f hr",s->gast);
printf ("\nGHAA = %-.4fø",s->ghaa);
printf ("\nLHAS = %-.4fø",s->lhas);
printf ("\nGHAS = %-.4fø",s->ghas);
printf ("\nEQT  = %-.4f sec = ",s->eqt); dms ((s->eqt)/3600.);
if (s->eqt < 0) printf ("  dial slow"); else printf ("  dial fast");
printf ("\nSUNRISE "); hms ("UT",s->utrise); pstpdt (s->utrise);
printf ("\nSUNSET  "); hms ("UT",s->utset); pstpdt (s->utset);
d=s->utset-s->utrise; p=100.*d/24.;
printf ("\nDAYLIGHT"); hms ("DH",d);
printf ("  =  %.2f %%\n",p);
}
/* tnorm..----------------------------------------------------------------- */

/* NORMALIZE TIME AND DATE */

int tnorm (struct sundat *s)

{
int mo[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
int m,d,y,ius;
dbl ut;

m=s->month, d=s->day, y=s->year, ut=s->ut;

if (y <= 0 || m < 1 || m > 12) goto eexit;
if (y % 4 == 0 && y % 100 != 0 || y % 400 == 0) mo[2]=29;
if (d < 1 || d > mo[m]) goto eexit;
ius=SGN(ut);

check: if (ut >= 0. && ut < 24.) goto nexit;
ut=ut-ius*24.; mo[2]=28;
if (y % 4) goto leap2;
mo[2]=29; if (y % 100) goto leap2;
if (!(y % 400)) goto leap2; else mo[2]=28;
leap2: if (ius < 0) goto back;
d++; if (d <= mo[m]) goto check;
m++; d=1; if (m <= 12) goto check;
m=1; y++; goto check;
back: d--; if (d > 0) goto check;
m--; if (m > 0) {d=mo[m]; goto check;}
m=12; y--; d=mo[m]; goto check;
nexit: s->month=m, s->day=d, s->year=y, s->ut=ut; return 0;
eexit: printf ("error in tnorm %i %i %i %g\n",m,d,y,ut); quit;
}
/* pstpdt..---------------------------------------------------------------- */

/* PRINT PST/PDT CORRESPONDING TO UT */

void pstpdt (dbl ut)

{
dbl pt;
char yt [4];

if ((pt=ut+s1.ost) < 0) {pt+=24.; strcpy (yt,"<ST");} else strcpy (yt,"ST");
hms (yt,pt);
if ((pt=ut+s1.odt) < 0) {pt+=24.; strcpy (yt,"<DT");} else strcpy (yt,"DT");
hms (yt,pt);
}
/* hms..------------------------------------------------------------------- */

/* print a time in h:m:s format */

void hms (char *label, dbl t)

{
int h,m,s;
real f;

h=t; f=FRAC(t)*3600.; m=f/60.; s=f-m*60.;
printf ("  %s = %02i:%02i:%02i",label,h,m,s);
}

/* dms..------------------------------------------------------------------- */

/* print an angle in d:m:s format */

void dms (dbl a)

{
int d,m,s;
char sgn=' ';
real f;

d=floor(ABS(a));
if (a < 0) sgn='-';
f=FRAC(ABS(a))*3600.; m=f/60.; s=f-m*60;
printf ("%1c%3i:%02i:%02i",sgn,d,m,s);
}

/* dtinit..---------------------------------------------------------------- */

/* initialize date and time array */

void dtinit (struct datetime *s)
{
char days [7] [4] = {"SUN","MON","TUE","WED","THU","FRI","SAT"};

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
s->uyear=today->tm_year;
if (s->uyear>50) s->uyear+=1900; else s->uyear+=2000;
s->uhour=gmt->tm_hour;
s->uminute=gmt->tm_min;
s->usecond=gmt->tm_sec;
s->ut=s->uhour+s->uminute/60.0+s->usecond/3600.0;
s->jd=jdnum (s->umonth,s->uday,s->uyear,s->ut);
}

/* jdnum..----------------------------------------------------------------- */

/*
GIVEN MONTH, DAY, YEAR AND UNIVERSAL TIME
 RETURN JULIAN DATE (JD) AND THE DAY OF THE WEEK (DOW)
VALID FOR 200 CENTURIES STARTING MARCH 1, 1 A.D.
THE FIRST MONTH OF EACH YEAR IS ASSUMED TO BE MARCH - THEREFORE
MONTHS AND YEARS ARE CORRECTED ACCORDINGLY
YEARS THAT ARE MULTIPLES OF 4000 ARE NOT LEAP YEARS
YEARS AFTER 1582 THAT ARE MULTIPLES OF 400 ARE LEAP YEARS
YEARS AFTER 1582 THAT ARE MULTIPLES OF 100 ARE NOT LEAP YEARS
YEARS THAT ARE MULTIPLES OF 4 ARE LEAP YEARS
THREE WEEKDAYS ARE ADDED IF THE DATE OCCURS AFTER OCTOBER 4, 1582
ONE TROPICAL YEAR = 365.242198 MEAN SOLAR DAYS
LENGTH OF CURRENT YEAR = 365.24250 MEAN SOLAR DAYS
AFTER 20000 YEARS A CORRECTION OF ONE DAY CHANGES THE LENGTH OF THE
 CALENDAR YEAR TO 365.242200 MEAN SOLAR DAYS
*/

double jdnum (int month, int day, int year, real ut)

{
int mond[]={31,29,31,30,31,30,31,31,30,31,30,31},dow;
double r0,r1,r2,r3,r4,ts,jd,jd0;

if (month<1 || month>12) {printf ("MONTH ERROR IN JDNUM"); quit;}
if (day<1 || day>mond[month-1]) {printf ("DAY ERROR IN JDNUM"); quit;}
if (year<1 || year>20001) {printf ("YEAR ERROR IN JDNUM"); quit;}
if (month == 2 && day == 29)
{
if ((year % 4)!=0 || ((year>1582 && (year % 4000)==0) || (year % 100)==0))
	{printf ("LEAP YEAR ERROR IN JDNUM"); quit;}
}

r4=year+.0001*day+.01*month; r0=year; r1=month-2; r2=day; r3=day;
if (r1<=0) {r1=r1+12; r0=r0-1;}
r3=r3+3+floor(30.6*(r1-1)+.5)+365.25*r0-floor(.00025*r0);
if (r4>1582.1005) r3=r3-floor((floor(.01*r0)-3.)*.75+1.);
jd0=floor(r3+1721114.)-.5; jd=jd0+ut/24.; 
ts=(r3-4.)/7.; dow=(int)floor(7.*(ts-floor(ts))+1.1);
return jd;
}

/* menu..------------------------------------------------------------------ */

void menu (void)

{
puts ("A  Sun data for current time");
puts ("B  Sun data for any input time");
puts ("E  EQT minimum (near local noon) for input month");
puts ("F  Fall Equinox");
puts ("H  (or spacebar) Help - print this menu");
puts ("I  Information");
puts ("M  Meridian transit (local solar noon)");
puts ("Q  (or Esc) Quit");
puts ("S  Summer Solstice");
puts ("V  Vernal Equinox");
puts ("W  Winter Solstice");
}

/* csep..------------------------------------------------------------------ */

/* print a number with comma separators */
/* note: format must be of form %-ww.ddf */

void csep (real x, char format[])

{
char buf[50],*b,*l,num[50];

sprintf (buf,format,x);
if ((l=strrchr(buf,'.')) != NULL) l-=3;
else {l=&buf[0]+strlen(buf); while (!isdigit(*l)) l--; l-=2;}
for (b=l ; b>&buf[0] ; b-=3)
	{
	if isdigit ((int)(*(b-1))) {strcpy (num,b); *b=','; strcpy (b+1,num);}
	}
printf ("%s",buf);
}

/* sd..-------------------------------------------------------------------- */

/*
sundial calculations

IF E,N,U are unit vectors along East(x), North(y) and Up(z)
 and P is a unit vector normal to the plane, then:

ALPHA = ANGLE OF SHADOW wrt N x P
BETA = ANGLE OF SHADOW wrt P x E
GAMMA = ANGLE OF SHADOW wrt P

Note that these "wrt" axes may have to be changed if P lies in the N-E plane.

*/

void sd (struct sundial *p)

{
real ts,as,bs,cs,ap,bp,cp,ag,bg,cg,delta,ls,ab,ac,bc,k,x,y,z;

ts=COSD(p->alt);
as=ts*SIND(p->az); bs=ts*COSD(p->az); cs=SIND(p->alt);
ts=COSD(p->thetap);
ap=ts*SIND(p->phip); bp=ts*COSD(p->phip); cp=SIND(p->thetap);
ts=COSD(p->thetag);
ag=ts*SIND(p->phig); bg=ts*COSD(p->phig); cg=SIND(p->thetag);
p->alpha=1.e10; p->beta=1.e10; p->gamma=1.e10; ls=1.e10;
delta=ap*as+bp*bs+cp*cs;
if (delta != 0.0)
{
	ab=ag*bs-as*bg; ac=ag*cs-as*cg; bc=bg*cs-bs*cg; k=1./delta;
	x=k*(bp*ab+cp*ac); y=k*(cp*bc-ap*ab); z=-k*(ap*ac+bp*bc);
	ls=RSS3(x,y,z);
	p->alpha=ACSD((x*cp-z*ap)/(ls*RSS2(ap,cp)));
	p->beta=ACSD((y*cp-z*bp)/(ls*RSS2(bp,cp)));
	p->gamma=ACSD((x*ap+y*bp+z*cp)/ls);
	p->ratio=ls;
}
}

/* sdp..------------------------------------------------------------------- */

/* print sundial information */

void sdp (struct sundial *p)

{
printf ("alpha (wrt N x P) = %.2fø",p->alpha);
printf (" | beta (wrt P x E) = %.2fø\n",p->beta);
printf ("gamma (wrt P) = %.2fø",p->gamma);
printf (" | shadow/(gnomon length) ratio = %.3f\n",p->ratio);
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
/* rinput..---------------------------------------------------------------- */

/* get real input from user with prompt and default value */

void rinput (char prompt[],float defalt,float *data)
{
char string[80];
fprintf (_out,"%s [%g] ? ",prompt,defalt);
fgets(string,sizeof (string),_in);
if (sscanf (string,"%f",data) != 1) *data=defalt;
}
/* normab..---------------------------------------------------------------- */

/* normalize value between a and b
(the quantity (b-a) must represent a 'revolution') */

double normab (double x, double a, double b)

{double z=b-a; while (x<a) {x+=z;}  while (x>b) {x-=z;} return x;}
/* info..------------------------------------------------------------------ */

void info (void)

{
cls;
puts ("UT   = Universal Time");
puts ("RA   = Right Ascension of sun");
puts ("DEC  = Declination of sun");
puts ("DIST = earth-sun Distance");
puts ("ALT  = Altitude of sun (angle above horizontal <0 => sun not visible)");
puts ("ZEN  = Zenith angle of sun (angle below vertical)");
puts ("AZ   = Azimuth of sun (N=0, E=90, S=180, W=270)");
puts ("SD   = apparent Semi-Diameter of sun");
puts ("GMST = Greenwich Mean Siderial Time");
puts ("GAST = Greenwich Apparent Siderial Time");
puts ("GHAA = Greenwich Hour Angle of Aries");
puts ("LHAS = Local Hour Angle of Sun");
puts ("GHAS = Greenwich Hour Angle of Sun");
puts ("EQT  = Equation of Time");
puts ("ST   = Local Standard Time");
puts ("DT   = Local Daylight Savings Time");
puts ("DH   = Hours of Daylight on indicated day");
puts ("\nIf standard or daylight savings times are preceeded by '<' (e.g. <ST)");
puts (" they refer to that time on the previous day.\n");
pausem;
}
/* rdata..----------------------------------------------------------------- */

/* read the data file */

char rdata (void)

{
char temp[81],f;
int i;
dbl dat[ND];

if ((fp=fopen(dfile,"rt")) == NULL)
	{printf ("FAILED TO OPEN DATA FILE: %s\n",dfile); quit;}

nd=0;			/* number of data entries */
f=0;
while ((fgets(temp,sizeof(temp),fp)) != NULL)
	{
	trim(temp);		/* uses library trim */
	if ((strstr(temp,"STARTOFDATA")) != NULL) {f=1; continue;}
	if (!f) continue;
	if ((strstr(temp,"ENDOFDATA")) != NULL) break;
	if (temp[0] != '\0' && temp[0] != ';')
		{
		/* decode and store data here */
		dat[nd]=atof(strtok(temp,",\t;"));
		if (++nd == ND) goto done;
		}
	}

done:
s1.lat=dat[0]; s1.lon=dat[1]; s1.ost=dat[2]; s1.odt=dat[3];

fclose (fp);
return 0;
}
/* caz..------------------------------------------------------------------- */

/* compute relative azimuth */

dbl caz (dbl lat1, dbl lon1, dbl lat2, dbl lon2)

{
dbl slat1,clat1,slat2,clat2;
dbl A,cosA,sinA,cosb,cosc,az;
slat1=SIND(lat1); slat2=SIND(lat2); clat1=COSD(lat1); clat2=COSD(lat2);
cosA=slat1*slat2+clat1*clat2*COSD(ABS(lon2-lon1));
A=ACSD(cosA); sinA=SIND(A);
cosb=(slat2-slat1*cosA)/(clat1*sinA);
az=ACSD(cosb);
if (lon1 == lon2) {if (lat1 < lat2) az=0.; else az=180.;}
if (lon2 > lon1) az=360.-az;
if (ABS(lon2-lon1) > 180.) az=360.-az;
return az;
}
/* ------------------------------------------------------------------------ */
