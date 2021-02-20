/* UNIT.C   M. W. Klotz   1-2/98, 4,7,12/03

Units conversion program

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
#define beep tone(440,100)
#define bpause beep; pause
#define bpausem beep; pausem
#define quit beep; fcloseall(); exit(1)
#define print printf("\n")
#define FALSE 0
#define TRUE !FALSE
#define cls _clearscreen(0);

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
#include <constant.h>

/* -------------------------- function prototypes ------------------------- */

/* Minimize the number of these included since they contribute to exectuable
size. */

/* global utility functions */
/* note: no space between function name and parameter list */
#include <util.h>

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
void readdata (void);
void menu (void);
void sort (void);
int acomp (const void *v1,const void *v2);
int ufind (char *un);
void pprim (char *u[], char *p);
void convert (int n);
real uconv (real x);
int uanal (char *ex, struct unitexp *p);
void deplural (char *n);
char *trim (char *s);
dbl fact (char *s);
void define (void);
void engprnt (real x);
void compound (void);

/* --------------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* struc..----------- global structure declarations ----------------------- */
#define NPREF 25	/* maximum number of prefixes */
#define NPRIM 7		/* number of primary units */
#define NU 500		/* maximum number of units */
#define NSL 15		/* maximum length of a unit name */
#define NPR 12		/* maximum length of a prefix name */

struct prefix
{
char name [NPR];
real val;
};
struct prefix pref[NPREF];

struct units
{
char name [NSL];
dbl fact;
char pfact [NPRIM];
};
struct units unit[NU];

struct unitexp
{
char np [NPR];		/* numerator prefix */
int npk;		/* numerator prefix index */
char nu [NSL];		/* numerator unit */
int nuk;		/* numerator unit index */
int ne;			/* numerator exponent */
char den;		/* =1 if denominator present in unit expression */
char dp [NPR];		/* denominator prefix */
int dpk;		/* denominator prefix index */
char du [NSL];		/* denominator unit */
int duk;		/* denominator unit index */
int de;			/* denominator exponent */
char pu [NPRIM];	/* used to determine unit compatibility */
};
struct unitexp from,to;

/* vmark..------------ global variable declarations ----------------------- */
#define NCPL 81			/* characters per line */
int npref=0;			/* number of prefixes */
int nu=0;			/* number of units */
char nlfile[]="UNIT.DAT";
/* names of primary units */
char *primary [NPRIM]=
{
"LENGTH","MASS","TIME","ANGLE","SOLIDANGLE","CHARGE","AMOUNT"
};
char *uprimary [NPRIM]=
{
"METER","KILOGRAM","SEC","RADIAN","STERADIAN","COULOMB","ND"
};
int alpha [NU];			/* used for alphabetizing units */
real yprimary;			/* converted quantity in primary units */

/* main..====xxxxxxxx====================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i,j,k,m,n;
char op,un[NSL],opp;

/* -------------------------- initializations ------------------------------ */
textscreen ();
kbdset (0xbf);		/* lower case */
/* ------------------------------------------------------------------------- */

readdata();			/* read the unit data file */
sort();				/* sort the unit pointers */
menu();				/* display the menu */
agin: op=getkey("?");

switch (op)
{
	case 'a':
		compound ();
		break;
	case 'b':	/* breakdown of unit */
		printf ("%s? ","UNIT NAME "); gets(un);
		if ((k=ufind(un)) == -1)
		  {beep; puts ("NOT A PRIMARY UNIT"); break;}
		printf ("%-s = ",unit[k].name);
		pprim (primary,&unit[k].pfact[0]); print;
		printf (" = %G ",1./unit[k].fact);
		pprim (uprimary,&unit[k].pfact[0]); print;
		break;
	case 'c':	/* display compatible units */
		printf ("%s? ","PRIMARY UNIT NAME"); gets(un);
		if ((k=ufind(un)) == -1)
		  {beep; puts ("UNIT NOT FOUND"); break;}
		for (i=0 ; i<nu ; i++)
		{
		n=1;
		for (j=0 ; j<NPRIM ; j++)
		  {if (unit[k].pfact[j] != unit[alpha[i]].pfact[j]) n=0;}
		if(n) printf ("%-15s ",unit[alpha[i]].name);
		}
		print;
		break;
	case 'd':	/* define a new unit */
		define();
		break;
	case 'f':	/* display fundamental units */
		for (i=0 ; i<NPRIM ; i++)
		  {printf ("%-15s %-15s\n",primary[i],uprimary[i]);}
		break;
	case 'h':	/* display menu */
		menu();
		break;
	case 'i':	/* display units starting with letter */
		opp=getkey("INITIAL LETTER ?"); opp=toupper(opp);
		for (i=0 ; i<nu ; i++)
		{
		if (unit[alpha[i]].name[0] == opp)
		  printf ("%-15s ",unit[alpha[i]].name);
		}
		print;
		break;
	case ' ':
	case 'j':	/* convert units */
		convert(0);
		break;
	case 'l':	/* repeat last conversion */
		convert(1);
		break;
	case 'p':	/* display prefixes */
		for (i=0 ; i<npref ; i++)
		  {
		  printf ("%-12s%-5.0E\n",pref[i].name,pref[i].val);
		  if (i > 0 && i % 20 == 0) pausem;
		  }
		break;
	case 'u':	/* display units */
		for (i=0 ; i<nu ; i++)
		{
		printf ("%-15s ",unit[alpha[i]].name);
		if (i > 0 && i % 99 == 0) getkey("\npress a key\n");
		}
		print;
		break;
	case 'q':	/* quit */
	case 27:
		goto mexit;
	default:
		beep; puts ("NOT A VALID OPTION");
		break;

}
goto agin;

/* post run processing */
mexit:
fcloseall();
exit(0);			/* normal exit */
}
/* ======================================================================== */
/* readdata..-------------------------------------------------------------- */

/* read the unit data file and store the values read */

void readdata (void)

{
int i;
char token[]="=;,";
int dtype=-1;			/* -1=undefined 0=prefixes, 1=unit data */
char *line,*name,*value,temp[NCPL];
char pu[NPRIM];
FILE *fp;

if ((fp=fopen(nlfile,"rt")) == NULL)
	{printf ("FAILED TO OPEN UNIT DATA FILE: %s",nlfile); quit;}

while ((fgets(temp,sizeof(temp),fp)) != NULL)
	{
	line=trim(temp);
	if ((strstr(line,"ENDOFDATA")) != NULL) break;
	if (line[0] == '\0' || line[0] == ';') continue;
	if (strstr(line,"BEGINPREFIX")) {dtype=0; continue;}
	if ((strstr(line,"BEGINPRIMARY")) != NULL) {dtype=1; continue;}
	if ((strstr(line,"BEGINMIXED")) != NULL) {dtype=2; continue;}


	switch (dtype)
		{
		case 0:		/* prefix processing */
			/* must be an assignment statement */
			if ((strchr(line,'=')) != NULL)
			{
			strcpy(pref[npref].name,trim(strtok(temp,token)));
			value=trim(strtok(NULL,token));
			pref[npref].val=(real) atof(value);
			npref++;
			}
			break;
		case 1:		/* primary unit data processing */
			if ((strstr(line,"NEWUNIT")) != NULL)
			{
			value=trim(strtok(temp,token)); i=0;
			while (value=trim(strtok(NULL,token)))
				pu[i++]=(int)atoi(value);
			}
			else
			{
			strcpy (unit[nu].name,trim(strtok(temp,token)));
			value=trim(strtok(NULL,token));
			unit[nu].fact=fact(value);
			for (i=0 ; i<NPRIM ; i++) unit[nu].pfact[i]=pu[i];
			nu++;
			if (nu >= NU) {puts ("OUT OF SPACE"); quit;}
			}
			break;
		case 2:		/* mixed (compound) unit data processing */
			strcpy (unit[nu].name,trim(strtok(temp,token)));
			value=trim(strtok(NULL,token));
			unit[nu].fact=fact(value); i=0;
			while (value=trim(strtok(NULL,token)))
				unit[nu].pfact[i++]=(int)atoi(value);
			nu++;
			if (nu >= NU) {puts ("OUT OF SPACE"); quit;}
			break;

		default:
			printf ("dtype error in readdata %i\n",dtype);
			quit;
			
		}		/* end switch on dtype */
	}		/* end while on file read */
fclose (fp);
printf ("%s = %i\n","NUMBER OF FUNDAMENTAL UNITS",NPRIM);
printf ("%s = %i\n","NUMBER OF PREFIXES",npref);
printf ("%s = %i\n","NUMBER OF UNITS",nu);
}

/* menu..------------------------------------------------------------------ */

void menu (void)

{
puts ("Esc/Q   Quit");
puts ("A       display Available units of a given type");
puts ("B       display Breakdown of a unit into fundamental units");
puts ("C       display units Compatible with input unit");
puts ("D       Define a new unit (not saved)");
puts ("F       display Fundamental units");
puts ("H       Help (display this list of options)");
puts ("I       display units with input Initial letter");
puts ("J/space perform a unit conversion");
puts ("L       redo Last conversion with different destination units");
puts ("P       display list of Prefixes");
puts ("U       display list of allowable Units");
}

/* sort..------------------------------------------------------------------ */

/* sort the unit names alphabetically by sorting the unit structure indices
   into alpha[]
*/

void sort (void)

{
int i,t;
char flag;

for (i=0 ; i<nu ; i++) alpha[i]=i;
do
{
	for (i=0,flag=0 ; i<nu-1 ; i++)
	{
	if (acomp (unit[alpha[i]].name,unit[alpha[i+1]].name) > 0)
		{t=alpha[i]; alpha[i]=alpha[i+1]; alpha[i+1]=t; flag=1;}
	}
} while (flag);
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

/* ufind..----------------------------------------------------------------- */

/* find the index corresponding to a unit name string */

int ufind (char *un)

{
int i;

for (i=0 ; i<nu ; i++)
	{if (strcmpi(un,unit[i].name) == 0) return i;}
return -1;
}

/* pprim..----------------------------------------------------------------- */

/* print a primary unit breakdown */

void pprim (char *u[], char *p)

{
int i;
char m=0,n=0,e;

printf (" %-s","(");
for (i=0 ; i<NPRIM ; i++)
	{
	e=p[i];
	if (e == 0) continue;
	if (e < 0) {m=1; continue;}
	if (n) printf ("%-s"," * ");
	if (e == 1) {printf ("%-s",u[i]); n=1;}
	  else {printf ("%-s^%-i",u[i],e); n=1;}
	}
printf ("%-s",")");
if (!m) return;
printf ("%-s"," / ("); n=0;
for (i=0 ; i<NPRIM ; i++)
	{
	e=p[i];
	if (e >= 0) continue;
	if (n) printf ("%-s"," * ");
	if (e == -1) {printf ("%-s",u[i]); n=1;}
	  else {printf ("%-s^%-i",u[i],-e); n=1;}
	}
printf ("%-s",")");
}

/* uanal..----------------------------------------------------------------- */

/* decompose a unit expression */

int uanal (char *ex, struct unitexp *p)

{
int i;
char e[40];
char n[40]="\0";	/* numerator portion of ex temp storage */
char d[40]="\0";	/* denominator portion of ex temp storage */
char *tp;		/* temporary string pointer */

	/* decompose into numerator and denominator strings */

strcpy (e,ex);
if (tp=strstr(e,"/"))
	{strcpy(n,trim(strtok(e,"/"))); strcpy(d,trim(tp+1)); p->den=1;}
else
	{strcpy(n,trim(e)); p->den=0;}

if (n[0] == '\0') goto eexit;	/* no unit specified */

			/* extract exponents */

p->ne=1; p->de=1;	/* initialize to default value */
if (tp=strstr(n,"^")) {strcpy (n,strtok(n,"^")); p->ne=atoi(tp+1);}

if (p->den)
{
if (tp=strstr(d,"^")) {strcpy (d,strtok(d,"^")); p->de=atoi(tp+1);}
}

/* remove numerator pluralizations */
deplural (n);

/* remove denominator pluralizations */
if (p->den) deplural (d);

			/* extract prefixes */

p->npk=-1; p->dpk=-1;		/* initialize to impossible value */
p->np[0]='\0'; p->dp[0]='\0';

if (ufind(n) == -1)	/* unit name may contain embedded prefix name */
{
for (i=0 ; i<npref ; i++)
	{
	if ((tp=strstr(n,pref[i].name)))
		{
		strcpy(p->np,pref[i].name); p->npk=i;
		strcpy(n,tp+strlen(pref[i].name));
		break;
		}
	}
}
if (p->den && (ufind(d) == -1))
{
for (i=0 ; i<npref ; i++)
	{
	if (tp=strstr(d,pref[i].name))
		{
		strcpy(p->dp,pref[i].name); p->dpk=i;
		strcpy(d,tp+strlen(pref[i].name));
		break;
		}
	}
}

		/* extract units */

p->nuk=-1; p->duk=-1;		/* initialize */
p->nu[0]='\0'; p->du[0]='\0';

/* find numerator unit and index thereof */
if ((i=ufind(n)) == -1) goto eexit;
strcpy(p->nu,unit[i].name); p->nuk=i;

/* find denominator unit and index thereof */
if (p->den)
{
if ((i=ufind(d)) == -1) goto eexit;
strcpy(p->du,unit[i].name); p->duk=i;
}

/* create numbers for checking unit compatibility */
for (i=0 ; i<NPRIM ; i++) p->pu[i]=0;	/* initialize */
for (i=0 ; i<NPRIM ; i++)
{
p->pu[i]+=unit[p->nuk].pfact[i]*(p->ne);
if (p->den) p->pu[i]-=unit[p->duk].pfact[i]*(p->de);
}

#if 0 == 1
/* debug print */
printf ("n = %s\n",n);
printf ("np,npk = %s %i\n",p->np,p->npk);
printf ("nu,nuk = %s %i\n",p->nu,p->nuk);
printf ("ne = %i\n",p->ne);
printf ("den = %i\n",p->den);
printf ("d = %s\n",d);
printf ("dp,dpk = %s %i\n",p->dp,p->dpk);
printf ("du,duk = %s %i\n",p->du,p->duk);
printf ("de = %i\n",p->de);
printf ("pu ="); for (i=0 ; i<NPRIM ; i++) printf ("%i,",p->pu[i]); print;
#endif

return 1;	/* normal exit */
eexit: return 0;	/* error exit */

}
/* uconv..----------------------------------------------------------------- */

/*
do the unit conversion mathematics
assumes from/to unit structures have been set up already by uanal

if 'k' is the stored conversion constant (desired units/primary units, e.g.
3.28 ft/m) and 'p' is the value of the stored prefix (e.g., micro=1e-6) and m
is the exponent associated with the conversion, then:

	fn=(p/k)^mfn	convert input numerator units to primary units
	fd=(k/p)^mfd	convert input denominator units to primary units
	tn=(k/p)^mtn	convert numerator primary units to desired units
	td=(p/k)^mtd	convert denominator primary units to desired units

x expressed in terms of the primary units (metric) can be found from:
   xp = x*fn*fd
*/

real uconv (real x)

{
/* from/to numerator/denominator conversion factors initialized to unity */
real fn=1.,tn=1.,fd=1.,td=1.;

/* numerator */
if (from.npk != -1) fn=pref[from.npk].val;	/* from prefix */
if (from.nuk != -1) fn=fn/unit[from.nuk].fact;	/* from -> primary */
if (from.ne != 1) fn=pow(fn,from.ne);		/* from power */
if (to.npk != -1) tn=1./pref[to.npk].val;	/* to prefix */
if (to.nuk != -1) tn=tn*unit[to.nuk].fact;	/* primary -> to */
if (to.ne != 1) tn=pow(tn,to.ne);		/* to power */

/* denominator */
if (from.dpk != -1) fd=1./pref[from.dpk].val;	/* from prefix */
if (from.duk != -1) fd=fd*unit[from.duk].fact;	/* from -> primary */
if (from.de != -1) fd=pow(fd,from.de);		/* from power */
if (to.dpk != -1) td=pref[to.dpk].val;		/* to prefix */
if (to.duk != -1) td=td/unit[to.duk].fact;	/* primary -> to */
if (to.de != 1) td=pow(td,to.de);		/* to power */

yprimary=x*fn*fd;

return x*fn*fd*tn*td;				/* put it all together */
}

/* deplural..-------------------------------------------------------------- */

/*
remove pluralizations from unit string

BE CAREFUL:  If you remove 'ES' from 'MILES' you're left with another unit! - 
	in this case the unit 'MIL'.  One way to circumvent this is to
	purposely misspell troublesome units - e.g., 'MILL' for 'MIL'.
	A better solution is to explicitly enter 'MILES' as a unit name in
	UNIT.DAT, then the check below will catch it.
*/

void deplural (char *n)

{
int i,l;
char temp [40];

/* if it's already a unitname, e.g., FPS or MILES, no need to depluralize */
for (i=0 ; i<nu ; i++) {if (strcmpi(n,unit[i].name) == 0) return;}

l=strlen(n); 
if (strcmpi(&n[l-3],"IES") == 0) n[l-3]='\0';
if (strcmpi(&n[l-2],"ES") == 0) n[l-2]='\0';
if (strcmpi(&n[l-1],"S") == 0) n[l-1]='\0';
}

/* trim..------------------------------------------------------------------ */

/* trim leading and trailing white space from a string */
/* returns pointer to trimmed string */
/* used by namelist function */
/* a veritable orgy of obscure pointer arithmetic */

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
/* fact..------------------------------------------------------------------ */

/*
determine unit conversion factor from string read from data file
data is either a string representing a number or represents 1/number 
*/

dbl fact (char *s)

{
char *p;

if ((p=strchr(s,'/')) == NULL) return (dbl) atof(s);
  else return (dbl) (1./atof(p+1));
}
/* convert..--------------------------------------------------------------- */

/* perform a unit conversion
n is used to control repeat conversions of the same quantity
*/

void convert (int n)

{
int i;
char buf[80];		/* buffer */
char val[40];		/* stores value to convert */
static real x;		/* numerical value to convert */
real y;			/* converted value */
char f[40];		/* stores from unit expression */
char t[40];		/* stores to unit expression */

puts ("UNIT SYNTAX:    (prefix)UNIT[^m [[/] (prefix)UNIT[^n]]]");
if (n) goto rpt;
getx:
printf ("%s ? ","[VALUE] [UNITS] TO CONVERT (or Quit)");
gets(buf); strupr(buf);
if (strlen(buf) == 0) {beep; goto getx;}
if (buf[0] == 'Q' && strlen(buf) == 1) return;
if (strchr(buf,' ') != NULL)
{
strcpy (val,strtok(buf," ")); strcpy (f,strtok(NULL," "));
}
else
{
	if (isalpha(buf[0]))
	  {strcpy (val,"1"); strcpy (f,buf);}
	else
	  {strcpy (val,buf); f[0]='\0';}
}
if (sscanf(val,"%f",&x) != 1) {beep; goto getx;}
getf: if (strlen(f) == 0)
{printf ("%s ? ","FROM UNIT(S)"); gets(f); strcpy (f,trim(strupr(f)));}
rpt: if (!uanal(f,&from)) {beep; f[0]='\0'; if (n) goto getx; else goto getf;}
gett: printf ("%s ? ","TO UNIT(S)"); gets(t); strcpy (t,trim(strupr(t)));
if (!uanal(t,&to)) {beep; t[0]='\0'; goto gett;}

/* check for from/to units compatibility */

for (i=0 ; i<NPRIM ; i++)
	{
	if (from.pu[i] != to.pu[i])
  	  {puts ("INCOMPATIBLE UNITS"); beep; return;}
	}

y=uconv(x);
engprnt (x); printf (" %s = ",f);
engprnt (y); printf (" %s = ",t);
engprnt (yprimary); pprim (uprimary,&from.pu[0]); print;
}
/* engprnt..--------------------------------------------------------------- */

/* print a number in engineering notation (exponent a multiple of three) */

void engprnt (real x)

{
int k=0;
real z;
char buf[40];

sprintf (buf,"%G",x);		/* if 'G' formatting contains an exponent */
if (strchr(buf,'E'))		/* make it a multiple of three */
	{
	k=floor(log10((dbl)ABS(x))); z=ABS(x)*pow(10.,-k);
	while (((ABS(k) % 3) != 0) || (z < 1)) {z=z*10.; k--;}
	printf ("%10.6fE%+04i",SGN(x)*z,k);
	}
else				/* else let C do the work */
	printf ("%s",buf);
}
/* define..---------------------------------------------------------------- */

/* define a new unit */

void define (void)

{
int i,j,k,exp;
char buf[80];

if (nu >= NU) {beep; puts("no space available for new units"); return;}
getn:
printf ("%s ? ","NEW UNIT NAME"); gets(buf); strcpy (buf,trim(strupr(buf)));
if (strlen(buf) == 0) return;
if (ufind(buf) != -1) {beep; puts ("name already in use"); goto getn;}
if (!isalpha(buf[0])) {beep; puts ("must start with a-z"); goto getn;}
strcpy (unit[nu].name,buf); unit[nu].fact=1.;
getu:
printf ("%s ? ","PRIMARY UNIT [default=done]");
gets(buf); strcpy (buf,trim(strupr(buf)));
if (strlen(buf) == 0) goto done;
if ((k=ufind(buf)) == -1) {beep; puts ("unit not found"); goto getu;}
gete:
printf ("%s ? ","INTEGER EXPONENT [1]");
gets(buf); strcpy (buf,trim(strupr(buf)));
if (strlen(buf) == 0) {exp=1; goto skip;}
if (sscanf(buf,"%d",&exp) != 1) {beep; puts ("try again"); goto gete;}
if (exp == 0) {beep; puts ("zero not allowed"); goto gete;}
skip: for (i=0 ; i<ABS(exp) ; i++)
{if (exp > 0) unit[nu].fact*=unit[k].fact; else unit[nu].fact/=unit[k].fact;}
for (i=0 ; i<NPRIM ; i++) unit[nu].pfact[i]+=exp*unit[k].pfact[i];
goto getu;
done:
printf ("to save this unit, add the line below to %s\n",nlfile);
printf ("%s=%g",unit[nu].name,unit[nu].fact);
for (i=0 ; i<NPRIM ; i++) printf (",%d",unit[nu].pfact[i]);
print;
nu++; sort();
}
/* compound..-------------------------------------------------------------- */

/* 
show all units corresponding to a given compound type
*/

void compound (void)

{
int i,j,t;
char op,pf[NPRIM];

for (i=0 ; i<NPRIM ; i++) pf[i]=0;

puts ("\n0  amount");
puts ("1  mass");
puts ("2  time");
puts ("3  angle");
puts ("4  length");
puts ("5  area");
puts ("6  volume");
puts ("7  speed");
puts ("8  acceleration");
puts ("9  force");
puts ("10 torque or energy");
puts ("11 power");
puts ("12 pressure");
puts ("13 angular speed");
puts ("14 solid angle");

vin ("Desired unit type",&t,"d",0.,"");

switch (t)
{
	case 0:			//amount
		pf[6]=1;
		break;
	case 1:			//mass
		pf[1]=1;
		break;
	case 2:			//time
		pf[2]=1;
		break;
	case 3:			//angle
		pf[3]=1;
		break;
	case 4:			//length
		pf[0]=1;
		break;
	case 5:			//area
		pf[0]=2;
		break;
	case 6:			//volume
		pf[0]=3;
		break;
	case 7:			//speed
		pf[0]=1; pf[2]=-1;
		break;
	case 8:			//acceleration
		pf[0]=1; pf[2]=-2;
		break;
	case 9:			//force
		pf[0]=1; pf[1]=1; pf[2]=-2;
		break;
	case 10:		//torque,energy
		pf[0]=2; pf[1]=1; pf[2]=-2;
		break;
	case 11:		//power
		pf[0]=2; pf[1]=1; pf[2]=-3;
		break;
	case 12:		//pressure
		pf[0]=-1; pf[1]=1; pf[2]=-2;
		break;
	case 13:		//angular speed
		pf[2]=-1; pf[3]=1;
		break;
	case 14:		//solid angle
		pf[4]=1;
		break;

	//LENGTH,MASS,TIME,ANGLE,SOLIDANGLE,CHARGE,AMOUNT

	default:
		beep; puts ("NOT A VALID OPTION");
		return;
}

for (i=0 ; i<nu ; i++)
	{
	for (j=0 ; j<NPRIM ; j++) if (unit[i].pfact[j] != pf[j]) goto iend;
	printf ("%-15s ",unit[i].name);
iend:;
	}
print;
}
/* ------------------------------------------------------------------------ */
