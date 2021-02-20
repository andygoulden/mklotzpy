/* CUTS.C   M. W. Klotz  3,11/98, 2/02

Optimize cutting pieces from bars of standard length.
Uses a modified greedy algorithm.

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
real greedy (int zeroflag);
char rdata (void);

/* --------------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* struc..----------- global structure declarations ----------------------- */

/* vmark..------------ global variable declarations ----------------------- */
FILE *fp;			/* utility file pointer */
char dfile[]="CUTS.DAT";	/* data file */
#define N 100			/* maximum number of object sizes */
int np;				/* actual number of object sizes */
/* NOTE: order arrays by decreasing size */
int num[N];			/* number needed */
real size[N];			/* object sizes */
real bar=0.;			/* standard bar size */
int best[N];			/* stores greedy solution */
int last[N];			/* used to compress identical solutions */
real precision=1.0e-3;		/* used in comparing reals */
char debug;

/* main..====xxxxxxxx====================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i,j,k,nbar,oreqd,dup;
real reqd,waste,twaste,owaste,lwaste;
char more,op;

/* -------------------------- initializations ------------------------------ */
textscreen();
/* ------------------------------------------------------------------------- */

/* starting program with an argument turns debug on */
if (argc > 1) debug=1; else debug=0;

/* initialization */
rdata ();		/* read the data file */
printf ("Number of data items read = %d\n",np);
for (i=0 ; i<np ; i++) last[i]=0;
twaste=0; nbar=0; dup=0;
for (i=0,reqd=0 ; i<np ; i++) reqd+=size[i]*num[i];
oreqd=ceil(reqd/bar);
owaste=oreqd*bar-reqd;
printf ("standard length = %g\n",bar);
if (size[0] > bar) 
{printf ("largest piece %g larger than bar %g\n",size[0],bar); quit;}
printf ("theoretical minimum waste possible = %g\n",owaste);
printf ("theoretical minimum standard lengths possible = %d\n\n",oreqd);
for (i=0 ; i<np ; i++) printf ("\t%d",num[i]); print;
for (i=0 ; i<np ; i++) printf ("\t%-5.2f",size[i]); print; print;
op=getkey("Search for zero waste solutions first (Y/[N]) ? ");
puts ("patience ...");

if (debug)
{
if (op == 'y')
{
waste=greedy (0);		/* test for zero waste solutions */
if (waste > 1.e8) waste=greedy (1); /* use greedy if no zero waste solutions */
}
else waste=greedy (1);
printf ("best returned = ");
for (i=0 ; i<np ; i++) printf ("%d,",best[i]); printf ("  %g\n",waste);
goto mexit;
}

agin:
if (op == 'y')
{
waste=greedy (0);		/* test for zero waste solutions */
if (waste > 1.e8) waste=greedy (1); /* use greedy if no zero waste solutions */
}
else waste=greedy (1);

nbar++;				/* count this bar */
twaste+=waste;			/* accumulate total waste */
for (i=0,more=0 ; i<np ; i++) {if ((num[i]-=best[i]) > 0) more=1;}
if (waste == bar) more=0;	/* prevents runaway */

if (nbar > 1)
{
for (i=0,k=0 ; i<np ; i++)		/* same as last solution? */
  if (best[i] != last[i]) {k=1; break;}
}
else k=0;

if (k)				/* if not, print last solution */
{
printf ("%d x",dup);
for (j=0 ; j<np ; j++) printf ("\t%d",last[j]);
printf ("\t%-5.2f\n",dup*lwaste);
dup=1;
}
else dup++;

for (i=0 ; i<np ; i++) last[i]=best[i]; lwaste=waste;

if (more) goto agin;		/* repeat if not done yet */

printf ("%d x",dup);		/* print pending solution */
for (j=0 ; j<np ; j++) printf ("\t%d",last[j]);
printf ("\t%-5.2f\n",dup*lwaste);

printf ("\nactual waste = %g\n",twaste);
printf ("actual standard lengths = %d\n",nbar);

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
char f,line[81],dum[40];
int i,n;
real t;


if ((fp=fopen(dfile,"rt")) == NULL)
	{printf ("FAILED TO OPEN DATA FILE: %s",dfile); beep; return 1;}

np=0;			/* number of data entries */
while ((fgets(line,sizeof(line),fp)) != NULL)
	{
	trim(line);
	if ((strstr(line,"ENDOFDATA"))) break;
	if (line[0] != '\0' && line[0] != ';')
		{
		if (!bar)
		{
		strcpy (dum,strtok(line,"\t;\n"));
		bar=(real)atof(dum);
		}
		else
		{
		strcpy (dum,strtok(line,","));
		num[np]=(int)atof(dum);
		strcpy (dum,strtok(NULL,";\n"));
		size[np]=(real)atof(dum);
		if (++np == N)
			{
			printf
			("More than maximum (%d) data items in data file\n",N);
			quit;
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

return 0;
}
/* greedy..---------------------------------------------------------------- */

/* 
find the best greedy combination by exhaustive search.
s, the score for a combination, is used to resolve situations where the
waste of two different combinations is logically equal.
s is higher for combinations that contain larger pieces,
all other things being equal.

'best[]' contains the best solution
returns waste for best solution
*/

real greedy (int zeroflag)

{
int i,k;
int max[N],x[N];
dbl waste=1.e9,w,len,s,score=0.,p,min=size[np-1];


#if 0 == 1
Construct a (initial) bar cutting schedule (bcs) to test and store it in x[].
This initial bcs contains the maximum number of pieces still needed (previous
bcs may have reduced this amount from that input by user) of each size where
maximum is the lesser of:
		the number of pieces still required (num[i])
		the number of pieces of size size[i] that can be cut from a
		bar.
#endif

for (i=0 ; i<np ; i++)		/* initialization */
{
max[i]=MIN(ceil(bar/size[i]), num[i]);
x[i]=max[i];			/* describes combination being tested */
}

if (debug)
{
printf ("max = ");
for (i=0 ; i<np ; i++) printf ("%d,",max[i]); print;
}

/* In test, we're going to examine each entry in the test bcs */

test:;				/* test this combination */

#if 0 == 1
The for loop below just generates the length, wastage and score for the
bcs under test (x[]).
#endif

for (i=0,len=0.,s=0.,p=ldexp(1.,np) ; i<np ; i++,p*=.5)
{

#if 0 == 1
If no pieces of this size are required, we have nothing to do.  Pieces of this
size may have already been taken care of with previous bcs's.
#endif

if (x[i])
{
len+=x[i]*size[i];			/* total length of this combination */
if (diff(len,bar) > 0) goto nextc;	/* too much for available stock ? */

/* test below should *always* fail */
if (x[i] > num[i]) goto nextc;		/* safety valve */

#if 0 == 1
Each piece size (x[i] for current value of i) has a weight, p, associated with
i.  The more pieces of higher weight (and corresponding larger size) we can get
out of a bar, the higher the score for that bcs.  Put another way, the score is
higher for "greedier" bcs's.
#endif

s+=x[i]*p;				/* score for this combination */
}
}
if (debug) {for (i=0 ; i<np ; i++) printf ("%d,",x[i]); printf ("  %g\n",s);}
w=bar-len;			/* waste for this combination */
if (diff(w,0.) == 0) w=0.;	/* handles internal representation of reals */


/* Only examine zero waste solutions */
if (zeroflag == 0 && w != 0.) goto nextc;


#if 0 == 1
We've dropped out of the for loop that generated some data about this bcs.

waste is the best (smallest) wastage we've found so far.
w is the wastage for the bcs we're examining at the moment.
s is the score for the bcs we're examining at the moment.
min is the size of the smallest piece required (set in main)

If waste is already < min, and w<=waste (i.e., the current bcs has less wastage
than we've found so far), we want to save this bcs iff (if and only if) it's
"greedier" than the solution associated with the current value of waste. 
Greediness is proportional to the score, so we go to scr and test the score.
(If the score is better, this bcs will replace the previous one and w will
replace waste and s will replace score).  If w>waste we want to discard this
bcs and try again.  I don't remember what my thinking was with the:
		if (w<min) scr else nextc
test.  It seems if waste < min and w > waste then w can never be < min.  I'll
leave it in since it works but probably always ends up going to nextc.  Maybe
there's some subtle point I've forgotten at this later annotation.

If waste >= min and w < waste, it's a no-brainer.  Clearly this bcs is an
improvement on what we've found to this point and we go tuck it away as the
"bcs to beat" (done at saveit).

When waste >= min and w > waste, it's another no-brainer.  This bcs is just
plain worse than what we've already found and we go to nextc and generate
a new one to test.

When it's a tie (waste >= min and w = waste), we go to scr and let the score
decide.  If the current solution is "greedier", we'll save it, even though
there's no improvement in the amount of material wasted.  Even though we don't
reduce wastage, we want to be as greedy as possible since dealing with the big
pieces early opens up the flexibility for handling the smaller pieces later.

#endif

#if 0 == 1
previous method:
if (w > waste) goto nextc;	/* worse than best so far ? */
if ((diff(w,waste) == 0) && (s <= score)) goto nextc;

if the waste is too small to get another piece, we don't really care what it's
size is, e.g., for min=0.45, wasting 0.30 is no worse than wasting 0.15.

		waste < min		waste=min		waste > min
w < waste	scr			saveit			saveit
w = waste	scr			scr			scr
w > waste	if (w<min) scr		nextc			nextc
		 else nextc
#endif

/* The logic below just implements the logic diagram above. */

if (waste < min)
	{
	if (w > waste) {if (w < min) goto scr; else goto nextc;}
	goto scr;
	}
if (waste > min)
	{
	if (w < waste) goto saveit;
	if (w > waste) goto nextc;
	goto scr;
	}
/* waste == min */
if (w < waste) goto saveit;
if (w > waste) goto nextc;

scr: if (s > score) goto saveit; else goto nextc;
/* save this solution - either less waste or same waste with better score */
saveit: waste=w; score=s; for (i=0 ; i<np ; i++) best[i]=x[i];
if (debug)
{
printf ("best = ");
for (i=0 ; i<np ; i++) printf ("%d,",best[i]);
printf ("  %g  %g\n",waste,score);
}

#if 0 == 1

This is the logic to generate the next variant bcs to test. Remember that
x[0:np-1] is already ordered in decreasing order of sizes of pieces required. 
x[0] contains some number (could be zero) of the largest size pieces still
required and x[np-1] contains some number of the smallest size pieces still
required.  Note that once a bcs is found by greedy, the logic in main will
decrease num[] to reflect the pieces obtained by employing that bcs, so, as
more and more "good" bcs's are found (and reported on the screen), the values
in num[] will decrease, eventually reaching zero when no more pieces of that
size are needed.  When this is true more=0 in main, and the program stops.

An example explains it better than words.  Suppose:

		x=3,2,1 

(we still need 3 pieces of size[0], 2 of size[1], and 1 of size[2])

initially, where 3,2,1 are the max[] (computed during greedy initialization)
numbers required of each of (only three, for simplicity) sizes.

The logic below will then generate the following sequence of bcs's to test.

(3,2,1 initially, already tested before we reach the nextc logic)
2,2,1
1,2,1
0,2,1
3,1,1
2,1,1
1,1,1
0,1,1
3,0,1
2,0,1
1,0,1
0,0,1
3,2,0
2,2,0
1,2,0
0,2,0
3,1,0
2,1,0
1,1,0
0,1,0
3,0,0
2,0,0
1,0,0
0,0,0

at which point we'll fall through the nextc logic into the "return waste"
statement and leave the greedy routine.  (A bcs with all zero entries clearly
doesn't need to be tested - it also serves as an end indicator to the nextc
logic.)

Note that, in an orderly fashion, we've generated all possible combinations
consistent with needing no more than (3,2,1) of the three sizes.  What's more,
they're generated with the "greediest" ones early on in the process. 
Presumably one of these will be the winner and, if found early, will speed up
the process of ignoring the less greedy solutions.

I'll leave it as an exercise for the reader to decide how I managed to do that
with only three lines of code :-) (With an "else", I could have made it two
lines!)
#endif

nextc:; k=0;			/* construct next combination */
nc2: if (x[k] > 0) {x[k]--; goto test;}
x[k]=max[k]; if (++k < np) goto nc2;
return waste;
}
/* ------------------------------------------------------------------------ */
