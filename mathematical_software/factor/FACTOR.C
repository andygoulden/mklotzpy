/* FACTOR.C   M. W. Klotz   4/98

Factor numbers < 2^32-1 = 4,294,967,295
*/

/* head..-------------------- header files -------------------------------- */

/* Unneeded headers can be left in place - the header files consist mainly of
'defines' for the compiler and do not add to the size of the executable code.
*/

/* turns off assert macros when not debugging */
/* #define NDEBUG */

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
typedef double dbl;		/* 8 byte,  15 digit, 10^+/-308 */
typedef float real;		/* 4 byte   7 digit, 10^+/-38 */
/* long		   		   4 byte  -2,147,483,647 to 2,147,483,648 */
typedef unsigned long ulong;	/* 4 byte   0 to 4,294,967,295 */
/* int				   2 byte  -32768 to 32767 */
typedef unsigned int uint;	/* 2 byte   0 to 65535 */
/* char				   1 byte   -128 to 127 */
typedef unsigned char uchar;	/* 1 byte   0 to 255 */

/* macro declarations */
#define _in stdin			/* makes redirection easier */
#define _out stdout
#define pause getch()
#define pausem getkey("press a key\n")
#define print printf("\n")
#define FALSE 0
#define TRUE !FALSE

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
ulong calcp (ulong num);
int factor (ulong n);
void addf (ulong factor);
void cprnt (ulong x);
void pifact (void);
void chkit (ulong c, ulong d);

/* --------------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* struc..----------- global structure declarations ----------------------- */

/* vmark..------------ global variable declarations ----------------------- */
char _huge *prime;
ulong np=66000L,num;
ulong factors[20];
int exps[20],index,kf;
#define NFX 500
ulong fx[NFX];
char flag;

/* main..====xxxxxxxx====================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i,j;
char string[20];

/* -------------------------- initializations ------------------------------ */
/* ------------------------------------------------------------------------- */

/*
these bytes form a sequence of 66000 flags
prime[n] will be set to TRUE if n is prime otherwise it is set to FALSE
*/
prime = (char _huge *)halloc(np,sizeof(char));
if (prime == NULL) {puts ("insufficient memory"); exit(1);}

if (argc == 1)
{
printf ("number to factor ? max = "); cprnt ((1 << 31)-1); print;
fgets(string,sizeof (string),_in); num=atol(string);
}
else num=atol(argv[1]);

calcp(num);			/* calculate prime factors needed */

if (num < 3) {printf ("%lu = %lu\n",num,num); goto mexit;}

factor (num);			/* factor number */

cprnt (num); printf (" = ");
for (j=1 ; j<=index ; j++)	/* print factors */
{
if (j > 1) printf(" ù ");
cprnt (factors[j]); if (exps[j] > 1) printf ("^%d",exps[j]);
}
print;
pifact ();

/* post run processing */
mexit:
hfree (prime);			/* free buffer */
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* calcp..----------------------------------------------------------------- */

/* calculate primes up to sqrt (num) */

ulong calcp (ulong num)
{
ulong upper,j,k;

upper = 1+(ulong)sqrt((dbl)num);
upper+=upper%2;		/*  make upper even if it is odd  */

/*  set all bytes with even index to FALSE (non-prime)     */
/*  and all bytes with odd index to TRUE (possibly prime)  */

for (j=0 ; j<=upper ; j+=2) *(prime+j)=0;
for (j=1 ; j<upper ; j+=2) *(prime+j)=1;

/*  mark all multiples of primes as non-prime (FALSE)  */
for (j=3 ; j<upper/2 ; j+=2)
    {if (*(prime+j)) {for (k=j+j ; k<upper ; k+=j) *(prime+k)=FALSE;}}

j=upper-1; while (!*(prime+j)) j-=2;

return j;		/*  largest prime calculated  */
}

/* factor..---------------------------------------------------------------- */

/* factor n into prime factors */

int factor (ulong n)

{
ulong rtn,j;

rtn = (ulong)sqrt((dbl)n);
index = 0; factors[0] = 0L;

/*  extract factors of 2  */
while (!(n % 2))		/* while n is divisible by 2 */
    {n/=2L; addf(2L);}
if (n == 1L) return FALSE;	/* n is a power of 2, so not prime */

j = 3L;

/*  extract the odd factors  */
while (j <= rtn)
    {
    if (*(prime+j))		/* if j is prime */
        {
        if ((n % j) == 0)	/* if n is divisible by j */
            {
            addf(j);		/* add j to factors[] */
            n/=j;		/* n = n/j */
            if (n < j) break;
            }
        else
            j+=2;
        }
    else
        j+=2;
    }
if (n != 1) addf(n);		/* n may exceed 65,535 */

return ((index == 1) && (exps[index] == 1));
/* returns TRUE if only factor (other than 1) is n itself */
}
/* addf..------------------------------------------------------------------ */

/*  add factor to factors array  */

void addf (ulong factor)

{
if (factors[index] != factor)
    {index++; factors[index]=factor; exps[index]=1;}
else exps[index]++;
}
/* cprnt..----------------------------------------------------------------- */

/* print number with comma separators */

void cprnt (ulong x)

{
char buf[50],*b,num[50];

sprintf (buf,"%lu",x);
for (b=&buf[strlen(buf)-3] ; b > &buf[0] ; b-=3)
	{
	if isdigit ((int)(*(b-1)))
	  {strcpy (num,b); *b=','; strcpy (b+1,num);}
	}
printf ("%s",buf);
}

/* pifact..---------------------------------------------------------------- */

/* print integer factors */

void pifact (void)

{
int i,j,kff;
ulong c,d;
dbl y;

kf=0;
for (i=0 ; i<=index ; i++)
{
for (j=1 ; j<=exps[i] ; j++) {c=pow(factors[i],(ulong)j); chkit (c,num/c);}
}

do
{
flag=0; kff=kf;
	for (i=0 ; i<kff-1 ; i++)
	{
		for (j=i+1 ; j<kff-1 ; j++)
		{
		c=fx[i]*fx[j]; d=num/c; 
		if (!modf((dbl)num/(dbl)c,&y)) chkit (c,d);
		}
	}
} while (flag);

do
{
for (i=0,flag=0 ; i<kf-1 ; i++)
  {if (fx[i] > fx[i+1]) {c=fx[i]; fx[i]=fx[i+1]; fx[i+1]=c; flag=1;}}
} while (flag);

printf ("factors = "); for (i=0 ; i<kf ; i++) {printf (" | "); cprnt (fx[i]);}
}

/* chkit..----------------------------------------------------------------- */

/* utility for pifact */

void chkit (ulong c, ulong d)

{
int k;

if (c > num || d > num) return;
if (c == 0 || d == 0) return;
if (kf) {for (k=0 ; k<kf ; k++) {if (fx[k] == c || fx[k] == d) return;}}
if (kf >= (NFX-3)) {puts ("insufficient fx storage"); exit (1);}
fx[kf++]=c; flag=1; if (c != d) fx[kf++]=d;
}
/* ------------------------------------------------------------------------ */
