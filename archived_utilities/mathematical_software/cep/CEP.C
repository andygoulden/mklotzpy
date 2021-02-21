/* BEGIN.C   M. W. Klotz   6/07

Circular Error Probable


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
dbl cep (dbl x1, dbl sig1, dbl x2, dbl sig2, dbl rho, dbl pcep);

/* gmark..-------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* vmark..------------ global variable declarations ----------------------- */

dbl a[4]={.69329763813785,-1.4621993839667,1.06372876137083,-.203505733447903};
dbl b[3]={1,-1.77649593676893,.977186538295758};
dbl cepchk;

/* smark..----------- global structure declarations ----------------------- */

/* main..================================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i,j,k;
dbl x1,x2,sig1,sig2,rho,pcep,c;



/* -------------------------- initializations ------------------------------ */

/* ------------------------------------------------------------------------- */

print;
vin ("Mean1",&x1,"lf",0.,"");
vin ("Sigma1",&sig1,"lf",1.,"");
vin ("Mean2",&x2,"lf",0.,"");
vin ("Sigma2",&sig2,"lf",1.,"");
vin ("Rho",&rho,"lf",0.,"");
vin ("CEP probability",&pcep,"lf",0.5,"");

c=cep(x1,sig1,x2,sig2,rho,pcep);

printf ("\nCEP(%.4lf) = %.4lf\n",pcep,c);
if (cepchk) printf ("Wilcox cepchk = %.4lf\n",cepchk);

/* post run processing */
mexit:
fcloseall();
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* cep..------------------------------------------------------------------- */

dbl cep (dbl x1, dbl sig1, dbl x2, dbl sig2, dbl rho, dbl pcep)

{
int k,ntheta;
dbl c11,c22,c12,psi,spsi,cpsi,t,s1,s2,ts,deltar,r,p,d,pl,rl,dth;
dbl th,stheta,ctheta,u,v,trp,m;


c11=sig1*sig1; c22=sig2*sig2; c12=rho*sig1*sig2;
if ((c11 != c12) || (c12 != 0)) psi=ATAN2D(2.*c12,c11-c12); else psi=0;
spsi=SIND(psi); cpsi=COSD(psi);
t=sqrt((c11-c22)*(c11-c22)+4*c12*c12); s1=0.5*(c11+c22+t); s2=0.5*(c11+c22-t);
ts=sqrt(s1*s2); if (ts == 0) return -1;

deltar=0.001*sqrt(min(s1,s2));	// size of radial step
ntheta=72;			// number of angular steps
r=0; p=0; d=deltar/(ntheta*ts); dth=2.*PI/ntheta;

do
{
ts=0; pl=p; rl=r; r+=deltar;
for (k=1 ; k<=ntheta ; k++)
	{
	th=k*dth; stheta=sin(th); ctheta=cos(th);
	u=(r*ctheta-x1)*cpsi + (r*stheta-x2)*spsi;
	v=(r*stheta-x2)*cpsi - (r*ctheta-x1)*spsi;
	ts+=exp(-0.5*(u*u/s1+v*v/s2));
	}
p+=r*d*ts;
} while (p < pcep);

//ANALYTIC COMPUTATION OF CEP (WILCOX'S ALGORITHM)

cepchk=0;
if ((x1 == 0) && (x2 == 0) && (pcep == 0.5))
{
trp=c11+c22; m=(atob(c11-c22,2.)+4.*c12*c12)/(trp*trp);
cepchk=sqrt(trp*(((a[3]*m+a[2])*m+a[1])*m+a[0])/((b[2]*m+b[1])*m+b[0]));
};

return rl+(pcep-pl)*(r-rl)/(p-pl);	// interpolate
}
/* ------------------------------------------------------------------------ */
