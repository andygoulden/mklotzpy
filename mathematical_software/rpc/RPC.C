/* RPC.C   M. W. Klotz  12/98

Reverse Polish Notation scientific calculator

*/

/*--------------------------- header files ---------------------------------*/

/* turns off assert macros when not debugging */
/* #define NDEBUG */

#include <conio.h>
#include <ctype.h>
#include <dos.h>
#include <float.h>
#include <graph.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*----------------------------- defines -------------------------------------*/


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
#define pausem getkey("press a key")
#define beep tone(440,100L)
#define bpause beep; pause
#define bpausem beep; pausem
#define print printf("\n")
#define cls _clearscreen(0)
#define bbr {beep; break;}

/* global utility functions (can return only one value)
   note: no space between function name and parameter list
*/
#define SGN(a) ((a)!=0 ? ((a)>0 ? 1 : -1) : 0)

/* scan codes for keys */
#include <keycode.h>

/* -------------------------- function prototypes ------------------------- */

/* pmark..  functions peculiar to this program */
void mouse (uint a, uint b, uint c, uint d);
void mouseinit (void);
void button (void);
void menu (void);
void clear (int k);
void display (void);
void vp (dbl x);
char getky (void);
char kbranch (void);
char mbranch (void);
void push (void);
void pop (void);
void rollup (void);
void rolldn (void);
void chk (void);
void entr (void);
dbl query (char *prompt, dbl def);
char chs (void);
int dplaces (dbl x);
void notes (void);
char *ccat (char *str, char c);
dbl tconv (void);
dbl wconv (void);
dbl lconv (void);
void save (void);
dbl gcd (dbl x, dbl y);

/* --------------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* ------------------ global structure declarations ----------------------- */

struct mousestruc
{
uint a,b,c,d;
int row,col,key;
} mo;

/* vmark..------------ global variable declarations ----------------------- */

dbl pi;			/* calculated for consistency with C */
dbl dpr,rpd;		/* using calculated pi */
char gp[80];		/* general purpose buffer */
#define NS 4		/* number of stack elements */
dbl stk[NS];		/* stack */
dbl lastx;		/* last x value */
dbl mem;		/* memory */
dbl swp[NS+2];		/* swap storage */
dbl undo[NS+2];		/* undo storage */
unsigned int key;	/* last key pressed */
unsigned int kbflag;	/* last value of keyboard flag */
char gotakey;		/* signals key press while waiting for mouse */
char dtype=0;		/* display flag  0=fixed, 1=engineering, 2=scientific */
int decimals=2;		/* number of decimal places */
char dflag=0;		/* decimals displayed 0=decimals, 1=self adjusting */
char entry[40];		/* pending entry */
char base=10;		/* number input base */
char amode=0;		/* angle mode  0=deg, 1=rad */
int tfrom=0;		/* from temperature */
int tto=1;		/* to temperature */
char tempr[]="FCRK";	/* temperature scales */
int wfrom=1;		/* from weight */
int wto=3;		/* to weight */
char wgt[]="ozlbgmkg";	/* weight scales */
int lfrom=0;		/* from length */
int lto=4;		/* to length */
char leng[]="inftydmimmcmm.km";		/* length scales */

/* main..================================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i,j,k;

/* -------------------------- initializations ------------------------------ */
savescreen ();
textscreen();
mouseinit();
pi=acos(-1.); dpr=180./pi; rpd=pi/180.;
clear (0);
menu ();
display ();
/* ------------------------------------------------------------------------- */

agin: button();		/* wait for mouse button or key press */

if (gotakey)		/* process key press */
	{
	if (kbranch ()) goto mexit;
	goto agin;
	}

switch (mo.key)		/* process mouse button press */
{
case 1:			/* left */
	if (mbranch ()) goto mexit;
	break;
case 2:			/* right */
	beep; break;
case 3:			/* leftright */
	goto mexit;
/* ignore middle button on three button mice */
case 4:			/* middle */
	beep; break;
case 5:			/* leftmiddle */
	beep; break;
case 6:			/* rightmiddle */
	beep; break;
case 7:			/* alldown */
	beep; break;
default:
	beep; break;
}
goto agin;

/* post run processing */
mexit:
mouse (2,0,0,0);			/* turn cursor off */
restorescreen ();
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* mouse..----------------------------------------------------------------- */

/* assembler call to mouse interrupt */

void mouse (uint a, uint b, uint c, uint d)

{
_asm
	{
	mov	ax,a
	mov	bx,b
	mov	cx,c
	mov	dx,d
	int	33h
	mov	mo.a,ax
	mov	mo.b,bx
	mov	mo.c,cx
	mov	mo.d,dx
	}
}

/* mouseinit..------------------------------------------------------------- */

/* initialize the mouse */

void mouseinit (void)

{
int rowt=0,rowb=24,coll=0,colr=79;

mouse (0,0,0,0);			/* initialize */
mouse (10,0,0xffff,0x5e00);		/* set cursor characteristics */
mouse (7,0,8*coll,8*colr);		/* limit column position */
mouse (8,0,8*rowt,8*rowb);		/* limit row position */
mouse (15,0,3,9);			/* set motion */
mouse (4,0,4*colr,4*rowb);		/* position cursor */

}
/* button..---------------------------------------------------------------- */

/* process mouse buttons */

void button (void)

{
uint mmax;
char c;

gotakey=0;
mouse (1,0,0,0);			/* turn cursor on */
waite: mouse (3,0,0,0);			/* wait for a press */
if ((gotakey=getky())) return;		/* keyboard check */
if (mo.b == 0) goto waite;
mmax=mo.b;
recheck: mouse (3,0,0,0);		/* wait for all buttons to be pressed */
if (mo.b > mmax) mmax=mo.b;
if (mo.b != 0) goto recheck;
mo.key=mmax; mo.row=(int)(mo.d/8+1); mo.col=(int)(mo.c/8+1);
}
/* clear..----------------------------------------------------------------- */

/* clearing utility */

void clear (int k)

{
int i;

switch (k)
{
	case 0:		/* clear all */
	mem=0.; lastx=0.;

	case 1:		/* clear stack */
	for (i=0 ; i<NS ; i++) stk[i]=0.;

}
}
/* display..--------------------------------------------------------------- */

/* display the stack, etc. */

void display (void)

{
char buf[60],*g,bin[]="000001010011100101110111",bp[2];
int i,r=1,c,f=14,b=2;
dbl x,fp;

mouse (2,0,0,0);
c=40;
vp (mem);    strcat (gp," M"); ptext (gp,r++,c,15,b);
vp (lastx);  strcat (gp," L"); ptext (gp,r++,c,10,b);
r++;
vp (stk[3]); strcat (gp," T"); ptext (gp,r++,c,f,b);
vp (stk[2]); strcat (gp," Z"); ptext (gp,r++,c,f,b);
vp (stk[1]); strcat (gp," Y"); ptext (gp,r++,c,f,b);
vp (stk[0]); strcat (gp," X"); ptext (gp,r++,c,15,b);

/* clear printing areas */
sprintf (gp,"                                         ");
ptext (gp,10,c,11,b);
for (r=12 ; r<15 ; r++) ptext (gp,r,c,11,0);

/* print pending entry, if any */
if (strlen(entry)) ptext (entry,10,c+2,11,b);

fp=modf(stk[0],&x);
if (!fp && fabs(x) <= 0xffffffff)
{
sprintf (gp," HEX: %lX",(ulong)x); ptext (gp,12,c,10,0);

sprintf (gp," OCT: %lo",(ulong)x); ptext (gp,13,c,7,0);

buf[0]='\0';
sprintf (gp,"%lo",(ulong)x);
for (g=&gp[0] ; (*g)!='\0' ; g++)
	{bp[0]=*g; bp[1]='\0'; strncat (buf,&bin[3*atoi(bp)],3);}
g=&buf[0];
while (*g == '0') strcpy (buf,g+1); if (!strlen(buf)) strcpy (buf,"0");
sprintf (gp," BIN: %s",buf); ptext (gp,14,c,15,0);
}

umenu:			/* update switches on menu */
b=6;
sprintf (gp,"%c",tempr[tfrom]); ptext (gp,15,2,f,b);
sprintf (gp,"%c",tempr[tto]); ptext (gp,15,5,f,b);
sprintf (gp,"%c%c",wgt[2*wfrom],wgt[2*wfrom+1]); ptext (gp,15,9,f,b);
sprintf (gp,"%c%c",wgt[2*wto],wgt[2*wto+1]); ptext (gp,15,12,f,b);
sprintf (gp,"%c%c",leng[2*lfrom],leng[2*lfrom+1]); ptext (gp,15,17,f,b);
sprintf (gp,"%c%c",leng[2*lto],leng[2*lto+1]); ptext (gp,15,20,f,b);
sprintf (gp,"FIX...."); ptext (gp,18,1,((dtype == 0)? 15 : f),b);
sprintf (gp,"ENG...."); ptext (gp,18,9,((dtype == 1)? 15 : f),b);
sprintf (gp,"SCI...."); ptext (gp,18,17,((dtype == 2)? 15 : f),b);
sprintf (gp,".dd...."); ptext (gp,18,25,((dflag == 0)? 15 : f),b);
sprintf (gp,"ADJ...Z"); ptext (gp,18,33,((dflag == 1)? 15 : f),b);
sprintf (gp,"DEC...."); ptext (gp,19,1,((base == 10)? 15 : f),b);
sprintf (gp,"HEX...."); ptext (gp,19,9,((base == 16)? 15 : f),b);
sprintf (gp,"BIN...."); ptext (gp,19,17,((base == 2)? 15 : f),b);
sprintf (gp,"OCT...."); ptext (gp,19,25,((base == 8)? 15 : f),b);
if (amode) sprintf (gp,"RAD...."); else sprintf (gp,"DEG....");
ptext (gp,19,33,15,b);
mouse (1,0,0,0);
_settextposition (22,1);
}
/* vp..-------------------------------------------------------------------- */

/* generalized variable print routine */

void vp (dbl x)

{
char buf[40],b2[40],fp[20],*l,*b,dsave;
int k;
dbl xd,zd;

if (x) zd=log10(fabs(x)); else zd=1.;
dsave=dtype; if (fabs(zd) > 15 && dtype == 0) dtype=1;
itoa(dplaces(x),b2,10);

if (dtype == 0)		/* fixed point display */
	{
	strcpy (fp,"%.");
	strcat (fp,b2); strcat (fp,"lf");
	sprintf (buf,fp,x);
	if (fabs(x) >= 1000.)		/* add comma separators */
		{
		if ((l=strrchr(buf,'.'))) l-=3;
  		  else {l=&buf[0]+strlen(buf); while (!isdigit(*l)) l--; l-=2;}
		for (b=l ; b>&buf[0] ; b-=3)
	  	  {if isdigit ((int)(*(b-1)))
	    	  {strcpy (b2,b); *b=','; strcpy (b+1,b2);}}
		}
	}

if (dtype == 1)		/* engineering notation */
	{
	xd=fabs(x); zd=x; k=0;
	if (xd != 0.)
		{
		k=floor(log10(xd)); zd=xd*pow(10.,-k);
		while (((abs(k) % 3) != 0) || (zd < 1)) {zd*=10; k--;}
		zd*=SGN(x);
		}
	strcpy (fp,"%.");
	strcat (fp,b2);
	strcat (fp,"lf E%+04d");
	sprintf (buf,fp,zd,k);
	}

if (dtype == 2)		/* scientific notation */
	{
	strcpy (fp,"%.");
	strcat (fp,b2);
	strcat (fp,"lE");
	sprintf (buf,fp,x);
	b=strchr (buf,'E');	/* insert blank before 'E' */
	strcpy (b2,b); *b++=' '; *b='\0'; strcat (buf,b2);
	}

while (strlen(buf) < 39)	/* pad with leading blanks */
  {strcpy (b2,buf); strcpy (buf," "); strcat (buf,b2);}
strcpy (gp,buf);
dtype=dsave;
}
/* dplaces..--------------------------------------------------------------- */

/* find the number of decimal places needed to print a real */

int dplaces (dbl x)

{
char buf[40],fp[10],*l;
int d,dk;
dbl acc=1.e-15,wd,ix,fx;

if (dtype) return decimals;
fx=modf(fabs(x),&ix);
dk=((dflag) ? 0 : decimals);
if (fx == 0.) return dk;		/* no fractional part */
for (d=dk ; d<16 ; d++)
	{
	strcpy (fp,"%.");		/* form trial format */
	strcat (fp,itoa(d,buf,10));
	strcat (fp,"lf");
	sprintf (buf,fp,x);		/* print in test format */
	l=strchr (buf,'\0')-1;		/* convert back */
	wd=strtod (buf,&l);		/* to number */
	if (dflag)			/* test difference */
	  {if (fabs(x-wd) < acc) break;}	/* adjustable */
	else
	  {if (wd != 0.) break;}		/* fixed */
	}
return d;
}
/* getky..----------------------------------------------------------------- */

/*
check for a keypress
returns 0 if none available else returns 1 with keypress stored in key
*/

char getky (void)

{
char retv;
unsigned int kk,flag;

_asm
	{
	mov	retv,0		;assume no keypress
	mov	ah,1		;check for a keypress
	int	16h
	jz	gk1		;no keypress available
	mov 	ah,0		;get the keypress
	int	16h
	mov	kk,ax
	mov	ah,2		; and the keyboard flag
	int	16h
	mov	flag,ax
	mov	retv,1		;indicate a keypress
gk1:
	}
if (retv) {key=kk; kbflag=flag;}
return retv;
}
/* push..------------------------------------------------------------------ */

void push (void)

{
int i;

for (i=NS-1 ; i>0 ; i--) stk[i]=stk[i-1];
}
/* pop..------------------------------------------------------------------- */

void pop (void)

{
int i;

for (i=0 ; i<NS-1 ; i++) stk[i]=stk[i+1];
}
/* rollup..---------------------------------------------------------------- */

void rollup (void)

{
dbl t;

t=stk[NS-1]; push (); stk[0]=t;
}
/* rolldn..---------------------------------------------------------------- */

void rolldn (void)

{
dbl t;

t=stk[0]; pop (); stk[NS-1]=t;
}
/* chk..------------------------------------------------------------------- */

/* check for a pending entry and enter it */

void chk (void)

{
if (strlen(entry)) entr ();
}
/* entr..------------------------------------------------------------------ */

void entr (void)

{
char *c;
dbl x;

lastx=stk[0];
if (strlen(entry))
	{
	if (base == 10) x=atof(entry);
	else
		{
		c=&entry[strlen(entry)];
		x=(dbl)strtoul (entry,&c,(int)base);
		}
	}
	else x=stk[0];
push (); stk[0]=x; entry[0]='\0';
}
/* query..----------------------------------------------------------------- */

/* query the user for an input */

dbl query (char *prompt, dbl def)

{
dbl x;

_settextposition (24,1);
vin (prompt,&x,"lf",def,"");
ptext ("                                                     ",24,1,14,2);
return x;
}
/* chs..------------------------------------------------------------------- */

/* change sign of entry */

char chs (void)

{
char buf[40];

if (strlen(entry))
	{
	strcpy (buf,entry);
	if (buf[0] == '-')
	{strcpy (entry,"+"); strcat (entry,&buf[1]); return 1;}
	if (buf[0] == '+')
	{strcpy (entry,"-"); strcat (entry,&buf[1]); return 1;}
	strcpy (entry,"-"); strcat (entry,buf); return 1;
	}
return 0;
}
/* ccat..------------------------------------------------------------------ */

/* concatenate a single character to a string */

char *ccat (char *str, char c)

{
int k;

k=strlen (str);
str[k]=c; str[k+1]='\0';
return str;
}
/* tconv..----------------------------------------------------------------- */

/* temperature conversions */

dbl tconv (void)

{
dbl x,c,c0=273.16,r0;

r0=9.*c0/5.-32.;
x=stk[0];
switch (tfrom)
{
case 0:		/* F */
	if (x < -r0) {beep; return x;} else c=5.*(x-32.)/9.;
break;
case 1:		/* C */
	if (x < -c0) {beep; return x;} else c=x;
break;
case 2:		/* R */
	if (x < 0.) {beep; return x;} else c=5.*((x-r0)-32.)/9.;
break;
case 3:		/* K */
	if (x < 0.) {beep; return x;} else c=x-c0;
break;
}

switch (tto)
{
case 0:		/* F */
	x=9.*c/5.+32.;
break;
case 1:		/* C */
	x=c;
break;
case 2:		/* R */
	x=9.*c/5.+32.+r0;
break;
case 3:		/* K */
	x=c+c0;
break;
}
return x;
}
/* wconv..----------------------------------------------------------------- */

/* weight conversion */

dbl wconv (void)

{
dbl x,kg,q=2.20462;

x=stk[0];
switch (wfrom)
{
case 0:		/* oz */
	kg=x/(16.*q);
break;
case 1:		/* lb */
	kg=x/q;
break;
case 2:		/* gm */
	kg=x*1.0e-3;
break;
case 3:		/* kg */
	kg=x;
break;
}
switch (wto)
{
case 0:		/* oz */
	x=16.*q*kg;
break;
case 1:		/* lb */
	x=kg*q;
break;
case 2:		/* gm */
	x=kg*1000.;
break;
case 3:		/* kg */
	x=kg;
break;
}
return x;
}
/* lconv..----------------------------------------------------------------- */

/* length conversion */

dbl lconv (void)

{
dbl x,m,q=0.0254;

x=stk[0];
switch (lfrom)
{
case 0:		/* in */
	m=x*q;
break;
case 1:		/* ft */
	m=x*12.*q;
break;
case 2:		/* yd */
	m=x*36.*q;
break;
case 3:		/* mi */
	m=x*12.*5280.*q;
break;
case 4:		/* mm */
	m=x*1.e-3;
break;
case 5:		/* cm */
	m=x*1.e-2;
break;
case 6:		/* m */
	m=x;
break;
case 7:		/* km */
	m=x*1000.;
break;
}
switch (lto)
{
case 0:		/* in */
	x=m/q;
break;
case 1:		/* ft */
	x=m/(12.*q);
break;
case 2:		/* yd */
	x=m/(36.*q);
break;
case 3:		/* mi */
	x=m/(12.*5280.*q);
break;
case 4:		/* mm */
	x=m*1000.;
break;
case 5:		/* cm */
	x=m*100.;
break;
case 6:		/* m */
	x=m;
break;
case 7:		/* km */
	x=m*1.e-3;
break;
}
return x;
}
/* save..------------------------------------------------------------------ */

void save (void)
{
int i;

for (i=0 ; i<NS ; i++) undo[i]=stk[i]; undo[NS]=lastx; undo[NS+1]=mem;
lastx=stk[0];
}
/* gcd..------------------------------------------------------------------- */

/* return greatest common divisor */

dbl gcd (dbl x, dbl y)

{
dbl a=x,b=y,c;
if (a == 0 && b == 0) return 1.;
if (a > b) swap (&a,&b,sizeof(a));
do {c=b-a*floor(b/a); b=a; a=c;} while (c != 0); return b;
}
/* kbranch..--------------------------------------------------------------- */

/* keyboard branching */

char kbranch (void)

{
char c;
dbl t;

c=toupper((char)(key & 0x00ff));

switch (base)
{
case 16:		/* hex */
	if (isxdigit(c)) {ccat (entry,c); goto done;}
	break;

case 10:		/* decimal */
	if (isdigit(c) || c == '.' || c == 'E' || c == '-')
	{
	if (c == '-')
		{
		/* exponent sign */
		if (entry[strlen(entry)-1] != 'E') goto chk;
		}
	ccat (entry,c); goto done;
	}
	break;

case 8:			/* octal */
	if (strrchr ("01234567",c)) {ccat (entry,c); goto done;}
	break;

case 2:			/* binary */
	if (strrchr ("01",c)) {ccat (entry,c); goto done;}
	break;
}

chk:
switch (c)
	{
	case '=':		/* leading minus sign for entry */
		if (!strlen(entry)) ccat (entry,'-'); else chs ();
		goto done;
	case '+':		/* + */
		mo.row=2; mo.col=1; return mbranch ();
	case '-':		/* - */
		mo.row=2; mo.col=9; return mbranch ();
	case '*':		/* * */
		mo.row=2; mo.col=17; return mbranch ();
	case '/':		/* / */
		mo.row=2; mo.col=25; return mbranch ();
	case 'L':		/* last x */
		mo.row=3; mo.col=9; return mbranch ();
	case 'I':		/* 1/x */
		mo.row=3; mo.col=17; return mbranch ();
	case 39 :		/* chs */
		mo.row=3; mo.col=25; return mbranch ();
	case 'S':		/* clear stack */
		mo.row=20; mo.col=9; return mbranch ();
	case 'N':		/* notes */
		mo.row=21; mo.col=9; return mbranch ();
	case 'Q':
		return 1;
	case 'U':
		mo.row=21; mo.col=33; return mbranch ();
	case 'X':
		mo.row=20; mo.col=1; return mbranch ();
	case 'P':
		mo.row=4; mo.col=1; return mbranch ();
	case 'Z':
		dflag=dflag^1;
		goto done;
	default:
		break;
	}

switch (key)
{
case esc:			/* quit */
case space:
	return 1;

case up:			/* roll up */
	rollup ();
	break;

case down:			/* roll down */
	rolldn ();
	break;

case right:			/* decimals++ */
	if (dflag) {dflag=0; break;}
	if (decimals < 15) decimals++;
	break;

case left:			/* decimals-- */
	if (dflag) {dflag=0; break;}
	if (decimals > 0) decimals--;
	break;

case home:			/* decimals = 2 */
	decimals=2;
	break;

case end:			/* x <> y */
	mo.row=1; mo.col=17; return mbranch ();

case enter:			/* enter */
	entr ();
	break;

case bks:			/* backspace */
	if (strlen (entry)) entry[strlen (entry)-1]='\0';
	break;

case del:
	entry[0]='\0';
	break;

default:
	beep;
	break;
}
done: display (); return 0;
}
/* menu..------------------------------------------------------------------ */

void menu (void)

{
int r=1,c=1,f=14,b=2;

/*           1       9       17      25      33 */
sprintf (gp,"roll.. roll.. xy.end xm.... .......");
ptext (gp,r++,c,f,b);					/* 1 */
sprintf (gp,"y+x.... y-x.... y*x.... y/x.... x/y....");
ptext (gp,r++,c,f,b);					/* 2 */
sprintf (gp,"e...... Lastx.. 1/x...I chs...' .......");
ptext (gp,r++,c,f,b);					/* 3 */
sprintf (gp,"ã.....P xý..... û...... x!..... .......");
ptext (gp,r++,c,f,b);					/* 4 */
sprintf (gp,"store.. store+. store-. store*. store/.");
ptext (gp,r++,c,f,b);					/* 5 */
sprintf (gp,"rcall.. rcall+. rcall-. rcall*. rcall/.");
ptext (gp,r++,c,f,b);					/* 6 */
sprintf (gp,"sin.... cos.... tan.... rss.... xy¯ré..");
ptext (gp,r++,c,f,b);					/* 7 */
sprintf (gp,"asin... acos... atan... unrss.. ré¯xy..");
ptext (gp,r++,c,f,b);					/* 8 */
sprintf (gp,"ø¯rad.. rad¯ø.. atan2.. ....... .......");
ptext (gp,r++,c,f,b);					/* 9 */
sprintf (gp,"frac... int.... split.. ymodx.. .......");
ptext (gp,r++,c,f,b);					/* 10 */
sprintf (gp,"floor.. ceil... round.. gcd.... lcm....");
ptext (gp,r++,c,f,b);					/* 11 */
sprintf (gp,"e^x.... 10^x... 2^x.... y^x.... y^(1/x)");
ptext (gp,r++,c,f,b);					/* 12 */
sprintf (gp,"ln..... log.... log2... ylogx.. .......");
ptext (gp,r++,c,f,b);					/* 13 */
sprintf (gp,"and.... or..... 1comp.. xor.... .......");
ptext (gp,r++,c,f,b);					/* 14 */
sprintf (gp,"øF¯øC.. oz¯lb.. in¯mm.. ....... .......");
ptext (gp,r++,c,f,b);					/* 15 */
sprintf (gp,"....... ....... ....... ....... .......");
ptext (gp,r++,c,f,b);					/* 16 */
sprintf (gp,"....... ....... ....... ....... .......");
ptext (gp,r++,c,f,b);					/* 17 */
sprintf (gp,"FIX.... ENG.... SCI.... .dd.... ADJ...Z");
ptext (gp,r++,c,f,b);					/* 18 */
sprintf (gp,"DEC.... HEX.... BIN.... OCT.... DEG....");
ptext (gp,r++,c,f,b);					/* 19 */
sprintf (gp,"clearX. clrStk. clrmem. clrall. .......");
ptext (gp,r++,c,f,b);					/* 20 */
sprintf (gp,"Quit... Notes.. swap... swapx.. Undo...");
ptext (gp,r++,c,f,b);					/* 21 */
}
/* mbranch..--------------------------------------------------------------- */

char mbranch (void)

{
char buf[40],b2[40],*l;
int i,r,c;
dbl t,ip,fp,xl,yl;

r=mo.row; c=1+(mo.col-1)/8;

if (r == 15)		/* set temperature,weight,length scales */
{
if (mo.col == 2) {tfrom=((++tfrom) % 4); goto mdone;}
if (mo.col == 5) {tto=((++tto) % 4); goto mdone;}
if (mo.col == 9 || mo.col == 10) {wfrom=((++wfrom) % 4); goto mdone;}
if (mo.col == 12 || mo.col == 13) {wto=((++wto) % 4); goto mdone;}
if (mo.col == 17 || mo.col == 18) {lfrom=((++lfrom) % 8); goto mdone;}
if (mo.col == 20 || mo.col == 21) {lto=((++lto) % 8); goto mdone;}
}

if (mo.col > 40)	/* swap stack elements with x */
{
switch (r)
{
case 1:
	swap (&mem,&stk[0],sizeof(stk[0])); goto mdone;
case 2:
	swap (&lastx,&stk[0],sizeof(stk[0])); goto mdone;
case 4:
	swap (&stk[3],&stk[0],sizeof(stk[0])); goto mdone;
case 5:
	swap (&stk[2],&stk[0],sizeof(stk[0])); goto mdone;
case 6:
	swap (&stk[1],&stk[0],sizeof(stk[0])); goto mdone;
case 10:
	entr (); goto mdone;
default:
	goto mdone;
}
}

chk ();
switch (r)
{
case 1:
	switch (c)
	{
	case 1:		/* roll up */
		rollup ();
		break;
	case 2:		/* roll down */
		rolldn ();
		break;
	case 3:		/* x<>y */
		swap (&stk[0],&stk[1],sizeof(stk[0]));
		break;
	case 4:		/* x<>mem */
		swap (&stk[0],&mem,sizeof(stk[0]));
		break;
	case 5:
		bbr
	}
break;
case 2:
	if (c == 4 && stk[0] == 0.) bbr
	if (c == 5 && stk[1] == 0.) bbr
	save ();
	switch (c)
	{
	case 1:			/* + */
		t=stk[1]+stk[0]; pop (); stk[0]=t;
		break;
	case 2:			/* - */
		t=stk[1]-stk[0]; pop (); stk[0]=t;
		break;
	case 3:			/* * */
		t=stk[1]*stk[0]; pop (); stk[0]=t;
		break;
	case 4:			/* / */
		t=stk[1]/stk[0]; pop (); stk[0]=t;
		break;
	case 5:			/* x/y */
		t=stk[0]/stk[1]; pop (); stk[0]=t;
		break;
	}
break;
case 3:
	switch (c)
	{
	case 1:			/* e */
		save ();
		push (); stk[0]=exp(1.);
		break;
	case 2:			/* last x */
		push (); stk[0]=lastx;
		break;
	case 3:			/* 1/x */
		if (stk[0] == 0.) bbr
		save ();
		stk[0]=1./stk[0];
		break;
	case 4:			/* chs */
		if (!chs ()) stk[0]=-stk[0];
		break;
	case 5:
		bbr
	}
break;
case 4:
	switch (c)
	{
	case 1:			/* pi */
		save (); push (); stk[0]=pi;
		break;
	case 2:			/* x^2 */
		save ();
		stk[0]=undo[0]*undo[0];
		break;
	case 3:			/* sqrt(x) */
		if (stk[0] < 0.) bbr
		save (); stk[0]=sqrt(undo[0]);
		break;
	case 4:			/* x! */
		fp=modf(fabs(stk[0]),&ip);
		if (fp != 0. || stk[0] < 0.) bbr
		if (ip > 170) bbr	/* 170! ~= 7E+306 */
		for (i=1,t=1. ; i<=(int)ip ; i++) t*=(dbl)i;
		save ();
		stk[0]=t;
		break;
	case 5:
		bbr
	}
break;
case 5:
	switch (c)
	{
	case 1:			/* store */
		mem=stk[0];
		break;
	case 2:			/* store + */
		mem+=stk[0];
		break;
	case 3:			/* store - */
		mem-=stk[0];
		break;
	case 4:			/* store * */
		mem*=stk[0];
		break;
	case 5:			/* store / */
		if (stk[0] == 0.) bbr
		mem/=stk[0];
		break;
	}
break;
case 6:
	save ();
	switch (c)
	{
	case 1:			/* recall */
		push ();
		stk[0]=mem;
		break;
	case 2:			/* recall + */
		stk[0]+=mem;
		break;
	case 3:			/* recall - */
		stk[0]-=mem;
		break;
	case 4:			/* recall * */
		stk[0]*=mem;
		break;
	case 5:			/* recall / */
		if (mem == 0.) bbr
		stk[0]/=mem;
		break;
	}
break;
case 7:
	t=(amode) ? 1. : rpd;
	save ();
	switch (c)
	{
	case 1:			/* sin */
		stk[0]=sin(stk[0]*t);
		break;
	case 2:			/* cos */
		stk[0]=cos(stk[0]*t);
		break;
	case 3:			/* tan */
		stk[0]=tan(stk[0]*t);
		break;
	case 4:			/* rss */
		t=sqrt(stk[0]*stk[0]+stk[1]*stk[1]); pop (); stk[0]=t;
		break;
	case 5:			/* rect->polar */
		stk[0]=hypot(undo[0],undo[1]);
		if (undo[0] == 0. && undo[1] == 0.) stk[1]=0.;
		  else stk[1]=atan2(undo[1],undo[0])/t;
		break;
	}
break;
case 8:
	t=(amode) ? 1. : dpr;
	save ();
	switch (c)
	{
	case 1:			/* asin */
		if (fabs(stk[0]) > 1.) bbr
		stk[0]=t*asin(stk[0]);
		break;
	case 2:			/* acos */
		if (fabs(stk[0]) > 1.) bbr
		stk[0]=t*acos(stk[0]);
		break;
	case 3:			/* atan */
		stk[0]=t*atan(stk[0]);
		break;
	case 4:			/* unrss */
		t=(stk[1]*stk[1]-stk[0]*stk[0]); if (t < 0.) t=-t;
		pop (); stk[0]=sqrt(t);
		break;
	case 5:			/* polar->rect */
		stk[0]=undo[0]*cos(undo[1]/t);
		stk[1]=undo[0]*sin(undo[1]/t);
		break;
	}
break;
case 9:
	switch (c)
	{
	case 1:			/* deg => rad */
		save ();
		stk[0]*=rpd;
		break;
	case 2:			/* rad => deg */
		save ();
		stk[0]*=dpr;
		break;
	case 3:			/* atan2 */
		if (stk[0] == 0. && stk[1] == 0.) t=0.;
		  else t=atan2(stk[1],stk[0]);
		if (!amode) t*=dpr;
		save (); pop (); stk[0]=t;
		break;
	case 4:
		bbr
	case 5:
		bbr
	}
break;
case 10:
	fp=modf(stk[0],&ip);
	save ();
	switch (c)
	{
	case 1:			/* frac */
		stk[0]=fp;
		break;
	case 2:			/* int */
		stk[0]=ip;
		break;
	case 3:			/* split */
		push (); stk[0]=fp; stk[1]=ip;
		break;
	case 4:			/* ymodx */
		t=fmod(stk[1],stk[0]); stk[0]=t;
		break;
	case 5:
		bbr
	}
break;
case 11:
	save ();
	switch (c)
	{
	case 1:			/* floor */
		stk[0]=floor(stk[0]);
		break;
	case 2:			/* ceiling */
		stk[0]=ceil(stk[0]);
		break;
	case 3:			/* round */
		strcpy (buf,"%.");
		strcat (buf,itoa(decimals,b2,10)); strcat (buf,"lf");
		sprintf (gp,buf,stk[0]);
		l=strchr(gp,'\0')-1; stk[0]=strtod(gp,&l);
		break;
	case 4:			/* gcd */
		fp=modf(fabs(stk[0]),&xl);
		if (fp || xl == 0) bbr
		fp=modf(fabs(stk[1]),&yl);
		if (fp || yl == 0) bbr
		pop (); stk[0]=gcd(xl,yl);
		break;
	case 5:			/* lcm */
		fp=modf(fabs(stk[0]),&xl);
		if (fp || xl == 0) bbr
		fp=modf(fabs(stk[1]),&yl);
		if (fp || yl == 0) bbr
		pop (); stk[0]=xl*yl/gcd(xl,yl);
		break;
	}
break;
case 12:
	t=log(10.);
	switch (c)
	{
	case 1:			/* e^x */
		if (fabs(stk[0]) > 300.*t) bbr
		save (); stk[0]=exp(stk[0]);
		break;
	case 2:			/* 10^x */
		if (fabs(stk[0]) > 300.) bbr
		save (); stk[0]=pow(10.,stk[0]);
		break;
	case 3:			/* 2^x */
		if (fabs(stk[0]) > 300.*t/log(2.)) bbr
		save (); stk[0]=pow(2.,stk[0]);
		break;
	case 4:			/* y^x */
		if (stk[1] <= 0.) bbr
		if (fabs(stk[0]) > 300.*t/log(stk[1])) bbr
		save ();
		t=pow(stk[1],stk[0]); pop (); stk[0]=t;
		break;
	case 5:			/* y^(1/x) */
		if (stk[0] == 0. || stk[1] <= 0.) bbr
		if (fabs(1./stk[0]) > 300.*t/log(stk[1])) bbr
		save ();
		t=pow(stk[1],1./stk[0]); pop (); stk[0]=t;
		break;
	}
break;
case 13:
	if (stk[0] <= 0.) bbr
	if (c == 4 && (t=stk[1]) <= 0.) bbr
	save ();
	switch (c)
	{
	case 1:			/* ln */
		stk[0]=log(stk[0]);
		break;
	case 2:			/* log10 */
		stk[0]=log10(stk[0]);
		break;
	case 3:			/* log2 */
		stk[0]=log(stk[0])/log(2.);
		break;
	case 4:			/* ylogx */
		if (log(lastx) == 0.) bbr
		pop (); stk[0]=log(t)/log(lastx);
		break;
	case 5:
		bbr
	}
break;
case 14:
	if (fabs(stk[0]) > 0xffffffff) bbr
	save ();
	switch (c)
	{
	case 1:			/* and */
		if (fabs(stk[1]) > 0xffffffff) bbr
		xl=(long)stk[0] & (long)stk[1]; pop (); stk[0]=(dbl)xl;
		break;
	case 2:			/* or */
		if (fabs(stk[1]) > 0xffffffff) bbr
		xl=(long)stk[0] | (long)stk[1]; pop (); stk[0]=(dbl)xl;
		break;
	case 3:			/* one's complement */
		xl=~(long)stk[0]; stk[0]=(dbl)xl;
		break;
	case 4:			/* xor */
		if (fabs(stk[1]) > 0xffffffff) bbr
		xl=(long)stk[0] ^ (long)stk[1]; pop (); stk[0]=(dbl)xl;
		break;
	case 5:
		bbr
	}
break;
case 15:
	save ();
	switch (c)
	{
	case 1:			/* temperature conversion */
		stk[0]=tconv ();
		break;
	case 2:			/* weight conversion */
		stk[0]=wconv ();
		break;
	case 3:			/* length conversion */
		stk[0]=lconv ();
		break;
	case 4:
		bbr
	case 5:
		bbr
	}
break;
case 16:
	switch (c)
	{
	case 1:
		bbr
	case 2:
		bbr
	case 3:
		bbr
	case 4:
		bbr
	case 5:
		bbr
	}
break;
case 17:
	switch (c)
	{
	case 1:
		bbr
	case 2:
		bbr
	case 3:
		bbr
	case 4:
		bbr
	case 5:
		bbr
	}
break;
case 18:
	switch (c)
	{
	case 1:			/* fix */
		dtype=0;
		break;
	case 2:			/* engineering */
		dtype=1;
		break;
	case 3:			/* scientific */
		dtype=2;
		break;
	case 4:			/* number of decimals */
		dflag=0;
		decimals=(int)query ("Number of decimal places (15 max)",2.);
		if (decimals > 15) decimals=15;
		break;
	case 5:			/* self adjusting format */
		dflag=1;
		break;
	}
break;
case 19:
	switch (c)
	{
	case 1:			/* decimal */
		base=10;
		break;
	case 2:			/* hex */
		base=16;
		break;
	case 3:			/* binary */
		base=2;
		break;
	case 4:			/* octal */
		base=8;
		break;
	case 5:			/* toggle DEG/RAD */
		amode=amode^1;
		break;
	}
break;
case 20:
	t=lastx; save (); lastx=t;
	switch (c)
	{
	case 1:			/* clear x */
		stk[0]=0.;
		break;
	case 2:			/* clear stack */
		clear (1);
		break;
	case 3:			/* clear memory */
		mem=0.;
		break;
	case 4:			/* clear all */
		clear (0);
		break;
	case 5:
		bbr
	}
break;
case 21:
	switch (c)
	{
	case 1:			/* quit */
		return 1;
		break;
	case 2:			/* help */
		notes ();
		break;
	case 3:			/* swap */
		for (i=0 ; i<NS ; i++) swap (&stk[i],&swp[i],sizeof(stk[i]));
		swap (&lastx,&swp[NS],sizeof(lastx));
		swap (&mem,&swp[NS+1],sizeof(mem));
		break;
	case 4:			/* swapx */
		swap (&stk[0],&swp[0],sizeof(stk[0]));
		break;
	case 5:			/* undo */
		for (i=0 ; i<NS ; i++) stk[i]=undo[i];
		lastx=undo[NS]; mem=undo[NS+1];
		break;
	}
break;
default:
break;
}
mdone:
display ();
return 0;
}
/* notes..----------------------------------------------------------------- */

void notes (void)

{
mouse (2,0,0,0);
cls;
puts ("Computational accuracy ÷ 15 significant digits.");
puts ("Usable range ÷ 1E+/-300.");
puts ("Hex values limited to FFFFFFFF (integer values only).");
puts ("If an entry is pending, any legitimate operation will enter it into the stack");
puts (" before the operation is performed.");
puts ("ADJ sets the number of decimal places to display each number with");
puts (" a precision of 1E-15.");
puts ("Any number that would display as 0.0... in FIX mode is displayed with");
puts (" at least one significant digit, regardless of the .dd setting.");
puts ("Numbers with absolute value outside the range 1E+/-15 are automatically");
puts (" displayed in engineering/scientific notation if FIX is active.");
puts ("Clicking on any stack element will swap it with the x element.");
puts (" ");
puts ("Exit   Q/Quit on menu or spacebar or Esc or both mouse buttons");
puts ("del    delete any pending entry");
puts ("®/¯    decrease/increase decimal places displayed");
puts ("Home   restore to 2 decimal place default");
puts ("Z      (Zoom) toggle between ADJ and .dd mode");
puts ("=      unary minus for negative entries (or chs if pending entry)");
puts ("swap   swap calculators (to make auxiliary calculations)");
puts ("round  round x to number of places currently displayed in FIX mode");
puts ("       use floor/ceil to truncate +/- numbers to an integer");
puts (" ");
pausem;
cls;
menu ();
display ();
mouse (1,0,0,0);
}
/* ------------------------------------------------------------------------ */
