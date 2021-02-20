/* VERNIER.C   M. W. Klotz   11/98, 4/02, 4/04

Calculations for two plate verniers

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
char cp (void);
void pout (void);

/* --------------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* struc..----------- global structure declarations ----------------------- */

/* vmark..------------ global variable declarations ----------------------- */
FILE *fp;				/* utility file pointer */
char ofile[]="VERNIER.OUT";		/* output file */
char alph[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
char _huge *prime;
ulong np=66000L,num,max;
ulong factors[20];
int exps[20],index;
ulong holes,sum,n1s,n2s,n1,n2;
# define nb 500
int bp1[nb],bp2[nb],sum1,sum2;

/* main..====xxxxxxxx====================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i,j,k;
char string[20];

/* -------------------------- initializations ------------------------------ */
/* ------------------------------------------------------------------------- */

/*
these bytes form a sequence of 66000 flags
prime[n] will be set to TRUE if n is prime otherwise it is set to FALSE
*/
prime = (char _huge *)halloc(np,sizeof(char));
if (prime == NULL) {puts ("insufficient memory"); exit(1);}

puts ("TWO PLATE ANGULAR VERNIER COMPUTATIONS\n");

max=(1 << 31) -1;
vin ("Number of angular subdivisions required",&num,"lu",360.,"");

if (num <= 1) {beep; puts ("MUST BE GREATER THAN 1"); goto mexit;}

calcp(num);			/* calculate prime factors needed */

factor (num);			/* factor number */

cprnt (num); printf (" = ");
for (j=1 ; j<=index ; j++)	/* print factors */
{
if (j > 1) printf(" ù ");
cprnt (factors[j]); if (exps[j] > 1) printf ("^%d",exps[j]);
}
print;

/* 
find the factor pair which requires the minimum number of holes to drill
and still produces the requisite number of subdivisions
*/

holes=max; n1s=0; n2s=0;
for (n1=2 ; n1<num ; n1++)
	{
	n2=num*n1/(num-n1); 		 // 1/n1 - 1/n2 = 1/num
	if (n2 == 0) continue;
	if (n1 == n2 || n2 == 1) continue;
	if (n1+n2 > holes) continue;
	if (cp()) continue;
	holes=n1+n2; n1s=n1; n2s=n2;
	}

if (holes == max) {puts ("NO SOLUTION WAS FOUND"); goto mexit;}
n1=n1s; n2=n2s; if (n1 > n2) {n1s=n1; n1=n2; n2=n1s;}
printf ("selected %lu,%lu\n",n1,n2);
if (holes > num)
{
printf ("\nTwo plate vernier requires drilling more holes (%lu) \n",holes);
printf ("than a single plate. Use a single plate.\n");
goto mexit;
}
pout ();
printf ("YOUR OUTPUT IS ON FILE: %s\n",ofile);

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

/* cp..-------------------------------------------------------------------- */

/* check a combination */

char cp (void)

{
ulong nd,i1,i2;
dbl ang,p1,p2,divs,div1,div2;

divs=(dbl)360/num; div1=(dbl)360/n1; div2=(dbl)360/n2;
for (nd=1 ; nd<=n1 ; nd++)
	{
	ang=nd*divs;
	for (i1=0 ; i1<=n1-1 ; i1++)
		{
		p1=i1*div1;
		for (i2=0 ; i2<=n2-1 ; i2++)
			{
			p2=i2*div2+ang;
			if (p2 >= 360.) p2-=360.;
			if (ABS(p2-360.) < 0.0001) p2=0.;
			if (ABS(p2-p1) < 0.0001) goto nexn;
			}
		}
		return 1;
nexn:;
	}
return 0;
}
/* pout..------------------------------------------------------------------ */

/* print the output file */

void pout (void)

{
char buf[80],dum[40],*p;
ulong nd,i1,i2,ip;
dbl ang,p1,p2,divs,div1,div2;

divs=(dbl)360/num; div1=(dbl)360/n1; div2=(dbl)360/n2;
if ((fp=fopen(ofile,"wt")) == NULL)
	{printf ("FAILED TO OPEN OUTPUT FILE: %s\n",ofile); quit;}

fprintf (fp,"DATA FOR %lu DIVISION TWO PLATE VERNIER\n\n",num);
fprintf (fp,"Total number of holes = %lu\n\n",n1+n2);
fprintf
(fp,"Assume holes in plate with %lu holes are labeled with LETTERS.\n",n1);
fprintf
(fp,"Assume holes in plate with %lu holes are labeled with NUMBERS.\n",n2);
fprintf (fp,"Either plate can be the movable plate.\n");
fprintf (fp,"Assume 'A' and '1' holes are aligned initially.\n");
fprintf
(fp,"To reverse the direction of rotation, read the list backwards.\n\n");
fprintf (fp,"Division  Angle(deg)  Holes to Align\n\n");
for (i1=0 ; i1<nb ; i1++) {bp1[i1]=0; bp2[i1]=0;}
for (nd=0 ; nd<=num ; nd++)
	{
	ang=nd*divs;
	for (i1=0 ; i1<=n1-1 ; i1++)
		{
		p1=i1*div1;
		for (i2=0 ; i2<=n2-1 ; i2++)
		{
		p2=i2*div2+ang;
		if (p2 >= 360.) p2-=360.;
		if (ABS(p2-360.) < 0.0001) p2=0.;
		if (ABS(p2-p1) < 0.0001)
			{
			ip=i1; p=&buf[1];
			while (ip >= 52) {ip-=52; *p=39; p++;}
			buf[0]=alph[ip];
			*p='\0';
			ultoa (i2+1,dum,10);
			strcat (buf,dum);
			fprintf (fp,"%8lu  %10.4lf  %14s\n",nd,ang,buf);
			bp1[i1]=1; bp2[i2+1]=1;
			goto nexn;
			}
		}
		}
nexn:;
	}
for (i1=0,sum1=0,sum2=0 ; i1<nb ; i1++) {sum1+=bp1[i1]; sum2+=bp2[i1];}
fprintf (fp,"\nTotal holes to drill on letter plate = %d\n",sum1);
fprintf (fp,"Total holes to drill on number plate = %d\n",sum2);
return;
}
/* ------------------------------------------------------------------------ */
