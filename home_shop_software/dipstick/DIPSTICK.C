/* DIPSTICK.C   M. W. Klotz   4/01, 9/03, 4,7/04, 7,9/05

Calculate fraction of liquid volume remaining in tanks of various shapes
as function of wetted dipstick height.

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
dbl fcyl (dbl h);
dbl fspher (dbl h);
dbl fellip (dbl h);
dbl fvcart (dbl h);
dbl fhcart (dbl h);
dbl binsearch (dbl l, dbl r, dbl val, dbl tol);
dbl fbuck (dbl h);
dbl fbarrel (dbl h);
dbl fhcyl (dbl h);
dbl fdcyl (dbl h);
dbl vdish (dbl h, dbl dishr, dbl dishh);

/* gmark..-------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* vmark..------------ global variable declarations ----------------------- */
char op;		// selected option
dbl h;			// wetted dipstick length
dbl d;			// maximum wetted length
dbl r;			// radius of circle/sphere
dbl a,b;		// semi-major/minor axes of ellipse
dbl l;
dbl tarea;		// total area
dbl f;
dbl tvol;		// total volume
dbl db,ds,hb,rb,rs,theta,t;
dbl l1,l2,vd,hdish;

/* smark..----------- global structure declarations ----------------------- */

/* main..================================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i,j,k;
dbl df,inc;

/* -------------------------- initializations ------------------------------ */
/* ------------------------------------------------------------------------- */

puts ("TANK VOLUME FRACTION FROM DIPSTICK READING\n");

puts ("A - horizontal cylindrical tank");
puts ("B - spherical tank");
puts ("C - elliptical tank");
puts ("D - vertical cartouche");
puts ("E - horizontal cartouche");
puts ("F - bucket (conic frustum)");
puts ("G - barrel (elliptical sides)");
puts ("H - horizontal cylindrical tank with hemispherical ends");
puts ("I - horizontal cylindrical tank with dished ends");

agin: op=getkey("(A-I) ? ");

switch (op)
{
	case 'a':	/* horizontal cylindrical tank */
		vin ("Tank diameter",&d,"lf",10.,"");
		r=0.5*d; tarea=PI*r*r;
		break;
	case 'b':	/* spherical tank */
		vin ("Tank diameter",&d,"lf",10.,"");
		r=0.5*d;
		tvol=4.*PI*r*r*r/3.;
		printf ("Total volume of tank = %.3lf\n",tvol);
		break;
	case 'c':	/* elliptical tank */
		vin ("Tank horizontal dimension",&a,"lf",10.,"");
		vin ("Tank vertical dimension",&b,"lf",7.,"");
		d=b; a*=0.5; b*=0.5; tarea=PI*a*b;
		break;
	case 'd':	/* vertical cartouche */
		vin ("Tank horizontal dimension",&a,"lf",4.,"");
		vin ("Tank vertical dimension",&b,"lf",10.,"");
		d=b; r=0.5*a; l=b-a; tarea=PI*r*r+l*a;
		break;
	case 'e':	/* horizontal cartouche */
		vin ("Tank horizontal dimension",&a,"lf",10.,"");
		vin ("Tank vertical dimension",&b,"lf",4.,"");
		d=b; r=0.5*b; l=a-b; tarea=PI*r*r+l*b;
		break;
	case 'f':	/* bucket */
		vin ("Diameter of bucket big end",&db,"lf",4.,"");
		vin ("Diameter of bucket small end",&ds,"lf",3.,"");
		vin ("Vertical height of bucket",&hb,"lf",6.,"");
		rb=0.5*db; rs=0.5*ds; d=hb;
		t=rs*rs+rs*rb+rb*rb; tvol=PI*hb*t/3.;
		printf ("Total volume of bucket = %.3lf\n",tvol);
		break;
	case 'g':	/* barrel */
		vin ("Diameter of barrel at middle",&db,"lf",8.,"");
		vin ("Diameter of barrel at end(s)",&ds,"lf",6.,"");
		vin ("Vertical height of barrel",&hb,"lf",5.,"");
		rb=0.5*db; rs=0.5*ds; d=hb;
		tvol=PI*hb*(2.*rb*rb+rs*rs)/3.;
		printf ("Total volume of barrel = %.3lf\n",tvol);
		break;
	case 'h':	/* horizontal cylindrical tank with hemispherical ends */
		vin ("Tank diameter",&d,"lf",10.,"");
		vin ("Tank length",&a,"lf",20.,"");
		r=0.5*d; tvol=PI*r*r*(a-d+4*r/3);
		printf ("Total volume of tank = %.3lf\n",tvol);
		break;
	case 'i':	// horizontal cylindrical tank with dished ends
		vin ("Tank diameter",&d,"lf",100.,"");
		vin ("(Maximum) Tank length over ends",&l1,"lf",1000.,"");
		vin ("Length of cylindrical portion of tank",&l2,"lf",800.,"");
		r=0.5*d;	// tank radius
		hdish=0.5*(l1-l2);	// dish 'height'
		vd=PI*hdish*(3.*r*r+hdish*hdish)/6.;  // dish volume
		tvol=PI*r*r*l2+2.*vd;
		printf ("Total volume of tank = %.3lf\n",tvol);
		break;
	default:
		beep; puts ("NOT A VALID OPTION"); goto agin;
}

agin1:
vin ("Wetted dipstick length",&h,"lf",d/3.,"");
if (h<0 || h>d)
	{beep; puts ("INPUT MAKES NO SENSE. Try again."); goto agin1;}

switch (op)
{
	case 'a':	/* horizontal cylindrical tank */
		f=fcyl (h);
		break;
	case 'b':	/* spherical tank */
		f=fspher (h);
		break;
	case 'c':	/* elliptical tank */
		f=fellip (h);
		break;
	case 'd':	/* vertical cartouche */
		f=fvcart (h);
		break;
	case 'e':	/* horizontal cartouche */
		f=fhcart (h);
		break;
	case 'f':	/* bucket */
		f=fbuck (h);
		break;
	case 'g':	/* barrel */
		f=fbarrel (h);
		break;
	case 'h':	// cylindrical tank with hemispherical ends
		f=fhcyl (h);
		break;
	case 'i':	// cylindrical tank with dished ends
		f=fdcyl (h);
		break;
}

printf ("\nTank has %.2lf%% of its full capacity remaining\n",100.*f);

puts ("\nCalibration of dip stick as function of tank volume.\n");

stupid:
vin ("Percentage increment of tank volume",&inc,"lf",10.,"%");
if (inc<=0. || inc>100.)
	{beep; puts ("INPUT MAKES NO SENSE. Try again."); goto stupid;}

for (f=inc ; f<=100.-inc ; f+=inc)
{
h=binsearch (0.,d,0.01*f,0.00001);
printf ("Tank volume = %.2lf%%, stick wetted length = %.2lf\n",f,h);
}
printf ("Tank volume = %.2lf%%, stick wetted length = %.2lf\n",100.,d);

/* post run processing */
mexit:
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* binsearch..------------------------------------------------------------- */

/* 
search the function 'func' between 'l' and 'r' for the value 'val' with
tolerance 'tol'
*/

dbl binsearch (dbl l, dbl r, dbl val, dbl tol)

{
int i=0;
dbl x,y;

agin:
x=l+0.5*(r-l);
switch (op)
{
	case 'a':	/* horizontal cylindrical tank */
		y=fcyl (x);
		break;
	case 'b':	/* spherical tank */
		y=fspher (x);
		break;
	case 'c':	/* elliptical tank */
		y=fellip (x);
		break;
	case 'd':	/* vertical cartouche */
		y=fvcart (x);
		break;
	case 'e':	/* horizontal cartouche */
		y=fhcart (x);
		break;
	case 'f':	/* bucket */
		y=fbuck (x);
		break;
	case 'g':	/* barrel */
		y=fbarrel (x);
		break;
	case 'h':	// cylindrical tank with hemispherical ends
		y=fhcyl (x);
		break;
	case 'i':	// cylindrical tank with dished ends
		y=fdcyl (x);
		break;
}
#if 0 == 1
if (val == 0.6 && i>15 && i<30)
{
printf ("x,y = %.4lf,%.4lf\n",x,y);
}
#endif
if (ABS(y-val) <= tol) return x;
if (val < y) r=x; else l=x;
if (++i>500)
{
#if 0 == 1
printf ("r,l = %.4lf,%.4lf\n",r,l);
printf ("x,y,val = %.4lf,%.4lf,%.4lf\n",x,y,val);
#endif
return 0;
}
goto agin;
}
/* fcyl..----------------------------------------------------------------- */

dbl fcyl (dbl h)

{
dbl z,angle,chord,area;

z=r-h;
angle=2.*acos(z/r);
chord=2.*r*sin(0.5*angle);

area=0.5*(r*r*angle-z*chord);
return area/tarea;
}
/* fspher..---------------------------------------------------------------- */

dbl fspher (dbl h)

{
return h*h*(3*r-h)/(4*r*r*r);
}
/* fellip..---------------------------------------------------------------- */

dbl fellip (dbl h)

{
dbl area;

area=(a/b)*((h-b)*sqrt(h*(2*b-h))+b*b*(asin((h-b)/b)+0.5*PI));
return area/tarea;
}
/* fvcart..---------------------------------------------------------------- */

dbl fvcart (dbl h)

{
dbl z,angle,chord,area;

if (h<=r)
	{
	z=r-h;
	angle=2.*acos(z/r);
	chord=2.*r*sin(0.5*angle);
	area=0.5*(r*r*angle-z*chord);
	goto done;
	}

if (h>r && h<=(r+l))
	{
	area=0.5*PI*r*r+a*(h-r);
	goto done;
	}

if (h>(r+l))
	{
	z=r-(h-l);
	angle=2.*acos(z/r);
	chord=2.*r*sin(0.5*angle);
	area=0.5*(r*r*angle-z*chord)+l*a;
	goto done;
	}

done: return area/tarea;
}
/* fhcart..---------------------------------------------------------------- */

dbl fhcart (dbl h)

{
dbl z,angle,chord,area;

z=r-h;
angle=2.*acos(z/r);
chord=2.*r*sin(0.5*angle);
area=0.5*(r*r*angle-z*chord)+h*l;
return area/tarea;
}
/* fbuck..----------------------------------------------------------------- */

dbl fbuck (dbl h)

{
dbl z,vol,r;

r=rs+(rb-rs)*h/hb;
z=rs*rs+rs*r+r*r;
vol=PI*h*z/3.;

return vol/tvol;
}
/* fbarrel..--------------------------------------------------------------- */

dbl fbarrel (dbl h)

{
dbl vol;

vol=PI*(rb*rb*h+(rs*rs-rb*rb)*(hb*hb*h-2.*hb*h*h+4.*h*h*h/3.)/(hb*hb));

return vol/tvol;
}
/* fhcyl..----------------------------------------------------------------- */

dbl fhcyl (dbl h)

{
dbl z,angle,chord,area,vc,vh;

z=r-h;
angle=2.*acos(z/r);
chord=2.*r*sin(0.5*angle);

area=0.5*(r*r*angle-z*chord);
vc=area*(a-d);				// volume of cylindrical part
vh=PI*h*h*(r-h/3);			// volume of hemisphere
return (vc+vh)/tvol;
}
/* fdcyl..----------------------------------------------------------------- */

dbl fdcyl (dbl h)

{
dbl z,angle,chord,area;

z=r-h;
angle=2.*acos(z/r);
chord=2.*r*sin(0.5*angle);
area=0.5*(r*r*angle-z*chord);

return (area*l2+2.*vdish(h,r,hdish))/tvol;
}
/* vdish..----------------------------------------------------------------- */

/* 

find volume of dished end filled to height 'h'  (numerical integration)

*/

dbl vdish (dbl h, dbl dishr, dbl dishh)

{
char flag;
dbl hp,rho,drho,vd,R,rp,area,angle,chord,z,tv,c;

if (h > dishr) {flag=1; hp=2.*r-h;} else {flag=0; hp=h;}

tv=PI*dishh*(3.*dishr*dishr+dishh*dishh)/6.;    // total dish volume
vd=0;					// wetted dish volume
drho=0.001*dishh;			//step size
R=0.5*(dishr*dishr+dishh*dishh)/dishh;	// radius of curved part of dish

z=dishr-hp;

for (rho=0. ; rho<=dishh ; rho+=drho)
{
rp=sqrt(R*R-SQR(R-dishh+rho));
if (ABS(z) > rp) break;
angle=2.*acos(z/rp);
chord=2.*rp*sin(0.5*angle);
area=0.5*(rp*rp*angle-z*chord);
vd+=area*drho;
}  // end rho loop

if (flag) return tv-vd; else return vd;

}
/* ------------------------------------------------------------------------ */
