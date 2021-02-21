/* CURFIT.C   M. W. Klotz   01/02, 12/04

Various kinds of curve fits to data

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
void gjor (void);
void swap (void *a,void *b,int type);
char rdata (void);
void menu (void);
void poly (void);
void logx (void);
void expx (void);
void power (void);
void summary (dbl x1, dbl x2, dbl y1, dbl y2, dbl sst, dbl sse);

/* gmark..-------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* vmark..------------ global variable declarations ----------------------- */

FILE *fpt;			/* utility file pointer */
char dfile[]="CURFIT.DAT";	/* input data file */
FILE *fp;
char ofile[]="CURFIT.OUT";	/* output file */

#define MN 50		/* maximum number of variables */

dbl mata [MN] [MN];	/* input matrix => inverse matrix at output */
dbl matb [MN];		/* constant vector => solution vector at output */
int ipvot [MN];
int index [MN] [2];
int nmat;		/* order of mata */
dbl detmat;		/* determinant of matrix at output */

#define ND 200		/* maximum number of data points */
dbl x[ND],y[ND];	/* storage for data points */
int nd;			/* actual number of data points */


/* smark..----------- global structure declarations ----------------------- */
textscreen();
/* main..================================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
char op;
int i,j,k;

/* -------------------------- initializations ------------------------------ */
textscreen();
/* ------------------------------------------------------------------------- */

if (rdata ()) goto mexit;		/* read the data file */
printf ("%d data pairs read from file\n\n",nd);

if (nd < 2)
	{puts ("TOO FEW DATA POINTS TO CONSTRUCT FIT"); beep; quit;}

if ((fp=fopen(ofile,"wt")) == NULL)
	{printf ("FAILED TO OPEN OUTPUT FILE: %s\n",ofile); quit;}

menu();
agin: op=getkey("(1,2,3,4,Q) ? ");
switch (op)
{
	case '1':		//polynomial
		poly ();
		break;
	case '2':		//logarithmic
		logx();
		break;
	case '3':		//exponential
		expx();
		break;
	case '4':		//power
		power();
		break;

	case 'q':
		goto mexit;
	default:
		beep; puts ("NOT A VALID OPTION");
		break;
}

/* post run processing */
mexit:
fcloseall();
printf ("\nYour output is on: %s\n",ofile);
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* gjor..------------------------------------------------------------------ */

/* 

GAUSS-JORDAN ELIMINATION MATRIX INVERSION AND EQUATION SOLUTION

MATA = INPUT MATRIX = INVERSE MATRIX AT OUTPUT
MATB = CONSTANT VECTOR = SOLUTION VECTOR AT OUTPUT
NMAT = ORDER OF MATRIX A
DETMAT = DETERMINANT OF MATRIX (OUTPUT)

*/

void gjor (void)

{
int i,j,k,l,li,iii,irow,icol;
dbl ts,pivot;

//SEARCH FOR PIVOTAL ELEMENT

detmat=1.;
for (i=0 ; i<nmat ; i++) {ipvot[i]=0; index[i][0]=0; index[i][1]=0;}

for (iii=0 ; iii<nmat ; iii++)		//beginning of iii loop
{
ts=0;
for (j=0 ; j<nmat ; j++)
{
if (ipvot[j] == 1) continue;
	for (k=0 ; k<nmat ; k++)
	{
	if (ipvot[k] == 1) continue;
	if (ipvot[k] > 1) {detmat=0; return;}
	if (ABS(mata[j][k]) > ABS(ts)) {irow=j; icol=k; ts=mata[j][k];}
	}
}

//PUT PIVOTAL ELEMENTS ON DIAGONAL

if (irow != icol)
{
detmat=-detmat;
for (l=0 ; l<nmat ; l++)
  swap (&mata[irow][l],&mata[icol][l],sizeof(mata[0][0]));
swap (&matb[irow],&matb[icol],sizeof(matb[0]));
}
index[iii][0]=irow; index[iii][1]=icol;
pivot=mata[icol][icol]; detmat*=pivot;


//DIVIDE PIVOT ROW BY PIVOT ELEMENT

mata[icol][icol]=1.; ipvot[icol]=1;
for (l=0 ; l<nmat ; l++) {mata[icol][l]/=pivot;}
matb[icol]/=pivot;

//REDUCE NON-PIVOT ROWS

for (li=0 ; li<nmat ; li++)
{
if (li != icol)
	{
	ts=mata[li][icol]; mata[li][icol]=0;
	for (l=0 ; l<nmat ; l++) mata[li][l]-=mata[icol][l]*ts;
	matb[li]-=matb[icol]*ts;
	}
}
}					//end of iii loop

//DECLARE MATRIX UNINVERTABLE IF ANY ROW UNPIVOTED

for (i=0 ; i<nmat ; i++) {if (ipvot[i] != 1) {detmat=0; return;}}

//INTERCHANGE COLUMNS

for (l=nmat-1 ; l>=0 ; l--)
{
if (index[l][0] != index[l][1])
	{
	irow=index[l][0]; icol=index[l][1];
	for (k=0 ; k<nmat ; k++)
		{
		swap (&mata[k][irow],&mata[k][icol],sizeof(mata[0][0]));
		}
	}
}
return;
}
/* swap..------------------------------------------------------------------ */

/* type independent swap */

void swap (void *a,void *b,int type)

{
char temp[80];
memcpy (temp,a,type);  memcpy (a,b,type);  memcpy (b,temp,type);
}
/* rdata..----------------------------------------------------------------- */

/* read the data file */

char rdata (void)

{
char temp[81],f;
int i;

if ((fpt=fopen(dfile,"rt")) == NULL)
	{printf ("FAILED TO OPEN DATA FILE: %s\n",dfile); beep; return 1;}

f=0; nd=0;
while ((fgets(temp,sizeof(temp),fpt)) != NULL)
	{
	trim(temp);		/* uses library trim */
	if ((strstr(temp,"STARTOFDATA")) != NULL) {f=1; continue;}
	if (!f) continue;
	if ((strstr(temp,"ENDOFDATA")) != NULL) break;
	if (temp[0] != '\0' && temp[0] != ';')
		{
		x[nd]=(dbl)atof(strtok(temp,",\t;"));
		y[nd]=(dbl)atof(strtok(NULL,",\t;"));
		if (++nd == ND)
			{
			printf
			("More than maximum (%d) data pairs in data file\n",ND);
			quit;
			}
		}
	}

do		/* sort data */
{
for (i=0,f=0 ; i<nd-1 ; i++)
{
if (x[i] > x[i+1])
	{
	f=1;
	swap (&x[i],&x[i+1],sizeof(x[i]));
	swap (&y[i],&y[i+1],sizeof(y[i]));
	}
}
} while (f);

fclose (fpt);
return 0;
}
/* menu..------------------------------------------------------------------ */

void menu (void)

{
puts ("1 - polynomial fit");
puts ("2 - logarithmic fit");
puts ("3 - exponential fit");
puts ("4 - power fit");
puts ("Q - quit");
}
/* poly..------------------------------------------------------------------ */

void poly (void)

{
int i,j,k,degree,maxdeg,maxd;
dbl x1,x2,y1,y2,z,err,perr,xx,yy,sst,sse;
dbl pmax;

maxdeg=MIN(nd-1,MN-1);
printf ("\nmaximum degree of polynomial fit possible = %d\n",maxdeg);

agin: vin ("Degree of polynomial fit",&degree,"d",3.,"");

if (degree < 1 || degree > maxdeg)
{
beep; puts ("INVALID DEGREE, TRY AGAIN"); goto agin;
}

nmat=degree+1; x1=0; x2=0; y1=0; y2=0; z=0;
for (i=0 ; i<nmat ; i++)
{
matb[i]=0; for (j=0 ; j<nmat ; j++) mata [i][j]=0;
}

for (k=0 ; k<nd ; k++)
{
x1+=x[k]; x2+=x[k]*x[k];
y1+=y[k]; y2+=y[k]*y[k];
z+=x[k]*y[k];
	for (i=0 ; i<nmat ; i++)
	{
	if (i>0) matb[i]+=y[k]*ATOB(x[k],i); else matb[i]+=y[k];
		for (j=i ; j<nmat ; j++)
		{
		if (i>0 || j>0)
		  {mata[i][j]+=ATOB(x[k],i+j);} else mata[i][j]+=1;
		mata[j][i]=mata[i][j];
		}
	}
}
x1/=nd; y1/=nd;
gjor ();

fprintf (fp,"polynomial fit Y = A0 + A1*X + A2*X^2 + A3*X^3 + ...\n");
fprintf (fp,"order of polynomial fit requested = %d\n\n",degree);

for (i=0 ; i<nmat ; i++)		//print coefficients
{
fprintf (fp,"A%d = %lf\n",i,matb[i]);
}

//print fit summary
pmax=0;
fprintf (fp,"\n%6s %10s %10s %10s %10s %10s\n\n",\
  "index","xdata","ydata","ycalc","error","err%");
for (i=0,sse=0 ; i<nd ; i++)
{
xx=x[i]; yy=matb[0];
for (j=1 ; j<nmat ; j++) yy+=matb[j]*ATOB(xx,j);
err=yy-y[i]; if (y[i] != 0) perr=100.*err/y[i]; else perr=0.;
fprintf (fp,"%6d %10.2lE %10.2lE %10.2lE %10.2lE %10.2lE",i,x[i],y[i],yy,err,perr);
if (ABS(perr) > pmax)
{
	pmax=ABS(perr);
	fprintf (fp," **\n");
}
else fprintf (fp,"\n");
sse+=err*err;
}

sst=y2-nd*y1*y1;

summary (x1,x2,y1,y2,sst,sse);
}
/* logx..------------------------------------------------------------------ */

void logx (void)

{
int i,j,k;
dbl v,x1,x2,y1,y2,z,err,perr,xx,yy,sst,sse,A,B;
dbl pmax;

nmat=2; x1=0; x2=0; y1=0; y2=0; z=0;

for (k=0 ; k<nd ; k++)
{
v=log(x[k]);
x1+=v; x2+=v*v;
y1+=y[k]; y2+=y[k]*y[k];
z+=v*y[k];
}
x1/=nd; y1/=nd; B=(z-nd*x1*y1)/(x2-nd*x1*x1); A=y1-B*x1;

fprintf (fp,"logarithmic fit Y = A + B * ln(X)\n");
fprintf (fp,"\nA = %lE\n",A);
fprintf (fp,"B = %lE\n",B);

//print fit summary
pmax=0;
fprintf (fp,"\n%6s %10s %10s %10s %10s %10s\n\n",\
  "index","xdata","ydata","ycalc","error","err%");
for (i=0,sse=0 ; i<nd ; i++)
{
xx=x[i]; yy=A+B*log(xx);
err=yy-y[i]; if (y[i] != 0) perr=100.*err/y[i]; else perr=0.;
fprintf (fp,"%6d %10.2lE %10.2lE %10.2lE %10.2lE %10.2lE",i,x[i],y[i],yy,err,perr);
if (ABS(perr) > pmax)
{
	pmax=ABS(perr);
	fprintf (fp," **\n");
}
else fprintf (fp,"\n");
sse+=err*err;
}

sst=y2-nd*y1*y1;

summary (x1,x2,y1,y2,sst,sse);
}
/* expx..------------------------------------------------------------------ */

void expx (void)

{
int i,j,k;
dbl v,x1,x2,y1,y2,z,err,perr,xx,yy,sst,sse,A,B;
dbl pmax;

nmat=2; x1=0; x2=0; y1=0; y2=0; z=0;

for (k=0 ; k<nd ; k++)
{
v=log(y[k]);
x1+=x[k]; x2+=x[k]*x[k];
y1+=v; y2+=v*v;
z+=v*x[k];
}
x1/=nd; y1/=nd; B=(z-nd*x1*y1)/(x2-nd*x1*x1); A=exp(y1-B*x1);

fprintf (fp,"exponential fit Y = A * exp (B * X)\n");
fprintf (fp,"\nA = %lE\n",A);
fprintf (fp,"B = %lE\n",B);

//print fit summary
pmax=0;
fprintf (fp,"\n%6s %10s %10s %10s %10s %10s\n\n",\
  "index","xdata","ydata","ycalc","error","err%");
for (i=0,sse=0 ; i<nd ; i++)
{
xx=x[i]; yy=A*exp(B*xx);
err=yy-y[i]; if (y[i] != 0) perr=100.*err/y[i]; else perr=0.;
fprintf (fp,"%6d %10.2lE %10.2lE %10.2lE %10.2lE %10.2lE",i,x[i],y[i],yy,err,perr);
if (ABS(perr) > pmax)
{
	pmax=ABS(perr);
	fprintf (fp," **\n");
}
else fprintf (fp,"\n");
sse+=err*err;
}

sst=y2-nd*y1*y1;

summary (x1,x2,y1,y2,sst,sse);
}
/* power..----------------------------------------------------------------- */

void power (void)

{
int i,j,k;
dbl u,v,x1,x2,y1,y2,z,err,perr,xx,yy,sst,sse,A,B;
dbl pmax;

nmat=2; x1=0; x2=0; y1=0; y2=0; z=0;

for (k=0 ; k<nd ; k++)
{
u=log(x[k]);
v=log(y[k]);
x1+=u; x2+=u*u;
y1+=v; y2+=v*v;
z+=v*u;
}
x1/=nd; y1/=nd; B=(z-nd*x1*y1)/(x2-nd*x1*x1); A=exp(y1-B*x1);

fprintf (fp,"power fit Y = A * X^B\n");
fprintf (fp,"\nA = %lE\n",A);
fprintf (fp,"B = %lE\n",B);

//print fit summary
pmax=0;
fprintf (fp,"\n%6s %10s %10s %10s %10s %10s\n\n",\
  "index","xdata","ydata","ycalc","error","err%");
for (i=0,sse=0 ; i<nd ; i++)
{
xx=x[i]; yy=A*ATOB(xx,B);
err=yy-y[i]; if (y[i] != 0) perr=100.*err/y[i]; else perr=0.;
fprintf (fp,"%6d %10.2lE %10.2lE %10.2lE %10.2lE %10.2lE",i,x[i],y[i],yy,err,perr);
if (ABS(perr) > pmax)
{
	pmax=ABS(perr);
	fprintf (fp," **\n");
}
else fprintf (fp,"\n");
sse+=err*err;
}

sst=y2-nd*y1*y1;

summary (x1,x2,y1,y2,sst,sse);
}
/* summary..--------------------------------------------------------------- */

void summary (dbl x1, dbl x2, dbl y1, dbl y2, dbl sst, dbl sse)

{
dbl ssr;		//residual sum of squares
dbl f;			//degrees of freedom adjusted correlation coefficient

ssr=sst-sse;
f=1-sse*(nd-1)/(sst*(nd-nmat-1));

fprintf (fp,"\ncorrelation coefficient = %lE\n",f);
fprintf (fp,"closer to 1, better the fit");

}
/* ------------------------------------------------------------------------ */
