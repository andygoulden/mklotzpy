/* DIFFTHRD.C   M. W. Klotz  11/98, 12/99

Differential thread calculations

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
char rdata (void);

/* gmark..-------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* vmark..------------ global variable declarations ----------------------- */
FILE *fp;			/* utility file pointer */
char dfile[]="DIFFTHRD.DAT";	/* input data file */
#define ND 400			/* maximum number of data items */
int np;				/* actual number of data items */

dbl pitch[ND];			/* available thread pitches (tpi) */
dbl pc,pf,pe,best,tnc,tnf,dm;

/* smark..----------- global structure declarations ----------------------- */

/* main..================================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i,j,k,ib,jb;
dbl t,d,e;

/* -------------------------- initializations ------------------------------ */
textscreen ();
/* ------------------------------------------------------------------------- */

rdata ();		/* read the data file */
puts ("DIFFERENTIAL THREAD CALCULATIONS\n");
printf ("\nNumber of data items read = %d\n\n",np);

vin ("Desired effective pitch of differential thread",&pe,"lf",100.0,"tpi");

for (i=0 ; i<np ; i++)
{
if (pe == pitch[i])
puts ("You can cut this thread directly with available screwcutting gear.");
}

for (i=0,best=1.e6 ; i<np ; i++)
{
for (j=0 ; j<np ; j++)
	{
	if (i == j) continue;
	pc=pitch[i]; pf=pitch[j];
	if (pc > pf) {t=pc; pc=pf; pf=t;}
	d=(1/pc)-(1/pf); e=ABS(1/d-pe);
	if (e < best) {best=e; ib=i; jb=j;}
	}
}
printf ("\nOf available threads, best match to %.3lf tpi is:\n",pe);
pc=pitch[ib]; pf=pitch[jb]; if (pc > pf) {t=pc; pc=pf; pf=t;}
printf ("Coarse thread = %.3lf tpi = %.3lf mm/thrd\n",pc,25.4/pc);
printf ("Fine thread = %.3lf tpi = %.3lf mm/thrd\n",pf,25.4/pf);
printf ("with an effective pitch of %.3lf tpi\n\n",1/((1/pc)-(1/pf)));

vin ("Pitch of coarse thread",&pc,"lf",pc,"tpi");
vin ("Pitch of fine thread",&pf,"lf",pf,"tpi");
vin ("Thickness of coarse (fixed) nut",&tnc,"lf",0.375,"in");
vin ("Thickness of fine (movable) nut",&tnf,"lf",0.25,"in");
vin ("Desired motion of movable nut",&dm,"lf",0.25,"in");

if (pc > pf) {t=pc; pc=pf; pf=t;}
pe=1/((1/pc)-(1/pf));
printf ("\nEffective pitch = %.3lf tpi\n",pe);
printf ("Motion for one revolution = %.5lf in\n",1/pe);
printf ("Total turns to obtain desired motion = %.3lf\n",dm*pe);
printf ("Minimum length of coarse thread needed = %.3lf in\n",tnc+dm*pe/pc);
printf ("Minimum length of fine thread needed = %.3lf in\n",tnf+dm*pe/pf);
printf ("Maximum distance between nuts = %.3lf in\n",dm*pe/pc);
printf ("Minimum distance between nuts = %.3lf in\n",dm*pe/pf);


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
char temp[81],f,k=0;
int i;
dbl t;

if ((fp=fopen(dfile,"rt")) == NULL)
	{printf ("FAILED TO OPEN DATA FILE: %s\n",dfile); beep; return 1;}

np=0;			/* number of data entries */
f=0;
while ((fgets(temp,sizeof(temp),fp)) != NULL)
	{
	trim(temp);		/* uses library trim */
	if ((strstr(temp,"STARTOFDATA")) != NULL) {f=1; continue;}
	if (!f) continue;
	if ((strstr(temp,"ENDOFDATA")) != NULL) break;
	if (temp[0] == 't' && temp[1] == 'p') {k=0; continue;}
	if (temp[0] == 'm' && temp[1] == 'm') {k=1; continue;}
	if (temp[0] != '\0' && temp[0] != ';')
		{
		/* decode and store data here */
		if (!k)
		{pitch[np]=atof(strtok(temp,"\t;"));}
		else
		{pitch[np]=25.4/atof(strtok(temp,"\t;"));}
		if (++np == ND)
			{
			printf
			("More than maximum (%d) data items in data file\n",ND);
			quit;
			}
		}
	}
fclose (fp);
printf ("%.4lf\n",pitch[np-1]);

do
{
for (i=0,f=0 ; i<np-1 ; i++)
{if (pitch[i] > pitch[i+1]) {t=pitch[i]; pitch[i]=pitch[i+1]; pitch[i+1]=t;}}
} while (f);

return 0;
}
/* ------------------------------------------------------------------------ */
