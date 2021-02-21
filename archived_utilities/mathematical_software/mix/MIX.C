/* MIX.C   M. W. Klotz   2/99, 4/02, 12/03

Mixed dimensional units four-function calculator

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
void vp (dbl x);
int dplaces (dbl x);
void help (void);
char process (void);
void display (void);
dbl gcd (dbl x, dbl y);

/* gmark..-------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* vmark..------------ global variable declarations ----------------------- */
char gp[80];		/* general purpose buffer */
char blank[80];		/* blanking buffer */
char *bp,*cp;		/* general purpose buffer pointers */
int qrow=12;		/* query row */
int qf=15,qb=2;		/* query colors */
int mrow=24;		/* message row */
int mf=11,mb=2;		/* message colors */
int drow=14;		/* display row */
int df=14,db=2;		/* display colors */
char dtype=0;		/* display flag  0=fixed, 1=engineering, 2=scientific */
int decimals=3;		/* number of decimal places */
char dflag=0;		/* decimals displayed 0=decimals, 1=self adjusting */
#define NU 7		/* number of units */
char udef=6;		/* default units */
/* 0=none, 1=m, 2=cm, 3=mm, 4=ft, 5=in,  */
char ud[NU][11]={"none","meter","centimeter","millimeter","yard","feet","inch"};
char uda[NU][3]={"nd","m\0","cm","mm","yd","ft","in"};
/* unit scale factors */
dbl usf[NU]={1.,.0254,2.54,25.4,0.02777777,0.08333333333,1.};
int uexp=0;		/* units exponent */
int op;			/* operator +,-,*,/ */
#define NA NU-1		/* number of accumulators */
dbl acc[NA];		/* accumulators for each unit */
dbl uacc[NA];		/* undo accumulators */
int uuexp;		/* undo exponent */
dbl fracd=64;		/* fractional inches precision */
dbl scale=1.;		/* scale factor */

/* smark..----------- global structure declarations ----------------------- */

/* main..================================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i,j,k;

/* -------------------------- initializations ------------------------------ */
textscreen ();
strcpy (blank,\
"                                                                            ");
for (i=0 ; i<NA ; i++) acc[i]=0.;
usf[4]=1./36.;		/* for precision */
usf[5]=1./12.;		/* for precision */
help ();
/* ------------------------------------------------------------------------- */

agin:

display ();
_settextposition (1,1);
puts ("SYNTAX: op=+,-,*,/ ; entry = 7, .2, 1.23, 2/3, 1 & 2/3");
puts ("unit designators (ud) = nd,m,cm,mm,yd,ft,in");
puts ("\nCOMMANDS: clear; decimals=(0-15); dflag=(0-1); dtype=(0-2); exit");
puts ("fracd=n; help; quit; scale=n; undo; unit=(ud)");
puts ("\nDATA ENTRY: [op] entry [ud]");

ptext (blank,qrow-3,1,14,2);
sprintf (gp,"Fractional approximation accuracy = 1/%.0lf in\n",fracd);
ptext (gp,qrow-3,1,14,2);

ptext (blank,qrow-2,1,14,2);
sprintf (gp,"Scale factor = %.5lf\n",scale); ptext (gp,qrow-2,1,14,2);

ptext (blank,qrow-1,1,14,2);
sprintf (gp,"Default unit = %s\n",ud[udef]); ptext (gp,qrow-1,1,14,2);

ptext (blank,qrow,1,qf,qb);
ptext ("?",qrow,1,qf,qb); _settextposition (qrow,4);
gets (gp); strlwr(gp);

if (strstr(gp,"quit")) goto mexit;
if (strstr(gp,"exit")) goto mexit;
if (strstr(gp,"help")) {help (); goto agin;}
if (strstr(gp,"clear")) {for (i=0 ; i<NA ; i++) acc[i]=0.; uexp=0; goto agin;}
if (strstr(gp,"unit"))
{
bp=strchr(gp,'=');  if (bp == NULL) goto error;
for (i=0; i<NU ; i++) if (strncmp(bp+1,uda[i],2) == 0) {udef=i; goto agin;}
goto error;
}
if (strstr(gp,"decimals"))
{
bp=strchr(gp,'=');  if (bp == NULL) goto error;
cp=strchr(gp,'\0')-1;
decimals=(int)strtod(bp+1,&cp);
if (decimals > 15) decimals=15;
if (decimals < 0) decimals=0;
goto agin;
}
if (strstr(gp,"dflag"))
{
bp=strchr(gp,'=');  if (bp == NULL) goto error;
cp=strchr(gp,'\0')-1;
dflag=(int)strtod(bp+1,&cp);
goto agin;
}
if (strstr(gp,"dtype"))
{
bp=strchr(gp,'=');  if (bp == NULL) goto error;
cp=strchr(gp,'\0')-1;
dtype=(int)strtod(bp+1,&cp);
goto agin;
}
if (strstr(gp,"fracd"))
{
bp=strchr(gp,'=');  if (bp == NULL) goto error;
cp=strchr(gp,'\0')-1;
fracd=(int)strtod(bp+1,&cp);
goto agin;
}
if (strstr(gp,"undo"))
{
for (i=0 ; i<NA ; i++) acc[i]=uacc[i]; uexp=uuexp;
goto agin;
}
if (strstr(gp,"scale"))
{
bp=strchr(gp,'=');  if (bp == NULL) goto error;
cp=strchr(gp,'\0')-1;
scale=strtod(bp+1,&cp);
goto agin;
}

if (process ()) goto error;
goto agin;

error:
ptext (blank,mrow,1,mf,mb); ptext ("INPUT ERROR - press a key",mrow,1,mf,mb);
beep; pause; ptext (blank,mrow,1,mf,mb);
goto agin;

/* post run processing */
mexit:
_settextposition (24,1);
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* vp..-------------------------------------------------------------------- */

/* generalized variable print routine */

void vp (dbl x)

{
char buf[40],b2[40],fp[20],*l,*b,dsave;
int k;
dbl xd,zd;

if (x) zd=log10(fabs(x)); else zd=1.;
dsave=dtype; if (fabs(zd) > 15 && dtype == 0) dtype=1;
itoa(dplaces(x),b2,10);

if (dtype == 0)		/* fixed point display */
	{
	strcpy (fp,"%.");
	strcat (fp,b2); strcat (fp,"lf");
	sprintf (buf,fp,x);
	if (fabs(x) >= 1000.)		/* add comma separators */
		{
		if ((l=strrchr(buf,'.'))) l-=3;
  		  else {l=&buf[0]+strlen(buf); while (!isdigit(*l)) l--; l-=2;}
		for (b=l ; b>&buf[0] ; b-=3)
	  	  {if isdigit ((int)(*(b-1)))
	    	  {strcpy (b2,b); *b=','; strcpy (b+1,b2);}}
		}
	}

if (dtype == 1)		/* engineering notation */
	{
	xd=fabs(x); zd=x; k=0;
	if (xd != 0.)
		{
		k=floor(log10(xd)); zd=xd*pow(10.,-k);
		while (((abs(k) % 3) != 0) || (zd < 1)) {zd*=10; k--;}
		zd*=SGN(x);
		}
	strcpy (fp,"%.");
	strcat (fp,b2);
	strcat (fp,"lf E%+04d");
	sprintf (buf,fp,zd,k);
	}

if (dtype == 2)		/* scientific notation */
	{
	strcpy (fp,"%.");
	strcat (fp,b2);
	strcat (fp,"lE");
	sprintf (buf,fp,x);
	b=strchr (buf,'E');	/* insert blank before 'E' */
	strcpy (b2,b); *b++=' '; *b='\0'; strcat (buf,b2);
	}

strcpy (gp,buf);
dtype=dsave;
}
/* dplaces..--------------------------------------------------------------- */

/* find the number of decimal places needed to print a real */

int dplaces (dbl x)

{
char buf[40],fp[10],*l;
int d,dk;
dbl acc=1.e-15,wd,ix,fx;

if (dtype) return decimals;
fx=modf(fabs(x),&ix);
dk=((dflag) ? 0 : decimals);
if (fx == 0.) return dk;		/* no fractional part */
for (d=dk ; d<16 ; d++)
	{
	strcpy (fp,"%.");		/* form trial format */
	strcat (fp,itoa(d,buf,10));
	strcat (fp,"lf");
	sprintf (buf,fp,x);		/* print in test format */
	l=strchr (buf,'\0')-1;		/* convert back */
	wd=strtod (buf,&l);		/* to number */
	if (dflag)			/* test difference */
	  {if (fabs(x-wd) < acc) break;}	/* adjustable */
	else
	  {if (wd != 0.) break;}		/* fixed */
	}
return d;
}
/* gcd..------------------------------------------------------------------- */

/* return greatest common divisor */

dbl gcd (dbl x, dbl y)

{
dbl a=x,b=y,c;
if (a == 0 && b == 0) return 1.;
if (a > b) swap (&a,&b,sizeof(a));
do {c=b-a*floor(b/a); b=a; a=c;} while (c != 0); return b;
}
/* help..------------------------------------------------------------------ */

/* instructions for use */

void help (void)

{
cls;
puts ("SYNTAX:");
puts ("entry => number expressed in any of the forms: 7, .2, 1.23, 3/4, 4&3/8");
puts ("op => operator (+,-,*,/) if omitted '+' is assumed");
puts ("ud => unit designator (nd,m,cm,mm,yd,ft,in) if omitted default units assumed");
puts ("  nd is used to enter non-dimensional constants");
puts ("\nCOMMANDS:");
puts ("clear => clear accumulators");
puts ("decimals=n => set number of decimal places to display (0<=n<=15)");
puts ("dflag=n => n=0 (decimals places); n=1 (self-adjusting)");
puts ("dtype=n => n=0 (fixed); n=1 (engineering); n=2 (scientific) display");
puts ("exit/quit => exit program");
puts ("fracd=n => set precision of fractional inches to 1/n in (default = 64)");
puts ("help => display this screen");
puts ("scale=n => set input scale factor");
puts ("undo => undo the last calculation performed");
puts ("unit=ud => set default units to ud");
puts ("\nDATA ENTRY:");
puts ("[op] entry [ud] => enter a value into the calculation");
puts (" ");
pausem;
cls;
}
/* display..--------------------------------------------------------------- */

/* display the accumulators */

void display (void)

{
char buf[40];
int i,r=drow;
dbl x,z,f,a,n,d,g,s,w,e,y;

for (i=0 ; i<NA ; i++)
	{
	vp (acc[i]);
	if (uexp)
		{
		sprintf (buf," %s",uda[i+1]); strcat (gp,buf);
		if (uexp != 1) {sprintf (buf,"^%d",uexp); strcat (gp,buf);}
		}
	ptext (blank,r,1,df,db); ptext (gp,r++,1,df,db);
	}

/* print ft,in in fractional form */
ptext (blank,r,1,df,db);
if (acc[4] == 0. || uexp != 1) return;
gp[0]='\0'; s=SGN(acc[4]);
x=modf(fabs(acc[4]),&f);
y=fabs(acc[5]);
z=modf(y-12.*f,&a);
d=modf(z*fracd,&n);
if (n) {g=gcd(n,fracd); n/=g; d=fracd/g;}
w=12.*f+a; if (n) w+=(dbl)n/(dbl)d;
if (y) e=100.*(w-y)/y; else e=0;

if (s < 0.) sprintf (gp,"-");
if (f) {sprintf (buf,"%d ft",(int)f); strcat (gp,buf);}
if (a)
{
if (n == 0.) sprintf (buf," %d in",(int)a); else sprintf (buf," %d &",(int)a);
strcat (gp,buf);
if (n) {sprintf (buf," %d/%d in",(int)n,(int)d); strcat (gp,buf);}
}
else
{
if (n) sprintf (buf," %d/%d in",(int)n,(int)d);
else sprintf (buf, " 0 in");
strcat (gp,buf);
}

sprintf (buf, " (error = %.4lf %%)",e); strcat (gp,buf);
ptext (gp,r++,1,df,db);
#if 0 == 1
sprintf (buf, "%d,%d,%d,%d",(int)f,(int)a,(int)n,(int)d); ptext (buf,r++,1,df,db);
#endif
}
/* process..--------------------------------------------------------------- */

/* process a data entry */

char process (void)

{
char val[40],unit[40],ops[5]="+-*/";
char *b,*v;
int i,ui;
dbl x,z,a,n,d;

b=&gp[0];
while (*b == ' ') b++;		/* skip blanks */
op=0;		/* default (addition) */

/* check for operator */
for (i=0 ; i<5 ; i++)
{
if (*b == ops[i]) {op=i; b++; break;}
}

/* copy numerical portion into val */
while (*b == ' ') b++;		/* skip blanks */
v=&val[0];
agin: if (*b == '\0' || isalpha(*b)) goto done;
if (*b == ' ') b++; else *v++=*b++;
goto agin;
done: *v='\0';

/* copy units (if any) into unit */
while (*b == ' ' && *b != '\0') b++;
v=&unit[0];
if (isalpha(*b)) {*v++=*b++; *v++=*b++; *v='\0';} else strcpy (unit,uda[udef]);

/* convert val string into a numerical quantity in x */
v=strchr(val,'/');
if (v)
{
b=strchr(val,'&');
if (b)
	{
	b--;
	a=strtod (val,&b);
	v=strchr (val,'/')-1;
	n=strtod (b+1,&v);
	b=strchr (val,'\0')-1;
	d=strtod (v+1,&b);
	}
else
	{
	a=0;
	v=strchr (val,'/')-1;
	n=strtod (val,&v);
	b=strchr (val,'\0')-1;
	d=strtod (v+1,&b);
	x=n/d;
	}
x=a+n/d;
}
else
{v=strchr (val,'\0')-1; x=strtod (val,&v);}

if (x == 0. && op == 3) return 1;	/* check for division by zero */

/* determine the index of the units input */
for (i=0,ui=-1 ; i<NU ; i++)
{
if (strcmp(unit,uda[i]) == 0) {ui=i; break;}
}
if (ui < 0) return 1;		/* no unit match */

x/=usf[ui];		/* convert x to inches */

if (ui) x*=scale;	/* scale dimensioned numbers */

/* do the arithmetic */
if (uexp != 0 && ui == 0 && op < 2) return 1;
for (i=0 ; i<NA ; i++) uacc[i]=acc[i]; uuexp=uexp;	/* save for undo */
switch (op)
{
case 0:			/* + */
	for (i=0 ; i<NA ; i++)
	{
	if (ui == 0) z=1.; else z=usf[i+1];
	acc[i]+=z*x;
	}
	if (!uexp && ui) uexp=1;
	break;
case 1:			/* - */
	for (i=0 ; i<NA ; i++)
	{
	if (ui == 0) z=1.; else z=usf[i+1];
	acc[i]-=z*x;
	}
	if (!uexp && ui) uexp=1;
	break;
case 2:			/* * */
	for (i=0 ; i<NA ; i++)
	{
	if (ui == 0) z=1.; else z=usf[i+1];
	acc[i]*=z*x;
	}
	if (ui) uexp++;
	break;
case 3:			/* / */
	for (i=0 ; i<NA ; i++)
	{
	if (ui == 0) z=1.; else z=usf[i+1];
	acc[i]/=z*x;
	}
	if (ui) uexp--;
	break;
}
return 0;
}
/* ------------------------------------------------------------------------ */

