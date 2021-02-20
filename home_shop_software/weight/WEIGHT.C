/* WEIGHT.C   M. W. Klotz  12/99, 4/02, 3,4,5,7/03, 2,12/04, 7,8/05, 10/09

Calculate weight from volume of standard shapes and density of material.

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
int acomp (const void *v1,const void *v2);
int mselect (void);
char rdata (void);
dbl wedge (dbl h, dbl s, dbl r);

/* gmark..-------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* vmark..------------ global variable declarations ----------------------- */
FILE *fp;			/* utility file pointer */
char dfile[]="WEIGHT.DAT";	/* input data file */
#define ND 200			/* maximum number of data items */
int nd;				/* actual number of data items */

char name[ND][40];		/* material name */
dbl density[ND];		/* material densities (lb/in^3) */

char unit[]="in";		// input units
dbl vfact;			// volume conversion factor
int mid;			/* material index */
int sid;			/* shape index */
dbl smul;			// shape multiplier for total weight
dbl volume;			/* volume of shape */
dbl tvolume;			// total volume of shapes
dbl rho;			/* density of material (lb/in^3) */
dbl weight;			/* weight (lb) */
dbl tweight;			// total weight of shapes

/* smark..----------- global structure declarations ----------------------- */

/* main..================================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
char op;
int i,j,k,n,even;
dbl x,y,z,d,r,h,d1,d2;
dbl ang,daf,dfv,f,c,s,p,area,dc,di,ca,sa,l,pp,pm,pp2,pm2;

/* -------------------------- initializations ------------------------------ */
textscreen();
/* ------------------------------------------------------------------------- */

puts ("WEIGHT CALCULATIONS\n");

tvolume=0; tweight=0;

rdata ();		/* read the data file */
printf ("Number of data items read = %d\n",nd);

newmat:
mid=mselect();		/* select material */
rho=density[mid];

printf ("\nDensity of %s = %.4lf lb/in^3 = %.4lf gm/cc\n",\
name[mid],rho,rho/0.036127);


op=getkey("Dimensions of shapes in [i]n, (f)t, (m)m, (c)m ? ");
if ((op != 'f') && (op != 'm')  && (op != 'c')) op='i';

switch (op)
{
case 'i':
	strcpy (unit,"in");
	vfact=1.;
	break;

case 'f':
	strcpy (unit,"ft");
	vfact=1728.;
	break;

case 'm':
	strcpy (unit,"mm");
	vfact=1./ATOB(25.4,3.);
	break;

case 'c':
	strcpy (unit,"cm");
	vfact=1./ATOB(2.54,3.);
	break;
}

agin:
puts ("\n-1 select new material");
puts (" 0 QUIT");
puts (" 1 Rectangular solid");
puts (" 2 Cylindrical solid");
puts (" 3 Cylindrical pipe");
puts (" 4 Hexagonal solid");
puts (" 5 Pyramid");
puts (" 6 Cone");
puts (" 7 Frustum of a cone");
puts (" 8 Sphere");
puts (" 9 Annulus (washer shape)");
puts ("10 Regular polygon cross section");
puts ("11 Spherical cap");
puts ("12 Volume formed by two intersecting cylinders");
puts ("13 Torus");
puts ("14 Hemispherical shell");
puts ("15 Tangent ogive");
puts ("16 Conical wedge");
puts ("99 User Volume Input\n");

vin ("Shape number",&sid,"d",1.,"");

switch (sid)
{
case -1:
	goto newmat;
case 0:
	goto mexit;

case 1:		/* rectangular solid */
	vin ("Length",&x,"lf",12.,unit);
	vin ("Width",&y,"lf",12.,unit);
	vin ("Height",&z,"lf",12.,unit);
	volume=x*y*z;
	break;
	
case 2:		/* cylinder */
	vin ("Diameter",&d,"lf",1.,unit);
	vin ("Length",&h,"lf",12.,unit);
	volume=PI*d*d*h/4;
	break;

case 3:		/* pipe */
	vin ("Outside Diameter",&d1,"lf",1.5,unit);
	vin ("Inside Diameter",&d2,"lf",1.,unit);
	vin ("Length",&h,"lf",12.,unit);
	volume=PI*(d1*d1-d2*d2)*h/4;
	break;

case 4:		/* hexagonal stock */
	vin ("Size across flats",&d,"lf",1.,unit);
	vin ("Length",&h,"lf",12.,unit);
	volume=1.5*d*d*h*TAND(30.);
	break;

case 5:		/* pyramid */
	vin ("Length of base side",&d,"lf",1.,unit);
	vin ("Altitude",&h,"lf",12.,unit);
	volume=d*d*h/3;
	break;

case 6:		/* cone */
	vin ("Diameter of base",&d,"lf",1.,unit);
	vin ("Altitude",&h,"lf",12.,unit);
	volume=PI*d*d*h/12;
	break;

case 7:		/* frustum of a cone */
	vin ("Larger Diameter",&d1,"lf",2.,unit);
	vin ("Smaller Diameter",&d2,"lf",1.,unit);
	vin ("Altitude",&h,"lf",12.,unit);
	volume=PI*(d1*d1+d1*d2+d2*d2)*h/12;
	break;

case 8:		/* sphere */
	vin ("Diameter",&d,"lf",1.,unit);
	volume=PI*d*d*d/6;
	break;

case 9:		/* annulus */
	vin ("Outer Diameter",&d1,"lf",1.,unit);
	vin ("Hole Diameter",&d2,"lf",0.5,unit);
	vin ("Thickness",&h,"lf",0.1,unit);
	volume=0.25*PI*h*(d1*d1-d2*d2);
	break;

case 10:	/* polygonal cross section */

	agin10: vin ("Number of polygon sides",&n,"d",6.,"");
	if (n<3)
		{
		beep;
		puts ("\nNOT A POLYGON, TRY AGAIN");
		goto agin10;
		}
	even=!fmod((dbl)n,2.);
	ang=360./n; ca=COSD(0.5*ang); sa=SIND(0.5*ang);

	if (even)
		{
		vin ("Size across flats",&daf,"lf",1.,"");
		r=daf/(2.*ca);
		}
	else
		{
		vin ("Size flat-to-opposite-vertex",&dfv,"lf",1.,"");
		r=dfv/(1.+ca);
		}
	vin ("Length",&h,"lf",12.,unit);
	f=r*ca;
	c=2.*r*sa;
	s=r-f;			//sagitta
	volume=0.5*c*f*n*h;
	break;

case 11:	/* spherical cap */
	vin ("Diameter of sphere",&d,"lf",1.,unit);
	vin ("Height of cap at center",&h,"lf",.5,unit);
	volume=PI*h*h*(1.5*d-h)/3;
	break;

case 12:	/* volume of two intersecting cylinders */
	vin ("Diameter of cylinder(s)",&d,"lf",1.,unit);
	volume=16*d*d*d/24;
	break;

case 13:	// torus
	vin ("Diameter of torus cross section",&d,"lf",1.5,unit);
	vin ("Outside Diameter of torus",&d1,"lf",6.5,unit);
	d2=d1-d;
	volume=0.25*PI*PI*d2*d*d;
	break;

case 14:	// hemispherical shell
	vin ("Outside diameter of shell",&d2,"lf",3.,unit);
	vin ("Inside diameter of shell",&d1,"lf",1.,unit);
	volume=(PI/12.)*(d2*d2*d2-d1*d1*d1);
	break;

case 15:	// tangent ogive
	vin ("Diameter of ogive base",&d,"lf",2.,unit);
	vin ("Length/height of ogive",&l,"lf",5.,unit);
	c=l/d;
	pp=d*(c*c+0.25); pm=d*(c*c-0.25); pp2=pp*pp; pm2=pm*pm;
	volume=PI*((pp2+pm2)*l-l*l*l/3.-pm*(l*sqrt(pp2-l*l)+pp2*asin(l/pp)));
	break;

case 16:	// conical wedge
	vin ("Cone base diameter",&d,"lf",2.,"");
	r=0.5*d;
	vin ("Height of cone",&h,"lf",10.,"");
	vin ("Sagitta of wedge base",&s,"lf",0.5,"");
	volume=wedge(h,s,r);
	break;

case 99:	/* user input */
	vin ("Volume of shape",&volume,"lf",1728.,"in^3");
	break;

default:
	beep; puts ("NOT A VALID OPTION");
	goto agin;

}
weight=rho*volume*vfact;

//d=2.54*2.54*2.54;
printf ("\nDensity of %s = %.4lf lb/in^3 = %.4lf gm/cc\n",\
name[mid],rho,rho/0.036127);
printf ("Volume of shape = %.4lf %s^3\n",volume,unit);
printf ("Weight of shape = %.4g lb = %.4g kg\n",weight,weight/2.20462);
vin ("Number of these shapes to add to total",&smul,"lf",1.,"");
tvolume+=smul*volume; tweight+=smul*weight;
printf ("Total volume = %.4lf %s^3\n",tvolume,unit);
printf ("Total weight = %.4g lb = %.4g kg\n",tweight,tweight/2.20462);
pausem; goto agin;

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
char temp[81],f;
int i;

if ((fp=fopen(dfile,"rt")) == NULL)
	{printf ("FAILED TO OPEN DATA FILE: %s\n",dfile); beep; return 1;}

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
		strcpy (name[nd],strtok(temp,",\t;"));
		density[nd]=(dbl)atof(strtok(NULL,",\t;"));
		if (++nd == ND)
			{
			printf
			("More than maximum (%d) data items in data file\n",ND);
			quit;
			}
		}
	}

do		/* sort data */
{
for (i=0,f=0 ; i<nd-1 ; i++)
{
if (acomp(name[i],name[i+1]) > 0)
	{
	f=1;
	swap (name[i],name[i+1],MAX(strlen(name[i]),strlen(name[i+1])));
	swap (&density[i],&density[i+1],sizeof(density[i]));
	}
}
} while (f);

fclose (fp);
return 0;
}

/* acomp..----------------------------------------------------------------- */

/* string variable comparison function used by library version of qsort
	for alphabetizing strings of variable length

v1<v2	=>	acomp<0
v1=v2	=>	acomp=0
v1>v2	=>	acomp>0
*/

int acomp (const void *v1,const void *v2)

{
int i,k,l1,l2,l;
char *p1,*p2;

l1=strlen (p1=(char*)v1); l2=strlen (p2=(char*)v2); l=MIN(l1,l2);
for (i=0 ; i<l ; i++) {if (k=tolower(*(p1+i))-tolower(*(p2+i))) return k;}
return SGN(l1-l2);
}
/* mselect..--------------------------------------------------------------- */

/* select the material for weight calculation */

int mselect (void)

{
int i;

for (i=0 ; i<nd ; i++)
  printf ("%s-%d  ",name[i],i+1);
printf ("\n%s-%d","User input",nd+1);


vin ("\nMaterial number",&i,"d",(real)(nd+1),"");
i--;
if (i > nd-1 || i<0) i=nd;

if (i == nd)
{
vin ("Material density",&density[nd],"lf",1.0,"lb/in^3");
strcpy (name[nd],"??");
}
return i;
}
/* wedge..---------------------------------------------------------------- */

/* 
volume of conical wedge shape of height h and sagitta s and base radius r
*/

dbl wedge (dbl h, dbl s, dbl r)

{
dbl a,c,p,t,st,ct,tt,vu,vc;

vc=PI*r*r*h/3.;

if (s == r) return 0.5*vc;

if (s<r) p=r-s; else p=s-r;
ct=p/r; t=acos(ct); st=sin(t); tt=tan(t);
vu=h*r*r*(t-2*st*ct+ct*ct*ct*log(tt+1./ct))/3.;

if (s<=r) return vu; else return vc-vu;
}
/* ------------------------------------------------------------------------ */
