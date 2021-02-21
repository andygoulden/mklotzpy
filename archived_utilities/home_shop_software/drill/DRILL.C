/* DRILL.C   M. W. Klotz  11/98, 6/00, 4/02, 2,4/03, 2/06

Drill sizes

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

/* \pmark..  functions peculiar to this program */
char getkey (char *prompt);
void menu (void);
char rdata (void);
int finddrill (dbl size);
dbl dfinput (char *prompt);
void dprint (int k, dbl size);
void ptext (char *text, int fgd, int bkg);
void fhole (void);
void tdrill (int opt);
char *trimx (char *s);
dbl carea (dbl d);
dbl cdiam (dbl area);
void step (void);

/* gmark..-------------- global graphics variables ------------------------ */
/* some are contained in graphics.h */

/* vmark..------------ global variable declarations ----------------------- */
FILE *fpt;		/* utility file pointer */
char dfile[] = "DRILL.DAT";		/* drill data file */
dbl td,p,mpi=25.4,dot,dtd;
dbl aa;			/* thread form constant */
char nomm=0;		/* no metric flag */

/* smark..----------- global structure declarations ----------------------- */
#define DMAX 500	/* maximum number of drills */
#define DLMAX 20	/* maximum characters in drill label */
int ndr;		/* actual number of drills */
struct drills
{
char name[DLMAX];
dbl size;
} dr[DMAX];


/* main..================================================================== */

main (int argc, char *argv[], char *envp[] )

/* argc=count of command line arguments */
/* argv points to parameters, envp points to environment parameters */

{
int i,j,k;
char op;
dbl v;

/* -------------------------- initializations ------------------------------ */
{_setvideomode(_TEXTC80); _setbkcolor(2); _settextcolor(14); cls;}
/* ------------------------------------------------------------------------- */

if (rdata()) goto mexit;
printf ("number of drills read from data file = %d\n",ndr);

menu();
agin: dot=0; op=getkey("(D,S,T,F,X,H,M,Q) ? ");
switch (op)
{
	case 'd':	/* find drill given hole size */
		v=dfinput ("Hole Size");
		k=finddrill (v);
		dprint (k,v);
		break;
	case 's':	/* find hole size given drill */
		fhole ();
		break;
	case 't':	/* find tapdrill */
		tdrill (0);
		break;
	case 'f':	/* find tapdrill for thread forming taps */
		tdrill (1);
		break;
	case 'x':	/* step drilling */
		step ();
		break;
	case 'h':	/* help */
	case 'm':	/* menu */
		menu();
		break;
	case 27:	/* quit */
	case 'q':
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
/* getkey..---------------------------------------------------------------- */

/* prompt and wait for a character from the user */

char getkey (char *prompt)
{
char c;
printf ("%s",prompt); c=getch(); printf ("\n"); return tolower(c);
}
/* menu..------------------------------------------------------------------ */

void menu (void)

{
puts ("D    find Drill designation given hole size");
puts ("S    find hole Size given drill designation");
puts ("T    find Tapdrill for any tap and dot");
puts ("F    find tapdrill for thread-Forming tap");
puts ("X    step drilling calculations");
puts ("H    display this Help/Menu");
puts ("M    display this Help/Menu");
puts ("Q    Quit (Esc also)");
}
/* finddrill..------------------------------------------------------------- */

/* returns index of drill closest to size */

int finddrill (dbl size)

{
int i,k;
dbl diff,best;

if (size <= dr[0].size) return 0;
if (size >= dr[ndr-1].size) return ndr-1;

for (i=0,best=1000.; i<ndr ; i++)
  {diff=ABS(size-dr[i].size); if (diff < best) {best=diff; k=i;}}

if (nomm) {while (strstr(dr[k].name,"mm") && !strstr(dr[k].name,"=")) k--;}

return k;
}
/* dfinput..--------------------------------------------------------------- */

/* input a number in either decimal or fractional form */

dbl dfinput (char *prompt)

{
char temp[81],*k,*m;
dbl v;

printf ("{[d].dd or [d-]d/d (e.g. 1.5 or 1-1/2)}  %s ? ",prompt);
gets (temp);
if (k=strchr(temp,'/'))
{
if (m=strchr(temp,'-')) {v=atof(strtok(temp,"-")); m++;} else {v=0; m=temp;}
v+=atof(trimx(strtok(m,"/")))/atof(trimx(strtok(NULL,"/")));
}
else v=atof(temp);
return v;
}
/* dprint..---------------------------------------------------------------- */

/* print drill and surrounding drills */

void dprint (int k, dbl size)

{
int l;

print;
for (l=k-3; l<=k+3; l++)
{
if (l<0 || l>ndr-1) continue;
if (l == k) printf("***** ");
printf ("(%s) with size %.4lf (%+.4lf)",dr[l].name,dr[l].size,dr[l].size-size);
if (dot) printf (" %.0lf%% dot",100*(td-dr[l].size)*p/aa);
if (l == k) printf(" *****");
printf ("\n");
}
print;
}
/* ptext..----------------------------------------------------------------- */

/* print text in color */

void ptext (char *text, int fgd, int bkg)

{
int pb,pf;
uchar row,col;

printf ("\n");
_asm					/* get cursor row */
	{
	mov	ah,15			/* read video state */
	int	10h			/* bh = display page */
	mov	ah,3
	int	10h
	mov	row,dh			/* cursor row */
	mov	col,dl
	}
pb=_setbkcolor(bkg); pf=_settextcolor (fgd);
_settextposition (row,col); _outtext(text);
_setbkcolor(pb); _settextcolor (pf);
}
/* fhole..----------------------------------------------------------------- */

/* find hole size given drill designation */

void fhole (void)

{
char b[81];
int i;

printf ("Drill Designation (e.g. 6, F, 3.10 mm, 3/8, 1-1/4) ? ");
gets (b);
for (i=0 ; i<ndr ; i++)		/* try for exact match */
 {if (!strcmpi(dr[i].name,b)) {dprint (i,dr[i].size); return;}}
for (i=0 ; i<ndr ; i++)		/* try for substring match */
 {if (strstr(dr[i].name,b)) {dprint (i,dr[i].size); return;}}
beep; puts ("NO MATCH FOUND");

}
/* tdrill..---------------------------------------------------------------- */

/* calculate tapdrill */

void tdrill (int opt)

{
char im,nu,si;
int k;
dbl tdm,pm;

im=getkey ("[I]nch [Default] or (M)etric Tap ? ");
if (im != 'm') im='i';
if (im == 'm')
{
if (opt)
{
puts ("For metric taps the approximate tap drill size can be found from:");
puts ("tapdrill diameter (mm) = tap diameter (mm) - pitch of tap (mm)");
}
vin ("Tap diameter",&tdm,"lf",6.,"mm"); td=tdm/mpi;
vin ("Pitch of tap",&pm,"lf",1.,"mm/thread"); p=mpi/pm;
aa=(5./8.)*TAND(60.);
si=getkey ("[S]tandard or (I)SO Thread Form ? ");
if (si != 'i') si='s';
if (si == 'i') aa=(5./8.)*TAND(60.); else aa=(6./8.)*TAND(60.);
}
else
{
puts ("Diameters of numbered taps:");
puts ("#0000 (160) = .021  #000 (120) = .034  #00 (90) = .047");
puts ("#0 = .060  #1 = .073  #2 = .085  #3  = .099  #4  = .112");
puts ("#5 = .125  #6 = .138  #8 = .164  #10 = .190  #12 = .216");
puts ("(for number sizes >= 0, you can enter number with a leading minus)");
td=dfinput ("Tap Diameter (in)");
if (td <= 0) td=0.060-td*0.013;
p=dfinput ("Pitch of Tap (threads/in)");
}
if (opt == 0)
{
if (im == 'i')
{
nu=getkey ("American [N]ational or (U)nified Thread Form ? ");
if (nu != 'u') nu='n';
if (nu == 'u') aa=(5./8.)*TAND(60.); else aa=(6./8.)*TAND(60.);
}
puts ("Recommended Depth of Thread:");
puts ("MATERIAL                                          % DOT\n");
puts ("MILD AND UNTREATED STEELS                         60-65");
puts ("HIGH CARBON STEEL                                 50");
puts ("HIGH SPEED STEEL                                  55");
puts ("STAINLESS STEEL                                   50");
puts ("FREE CUTTING STAINLESS STEEL                      60");
puts ("CAST IRON                                         70-75");
puts ("WROUGHT ALUMINUM                                  65");
puts ("CAST ALUMINUM                                     75");
puts ("WROUGHT COPPER                                    60");
puts ("FREE CUTTING YELLOW BRASS                         70");
puts ("DRAWN BRASS                                       65");
puts ("MANGANESE BRONZE                                  55");
puts ("MONEL METAL                                       55-60");
puts ("NICKEL SILVER (GERMAN SILVER)                     50-60");
}
vin ("Depth of thread desired",&dot,"lf",75.,"%");

printf ("\nTap Diameter = %.4lf in = %.4lf mm\n",td,td*mpi);
printf ("Tap Pitch = %.4lf threads/in = %.4lf in/thread\n",p,1/p);
printf ("          = %.4lf threads/mm = %.4lf mm/thread\n",p/mpi,mpi/p);
if (opt == 0)
{
printf ("Thread form constant used = %.4lf\n",aa);
dtd=td-.01*dot*aa/p;			/* tap drill diameter */
}
else
{
dtd=td-0.0068*dot/p;			// imperial thread forming
if (im == 'm') dtd=(tdm-dot*pm/147.06)/mpi;	// metric thread forming
}
printf ("TAP DRILL DIAMETER FOR %.0lf%% DOT = %.4lf in = %.4lf mm\n",\
  dot,dtd,dtd*mpi);
k=finddrill (dtd);
if (opt == 1) dot=0;
dprint (k,dtd);
}
/* rdata..----------------------------------------------------------------- */

/* read the data file */

char rdata (void)

{
char temp[81],f;
int i;
dbl t;

if ((fpt=fopen(dfile,"rt")) == NULL)
	{printf ("FAILED TO OPEN DATA FILE: %s\n",dfile); beep; return 1;}

ndr=0;			/* number of drills */
f=0;
while ((fgets(temp,sizeof(temp),fpt)) != NULL)
	{
	trim(temp);		/* uses library trim */
	if ((strstr(temp,"STARTOFDATA")) != NULL) {f=1; continue;}
	if (!f) continue;
	if ((strstr(temp,"ENDOFDATA")) != NULL) break;
	if (temp[0] != '\0' && temp[0] != ';')
		{
		/* decode and store data here */
		strcpy (dr[ndr].name,trimx(strtok(temp,"\t")));
		dr[ndr].size=atof(trimx(strtok(NULL,"\t")));
		if (++ndr == DMAX)
			{
			printf
			("More than maximum (%d) data items in data file\n",DMAX);
			quit;
			}
		}
	}

do		/* sort data */
{
for (i=0,f=0 ; i<ndr-1 ; i++)
	{
	if (dr[i].size > dr[i+1].size)
		{
		strcpy (temp,dr[i].name);
		strcpy (dr[i].name,dr[i+1].name);
		strcpy (dr[i+1].name,temp);
		t=dr[i].size; dr[i].size=dr[i+1].size; dr[i+1].size=t; f=1;
		}
	}
} while (f);

fclose (fpt);
return 0;
}
/* trimx..----------------------------------------------------------------- */

/* trim leading and trailing white space from a string */
/* returns pointer to trimmed string */

char *trimx (char *s)
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
/* carea..----------------------------------------------------------------- */

/* compute area of circle given diameter */

dbl carea (dbl d)

{
return 0.25*PI*d*d;
}
/* cdiam..----------------------------------------------------------------- */

/* compute diameter of circle given area */

dbl cdiam (dbl area)

{
return 2.0*sqrt(area/PI);
}
/* step..------------------------------------------------------------------ */

void step (void)

{
char b[81],op;
int kf,kp,k;
dbl df,dp,fr,f,d;
dbl af,area;

op=getkey("Include metric drills in search [Y]/N?");
if (op != 'n') op='y';
if (op == 'n') nomm=1;
df=dfinput ("Final Hole Size");
kf=finddrill (df);
if (dr[kf].size != df)
{
puts ("Final hole size drill NOT in data file.  Will use closest value:");
sprintf (b,"CLOSEST DRILL IS (%s) WITH SIZE %.4lf (%+.4lf)\n",\
  dr[kf].name,dr[kf].size,dr[kf].size-df);
ptext (b,15,2);
df=dr[kf].size;
}
dp=dfinput ("Pilot Hole Size");
kp=finddrill (dp);
vin ("Percentage of material to remove with each step",&fr,"%.0lf%%",20.,"");
f=0.01*fr;
af=carea(df);
area=carea(dp);
print;
if (area)
printf ("Pilot drill removes %.0lf%% of material to be removed\n",100.*area/af);

if (area < f*af) {area=f*af; goto skip;}
agin:
area+=f*af; if (area > af) area=af;

skip:
d=cdiam(area);
k=finddrill(d);
area=carea(d);
printf ("Drill (%s) with size %.4lf removes %.0lf%%\n",\
  dr[k].name,dr[k].size,100.*area/af);
if (k != kf) goto agin;
nomm=0;
return;
}
/* ------------------------------------------------------------------------ */
