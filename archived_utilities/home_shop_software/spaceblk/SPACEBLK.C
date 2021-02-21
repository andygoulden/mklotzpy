/* SPACEBLK.C   M. W. Klotz  11/98, 1/00, 10/09, 01/10

Select blocks to obtain a given size spacer

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
char *trim (char *s);
char rdata (void);
int kp (dbl x, int k);

/* gmark..-------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* vmark..------------ global variable declarations ----------------------- */
FILE *fp;		/* utility file pointer */
char sbfile[] = "SPACEBLK.DAT";		/* block data file */
#define NB 500		/* maximum number of blocks */
int nb;			/* actual number of blocks */
dbl blk[NB];		/* available block sizes */
char kblk[NB];		/* bookkeeping */
dbl bl;			/* total length of all the blocks */

#define LMAX 10		/* maximum number of loops */
int index[LMAX];	/* indices for loops */
int llimit[LMAX];	/* loop lower limits */
int ulimit[LMAX];	/* loop upper limits */
int inc[LMAX];		/* loop increment */
int nlm=5;		/* actual number of loops */
int nl;			/* loop number */

/* smark..----------- global structure declarations ----------------------- */

/* main..================================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i,j,k,l,dp=9;
dbl x,sum,t,d;
ulong u;

/* -------------------------- initializations ------------------------------ */
textscreen ();
/* ------------------------------------------------------------------------- */

if (rdata()) goto mexit;
puts ("SPACEBLOCK SELECTION UTILITY\n");
printf ("number of blocks read from data file = %d\n",nb);
for (i=0,bl=0 ; i<nb ; i++) bl+=blk[i];		/* total length */
printf ("total length of all blocks = %.4lf\n",bl);
printf ("largest  block = %.4lf\n",blk[0]);
printf ("smallest block = %.4lf\n\n",blk[nb-1]);

vin ("Required spacer size",&x,"lf",0.104,"");

x=1e-4*floor(x*1e4+.5);		//truncate to 4 places

if (x < blk[nb-1])
  {
  beep;
  puts ("required size smaller than smallest available block\n");
  goto mexit;
  }
if (x > bl)
  {
  beep;
  puts ("required size larger than total length of all blocks\n");
  goto mexit;
  }
for (i=0 ; i<nb ; i++) kblk[i]=0;	/* zero bookkeeping array */

/* ------------ */
/* skip over blocks bigger than spacer required */
for (j=0,k=0 ; j<nb ; j++){if (blk[j] > x) {k=j; break;}}
/* set loop limits and increments */
for (l=0 ; l<nlm ; l++) {llimit[l]=k; ulimit[l]=nb; inc[l]=1;}

/* try just one loop, then two, then ... up to nlm loops */
for (nl=1 ; nl<=nlm ; nl++)
{

printf ("trying %d block combinations\n",nl);

/* initialize loop indices */
for (l=0 ; l<nl ; l++) index[l]=llimit[l];

repeat:			/* start of nested looping */

if (kbhit()) goto mexit;	/* bailout */

#if 0 == 1
for (l=0 ; l<nl ; l++) printf ("%d:",index[l]); print;	/* print indices */
#endif

/* can't use the same block twice */
for (j=0 ; j<nl ; j++)
{for (i=0 ; i<nl ; i++) {if (i != j && index[i] == index[j]) goto increment;}}

/* nested loop processing here */
for (l=0,t=0 ; l<nl ; l++)
{
t+=blk[index[l]];

//printf ("l,index[l},t = %d, %d, %lf\n",l,index[l],t);

if ((t-x) > 1.e-8) goto increment;
}
d=ABS(t-x);
if (d < 1.e-8) {for (l=0 ; l<nl ; l++) kblk[index[l]]=1; goto done;}

increment:			/* increment loop indices */
for (j=nl-1 ; j>=0 ; j--)
{
index[j]+=inc[j];
if (inc[j] > 0) {if (index[j] < ulimit[j]) goto repeat;}
if (inc[j] < 0) {if (index[j] > ulimit[j]) goto repeat;}
index[j]=llimit[j];
}
}	/* end of loop on nl */
/* ------------ */

puts ("\nCAN'T FIND A SOLUTION\n"); goto mexit;

done: puts ("\nUSE BLOCKS:\n");
for (i=0,sum=0 ; i<nb ; i++)
	{if (kblk[i]) {printf ("%.4lf\n",blk[i]); sum+=blk[i];}}
printf ("TOTAL LENGTH = %.4lf\n",sum);

/* post run processing */
mexit:
fcloseall();
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* trim..------------------------------------------------------------------ */

/* trim leading and trailing white space from a string */
/* returns pointer to trimmed string */

char *trim (char *s)
{
int l;
char *b;

l=strlen(s); l--;		/* truncate trailing white space */
while (isspace(*(s+l))) {l--;}
*(s+l+1)='\0';
b=s;				/* find first non-whitespace character */
while (isspace(*b)) {b++;}
return b;
}
/* rdata..----------------------------------------------------------------- */

/* read the block data */

char rdata (void)

{
char *line,temp[81],f;
int i;
dbl t;

if ((fp=fopen(sbfile,"rt")) == NULL)
	{printf ("FAILED TO OPEN BLOCK DATA FILE: %s",sbfile); beep; return 1;}

nb=0;			/* number of blocks */
while ((fgets(temp,sizeof(temp),fp)) != NULL)
	{
	line=trim(temp);
	if ((strstr(line,"ENDOFDATA")) != NULL) break;
	if (line[0] != '\0' && line[0] != ';')
		{blk[nb++]=atof(trim(strtok(temp,";")));}
	}
fclose (fp);

do
{
for (i=0,f=0 ; i<nb-1 ; i++)
	{
	if (blk[i] < blk[i+1]) {t=blk[i]; blk[i]=blk[i+1]; blk[i+1]=t; f=1;}
	}
} while (f);
return 0;
}
/* kp..-------------------------------------------------------------------- */

/*
return kth decimal place of a non-integerial number

if x=123.456 then kp (x,k) yields:
k	kp
4	0
3	6
2	5
1	4
0	0	decimal point always returns zero
-1	3
-2	2
-3	1
-4	0
*/

int kp (dbl x, int k)

{
char temp [40],buf[4];
int i;

if (k == 0) return 0;
sprintf (temp,"%-+20.9f",x);
i=0; while (temp[i] != '.') i++;
buf[0]=temp[k+i]; buf[1]='\0'; return atoi(buf);
}
/* ------------------------------------------------------------------------ */
