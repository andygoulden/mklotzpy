/* COLSORT.C   M. W. Klotz  3/03

Multi-column sorting

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
void sort (void);

/* gmark..-------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* vmark..------------ global variable declarations ----------------------- */
FILE *fp;			/* utility file pointer */
char dfile[]="COLSORT.DAT";	/* input data file */
char ofile[]="COLSORT.OUT";	// output data file
#define ND 200			/* maximum number of data lines */
int nd;				/* actual number of data lines */

#define MAXCOL 8		// maximum number of columns
#define MAXALPHA 30		// max characters in column entry

int ncol;			// number of columns
int coltype[MAXCOL];		// column type (1=alphabetic, 0=numeric)
int sortcol;			// column to sort on
int sorttype;			// type of sort (1=increasing, 0=decreasing)

/* smark..----------- global structure declarations ----------------------- */

struct data {
char e[MAXALPHA];
} d [MAXCOL] [ND];

/* main..================================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i,j,k;

/* -------------------------- initializations ------------------------------ */
textscreen();
/* ------------------------------------------------------------------------- */

puts ("MULTI-COLUMN SORTING\n");

rdata ();		/* read the data file */

printf ("Number of columns in a data line = %d\n",ncol);
printf ("Number of data lines read = %d\n",nd);

if ((fp=fopen(ofile,"wt")) == NULL)
	{printf ("FAILED TO OPEN OUTPUT FILE: %s\n",dfile); beep; goto whoops;}

fprintf (fp,"Data before sorting:\n\n");
for (i=0 ; i<nd ; i++)
{for (j=0 ; j<ncol ; j++) fprintf (fp,"%s,",d[j][i].e); fprintf (fp,"\n");}

sort ();

fprintf (fp,"\nData after ");
if (sorttype == 1) fprintf (fp,"increasing "); else fprintf (fp,"decreasing ");
if (coltype[sortcol-1] == 1)
fprintf (fp,"alphabetic "); else fprintf (fp,"numerical ");
fprintf (fp,"sort on column %d:\n\n",sortcol);

for (i=0 ; i<nd ; i++)
{for (j=0 ; j<ncol ; j++) fprintf (fp,"%s,",d[j][i].e); fprintf (fp,"\n");}

/* post run processing */
mexit:
printf ("\nYour sorted data is on %s\n",ofile);
whoops:
fcloseall();
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* rdata..----------------------------------------------------------------- */

/* read the data file */

char rdata (void)

{
char temp[81],f,g;
int i,c;

if ((fp=fopen(dfile,"rt")) == NULL)
	{printf ("FAILED TO OPEN DATA FILE: %s\n",dfile); beep; return 1;}

nd=0;			/* number of data entries */
f=0; g=0; c=0;
while ((fgets(temp,sizeof(temp),fp)) != NULL)
	{
	trim(temp);		/* uses library trim */
	if ((strstr(temp,"STARTOFDATA")) != NULL) {f=1; continue;}
	if (!f) continue;
	if ((strstr(temp,"ENDOFDATA")) != NULL) break;
	if (temp[0] == '\0' || temp[0] == ';') continue;
		if (g == 0)
		{
		ncol=(int)atof(strtok(temp,",\t;")); g=1; c=0;
		continue;
		}
		if (g == 1)
		{
		if (temp[0] == 'a') coltype[c]=1; else coltype[c]=0;
		if (++c == ncol) g=2;
		continue;
		}
		if (g == 2)
		{
		sortcol=(int)atof(strtok(temp,",\t;")); g=3;
		continue;
		}
		if (g == 3)
		{
		if (temp[0] == 'i') sorttype=1; else sorttype=0;
		g=4;
		continue;
		}
		{
		/* decode and store data here */
		strcpy (d[0][nd].e,strtok(temp,",\t;"));
		if (ncol == 1) continue;
		for (i=1 ; i<ncol ; i++)
		strcpy (d[i][nd].e,strtok(NULL,",\t;"));
		if (++nd == ND)
			{
			printf
			("More than maximum (%d) data items in data file\n",ND);
			quit;
			}
		}
	}

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
/* sort..------------------------------------------------------------------ */

/* do the sort on the appropriate column */

void sort (void)

{
char f;
int i,j,s=sortcol-1;

if (coltype[s] == 1)		// alphabetic sort
{
	if (sorttype == 1)		//increasing sort
	{
	printf ("increasing alphabetic sort on column %d\n",sortcol);
	do
		{
			for (i=0,f=0 ; i<nd-1 ; i++)
			{
			if (acomp(d[s][i].e,d[s][i+1].e) > 0)
				{
				f=1;
				for (j=0 ; j<ncol ; j++)
				swap (d[j][i].e,d[j][i+1].e,MAXALPHA);
				}
			}
		} while (f);
	}
	else				//decreasing sort
	{
	printf ("decreasing alphabetic sort on column %d\n",sortcol);
	do
		{
			for (i=0,f=0 ; i<nd-1 ; i++)
			{
			if (acomp(d[s][i].e,d[s][i+1].e) < 0)
				{
				f=1;
				for (j=0 ; j<ncol ; j++)
				swap (d[j][i].e,d[j][i+1].e,MAXALPHA);
				}
			}
		} while (f);
	}
}

else				//numeric sort
{
	if (sorttype == 1)		//increasing sort
	{
	printf ("increasing numeric sort on column %d\n",sortcol);
	do
		{
			for (i=0,f=0 ; i<nd-1 ; i++)
			{
			if (atof(d[s][i].e) > atof(d[s][i+1].e))
				{
				f=1;
				for (j=0 ; j<ncol ; j++)
				swap (d[j][i].e,d[j][i+1].e,MAXALPHA);
				}
			}
		} while (f);
	}
	else				//decreasing sort
	{
	printf ("decreasing numeric sort on column %d\n",sortcol);
	do
		{
			for (i=0,f=0 ; i<nd-1 ; i++)
			{
			if (atof(d[s][i].e) < atof(d[s][i+1].e))
				{
				f=1;
				for (j=0 ; j<ncol ; j++)
				swap (d[j][i].e,d[j][i+1].e,MAXALPHA);
				}
			}
		} while (f);
	}
}

}
/* ------------------------------------------------------------------------ */
