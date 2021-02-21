/* SIMUL.C   M. W. Klotz   01/02

Gauss-Jordan elimination solution to simultaneous equations

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

/* gmark..-------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* vmark..------------ global variable declarations ----------------------- */

FILE *fpt;			/* utility file pointer */
char dfile[]="SIMUL.DAT";	/* input data file */

#define MN 50		/* maximum number of variables */

dbl mata [MN] [MN];	/* input matrix => inverse matrix at output */
dbl matb [MN];		/* constant vector => solution vector at output */
int ipvot [MN];
int index [MN] [2];
int nmat;		/* order of mata */
dbl detmat;		/* determinant of matrix at output */


/* smark..----------- global structure declarations ----------------------- */

/* main..================================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i,j,k;

/* -------------------------- initializations ------------------------------ */
/* ------------------------------------------------------------------------- */

if (rdata ()) goto mexit;		/* read the data file */

print;
for (i=0 ; i<nmat ; i++)
{
for (j=0 ; j<nmat ; j++) {printf (" %+.3lf *x%d",mata[i][j],j+1);}
printf (" = %.3lf\n",matb[i]);
}

gjor (); if (detmat == 0) {puts ("ERROR"); beep; quit;}

print; for (i=0 ; i<nmat ; i++) {printf ("x%d = %lf\n",i+1,matb[i]);}

/* post run processing */
mexit:
fcloseall();
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
int i,irow;
dbl x;

if ((fpt=fopen(dfile,"rt")) == NULL)
	{printf ("FAILED TO OPEN DATA FILE: %s\n",dfile); beep; return 1;}

nmat=0; irow=0; f=0;
while ((fgets(temp,sizeof(temp),fpt)) != NULL)
	{
	trim(temp);		/* uses library trim */
	if ((strstr(temp,"STARTOFDATA")) != NULL) {f=1; continue;}
	if (!f) continue;
	if ((strstr(temp,"ENDOFDATA")) != NULL) break;
	if (temp[0] != '\0' && temp[0] != ';')
		{

		if (nmat == 0)
		{
		nmat=(int)atof(strtok(temp,",\t;"));
		if (nmat>MN) {beep; puts ("TOO MANY EQUATIONS"); return 1;}
		}
		else
		{
		mata[irow][0]=(dbl)atof(strtok(temp,",\t;"));
		for (i=1 ; i<nmat ; i++)		
			{mata[irow][i]=(dbl)atof(strtok(NULL,",\t;"));}
		matb[irow]=(dbl)atof(strtok(NULL,",\t;"));
		irow++;
		}
		}
	}

fclose (fpt);
return 0;
}
/* ------------------------------------------------------------------------ */
