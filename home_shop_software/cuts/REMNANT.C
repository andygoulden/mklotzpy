/* REMNANT.C   M. W. Klotz  3,11/98, 12/01, 2,3/02

A modified form of CUTS written for Chris Peters (NZ) where a selection of
variable sized 'remnants' replaces the constant bar length in the data file.

This improved version makes use of Mike Graham's superior cutlist logic and
does a far better job than my previous attempt.

*/

/* head..-------------------- header files -------------------------------- */

/* Unneeded headers can be left in place - the header files consist mainly of
'defines' for the compiler and do not add to the size of the executable code.
*/

/* turns off assert macros when not debugging */
#define NDEBUG

/* #include <assert.h> */
#include <conio.h>
#include <ctype.h>
/* #include <dos.h> */
#include <float.h>
#include <graph.h>
/* #include <malloc.h>  */
#include <math.h>
/* #include <search.h> */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
/* #include <string.h> */
/* #include <time.h> */

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

/* constant declarations - all 'defines' - no executable code */
/* #include <constant.h> */

/* -------------------------- function prototypes ------------------------- */

/* Minimize the number of these included since they contribute to exectuable
size. */

/* global utility functions */
/* note: no space between function name and parameter list */
/* #include <util.h> */

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
int diff (real a, real b);
void cutlist(void);
char rdata (void);

/* --------------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* struc..----------- global structure declarations ----------------------- */

#define MAXLENGTHS 100		// Maximum number of lengths to be used
#define MAXPIECESPERLENGTH 100  // Maximum number of pieces per length of stock

typedef struct length {
int cuts;	// number of pieces cut from this length
real bar;	// original bar length
real drop;	// unallocated length
real size[MAXPIECESPERLENGTH];    	// sizes of pieces
};
struct length stock[MAXLENGTHS];  // stock represents the pieces of stock

/* vmark..------------ global variable declarations ----------------------- */
FILE *fp;			/* utility file pointer */
char dfile[]="REMNANT.DAT";	/* data file */
#define N 100			/* maximum number of object sizes */
int np;				/* actual number of object sizes */
int num[N];			/* number needed */
int npiece=0;			/* number of pieces needed */
real size[N];			/* object sizes */
real precision=1.0e-3;		/* used in comparing reals */
real bars[MAXLENGTHS];		/* storage for remnant sizes */
int nbars=0;			/* actual number of remnants */
real maxbar=0;			/* length of longest remnant */
real tbars=0;			/* total length of remnants */
real reqd=0;			/* total required pieces length */
real kreqd=0;			/* total required length including kerf */
real waste=0;			/* total waste */
real kerf=0;			/* saw kerf width */
real kwaste=0;			/* total kerf waste */
real dwaste=0;			/* total drop waste */

#define MAXPIECES 1000          // Maximum number of pieces to be cut
real pieces[MAXPIECES]; 	// sizes to cut
int numpieces=0;  // used to index the pieces array - number of pieces to cut

/* main..====xxxxxxxx====================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i,j,tcuts=0,k;
real kw;

/* -------------------------- initializations ------------------------------ */
textscreen();
for (i=0 ; i<MAXLENGTHS ; i++) stock[i].bar=0;
/* ------------------------------------------------------------------------- */

/* initialization */
puts ("REMNANT CUTTING LIST\n");
rdata ();		/* read the data file */
printf ("saw kerf = %g\n",kerf);
printf ("remnants = ");
for (i=0 ; i<nbars ; i++) printf ("%g, ",bars[i]); print;
printf ("total remnant pieces, length = %d, %g\n",nbars,tbars);
printf ("total required pieces length = %g\n\n",reqd);
if (size[0] > maxbar) 
{printf ("largest piece %g larger than largest remnant %g\n",size[0],maxbar); quit;}
if (reqd > tbars)
{printf ("total piece length %g greater than total remnant length %g\n",reqd,tbars); quit;}
if (kreqd > tbars)
{printf ("total piece length including kerf allowance %g greater than total remnant length %g\n",kreqd,tbars); quit;}
if (size[np-1] > bars[nbars-1])
printf ("smallest piece %g longer than shortest remnant %g\n\n",size[np-1],bars[nbars-1]);
for (i=0 ; i<np ; i++) printf ("\t%d",num[i]); print;
for (i=0 ; i<np ; i++) printf ("\t%-5.2f",size[i]); print; print;

// construct the pieces array

for (i=0 ; i<np ; i++)
{for (j=0 ; j<num[i] ; j++) pieces[numpieces++]=size[i]+kerf;}

//store the stock data
for (i=0 ; i<nbars ; i++)
{
stock[i].bar=bars[i]; stock[i].drop=bars[i];
}

cutlist ();		// do the assignment

i=0;
do
{
tcuts+=stock[i].cuts;
kw=stock[i].cuts*kerf;
kwaste+=kw;
dwaste+=stock[i].drop;
printf("Length %g - ",stock[i].bar);
for (j=0 ; j<stock[i].cuts ; j++) printf("%g, ", stock[i].size[j]-kerf);
printf ("(%g kerf + %g drop = %g waste)\n",kw,stock[i].drop,stock[i].drop+kw);
} while (stock[++i].bar != 0);

if (tcuts < npiece) printf ("\n*** WARNING: %d PIECE(S) NOT ASSIGNED ***\n",npiece-tcuts);

printf ("\ntheoretical possible waste = %g - %g = %g\n",tbars,reqd,tbars-reqd);
printf ("kerf waste = %g\n",kwaste);
printf ("drop waste = %g\n",dwaste);
printf ("kerf + drop waste = %g\n",kwaste+dwaste);

/* post run processing */
mexit:
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* diff..------------------------------------------------------------------ */

/* test two reals to some precision */

int diff (real a, real b)

{
real t;

t=a-b;
if (ABS(t) <= precision) return 0; else return SGN(t);
}
/* rdata..----------------------------------------------------------------- */

/* read the data file */

char rdata (void)

{
char f=1,ff=1,line[81],dum[40];
int i,j,k,n;
real t;


if ((fp=fopen(dfile,"rt")) == NULL)
	{printf ("FAILED TO OPEN DATA FILE: %s",dfile); beep; quit;}

np=0;			/* number of data entries */
while ((fgets(line,sizeof(line),fp)) != NULL)
	{
	trim(line);
	if ((strstr(line,"ENDOFDATA"))) break;
	if (line[0] != '\0' && line[0] != ';')
		{
		if (ff)
		{
		strcpy (dum,strtok(line,",;\n"));
		kerf=(real)atof(dum);
		ff=0;
		}
		else
		{
		if (f)
		{
		strcpy (dum,strtok(line,","));
		k=(int)atof(dum);
		if (k == 0) {f=0; continue;}
		strcpy (dum,strtok(NULL,";\n"));
		t=(real)atof(dum);
		if (t > maxbar) maxbar=t;
		for (i=0 ; i<k ; i++) 
			{
			bars[nbars]=t;
			tbars+=t;
			if (++nbars == N)
			{
			printf
			("More than maximum (%d) data items in data file\n",N);
			quit;
			}
			}
		}
		else
		{
		strcpy (dum,strtok(line,","));
		num[np]=(int)atof(dum);
		strcpy (dum,strtok(NULL,";\n"));
		size[np]=(real)atof(dum);
		npiece+=num[np];
		reqd+=num[np]*size[np];
		kreqd+=num[np]*(size[np]+kerf);
		if (++np == N)
			{
			printf
			("More than maximum (%d) data items in data file\n",N);
			quit;
			}
		}
		}
		}
	}
fclose (fp);

do		/* sort data */
{
	for (i=0,f=0 ; i<np-1 ; i++)
	{
	if (size[i] < size[i+1])
		{
		n=num[i]; num[i]=num[i+1]; num[i+1]=n;
		t=size[i]; size[i]=size[i+1]; size[i+1]=t; f=1;
		}
	}
} while (f);

do		/* sort data */
{
	for (i=0,f=0 ; i<nbars-1 ; i++)
	{
	if (bars[i] < bars[i+1])
		{
		t=bars[i]; bars[i]=bars[i+1]; bars[i+1]=t; f=1;
		}
	}
} while (f);

return 0;
}
/* cutlist..--------------------------------------------------------------- */
/*
Make up the cutting list

  The basic algorithm is this: for each piece to be cut, have it cut from
  the shortest piece that it fits in.  There's not much to it, eh?
*/
void cutlist(void)
{

int i=0,j=0;	// temporary indexing variables
int best=0;	// index of the current best stock length

	for (i=0 ; i<numpieces ; i++)
	{
		j=0;
		while (diff(stock[j].drop,pieces[i]) < 0) j++;
		best=j;
		if (best > MAXPIECESPERLENGTH)
		{
		printf ("*** PIECE %g CAN'T BE ASSIGNED ***\n",pieces[i]-kerf);
		continue;
		}
// search to find shortest drop that will accommodate this piece
	while (stock[++j].bar != 0) 
// stock[j].drop <= stock[best].drop and stock[j].drop > pieces[i]
		if (diff(stock[best].drop,stock[j].drop) > 0 &&
		diff(stock[j].drop,pieces[i]) >= 0) best=j;
// assign this piece
		stock[best].drop-=pieces[i];
		stock[best].size[stock[best].cuts]=pieces[i];
		stock[best].cuts++;
	}
}
/* ------------------------------------------------------------------------ */
