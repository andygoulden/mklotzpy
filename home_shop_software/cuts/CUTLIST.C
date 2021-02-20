/*
  Cutlist
  by Mike Graham
  Feb 8, 2002

  This is an attempt to create a better solution to the problem of creating a
cutting list than has thus-far been implemented in Marv's CUTS program.
  The heuristics in Cutlist are fundamentally different than in CUTS.
  Whether these heuristics are superior remains to be seen.

  Marv's algorithm picks the best-fit piece for the current length of stock.
  My algorithm picks the best-fit piece of stock for the current piece.
  Similar, but backwards.  Will it make a difference?  Maybe.

  - - - - -

  Having completed the trial run of Cutlist, I find that it finds the same
  solution as CUTS in many cases, but finds superior solutions in others.
  We have not yet found a case where it is inferior since correcting for a
  floating-point drift problem.

  - - - - - -

  Like Marv's CUTS program, Cutlist reads in a cutting list stored in a text
  file.  The format is the same.  The primary difference is that Cutlist will
  simply keep reading data until there is no more to be read... it doesn't
  require the ENDOFDATA marker, though it will be acted upon if found.

  Unlike CUTS, you can specify the name of the datfile on the command line,
  though it will default to 'cuts.dat'.

*/

#include <stdio.h>
#include <string.h>

/* Limiting values, and other constants */
#define MAXPIECES 1000          // Maximum number of pieces to be cut
#define MAXPIECESPERLENGTH 100  // Maximum number of pieces per length of stock
#define MAXLENGTHS 100			// Maximum number of lengths to be used
#define ulong unsigned long
const char	blankline[5] = {' ','\n','\t',';','\0'};

/* structures */

typedef struct length {
	ulong cuts;     	             	// Number of pieces cut from this length
	ulong drop;              			// unallocated size.
	ulong size[MAXPIECESPERLENGTH];    	// sizes of pieces
};

/* Variables */

FILE *Datfile; 							// text file of sizes to cut
char Datfilename[80] = "cuts.dat\0";  	// this is the default.. it can be
										// overridden at the command line.

char Inlin[220];	// general storage for reading a line of text from *Datfile
char Error[80];		// used for outputting Error messages

ulong Stocklength;			// length of standard stock
ulong Pieces[MAXPIECES]; 	// sizes to cut
ulong NumPieces;			// used to index the Pieces array - number of Pieces to cut
struct length Stock[MAXLENGTHS];	// Stock represents the pieces of stock
									// being cut up, and stores the pieces
									// being cut from it, and the drop length


/* 
	init - set all variables to initial values 
*/

void init(void)
{
	printf("\nCutlist 1.0\nPublic Domain by Mike Graham\n");
	memset(Stock, 0, sizeof(Stock));
	memset(Pieces, 0, sizeof(Pieces));
	NumPieces=0;
	Inlin[0]='\0';
	Error[0]='\0';
	Stocklength=0;
}

/*
	crash - end the program and output an Error message.
*/
void crash(void)
{
	printf("CUTLIST-F-%s\n",Error);
	if (Datfile!=NULL) fclose(Datfile);
	exit(1);
}

/*
	readnext - return the next line of useful info from the dat file
*/
char *readnext(void)
{
	do {
		fgets(Inlin, 220, Datfile);
	}
	while (!feof(Datfile) & (strchr(blankline,Inlin[0])!=NULL));
	if (feof(Datfile) | (strstr(Inlin,"ENDOFDATA")!=NULL)) return NULL; else return Inlin;
}

/*
	readdat - open and read the data file.
*/
void readdat(void)
{
	ulong i=0;	// temporary index value
	ulong j=0;	// temporary index for number of Pieces
	long double f=0;	// temporary storage for floating point values
						// to be multiplied by 1000 and stored in unsigned
						// variables.

	/* If the file won't open, crash with an error */
	if ((Datfile=fopen(Datfilename,"r"))==NULL) {
		strcpy(Error,"NOINPUT : Couldn't open input file \"");
		strcat(Error,Datfilename);
		strcat(Error,"\"\n");
		crash();
	}
	/*  Read first useful line from Datfile, should contain stock length  */
	if (sscanf(readnext(), "%LE\n", &f)!=1){
		strcpy(Error,"BADDATFILE : Stock length read error.");
		crash();
	} else
		Stocklength=f*1000;
	/*  Now that we have the stock length, fill it in on the stock array */
    for (i=0;i<MAXLENGTHS;i++) Stock[i].drop=Stocklength;


	/*  Keep reading lines until there aren't any more to be read */
	while (readnext()!=NULL) {
		/*  Each line should have an unsigned int, a comma, and a floating
			point number, representing the number of pieces of this size to
			cut, and the size.  */
		if (sscanf(Inlin,"%u,%LE\n",&j,&f)!=2) {
			strcpy(Error,"BADDATFILE : Piece info read error.\n");
			strcat(Error,"Offending line: ");
			strcat(Error,Inlin);
			crash();
		}

		/* Add the pieces to the Pieces array */
		for (i=0;i<j;i++) {
			Pieces[NumPieces]=f*1000;
			NumPieces++;
		}
	}

	fclose(Datfile);
}

/*
  void sortpieces(void)

  Sort the entries in the pieces array.  In the interest of keeping this
  extremely portable I'll just type in a simpleminded sort myself rather
  than using the quicksort algorithm that is built into many, but not all,
  of the compilers out there.  This is just a bubble sort, which is
  hideously slow as it is an O(n^2) algorithm, but it's more than fast
  enough to sort the number of pieces being cut by the average person.
  This function will sort in decreasing order.
*/

void sortpieces(void)
{
	ulong i=0;
	ulong j=0;
	ulong f=0;

	for (i=0;i<NumPieces;i++)
		for (j=i+1;j<NumPieces;j++)
			if (Pieces[i]<Pieces[j]) {
				f=Pieces[i];
				Pieces[i]=Pieces[j];
				Pieces[j]=f;
			}
}

/*
  void cutlist(void)

  The basic algorithm is this: for each piece to be cut, have it cut from
  the shortest piece that it fits in.  There's not much to it, eh?
*/
void cutlist(void)
{
	ulong i=0,j=0;	// temporary indexing variable
	ulong best=0;	// index of the current best stock length
	for (i=0;i<NumPieces;i++) {
		j=0;
		while (Stock[j].drop<Pieces[i]) j++;
		best=j;
		while (Stock[++j].drop!=Stocklength) 
			if ((Stock[j].drop<Stock[best].drop) & (Stock[j].drop>Pieces[i]))
				best=j;
		Stock[best].drop-=Pieces[i];
		Stock[best].size[Stock[best].cuts]=Pieces[i];
		Stock[best].cuts++;
	}

}

/*
  void printresults(void)

  Output the results to the screen.  I use a relatively verbose output
  format, because I like to be able to easily cross sizes off on a print-out.
  The extra space is a cheap alternative to accidentally cutting a piece
  twice.
*/
void printresults(void)
{
	ulong f=0;
	ulong waste=0;     	// to store the actual waste
	ulong lengths=0; 	// to store the actual number of lengths used
	ulong i=0,j=0;

	for (i=0;i<NumPieces;i++) f+=Pieces[i];

	printf("Number of pieces to cut = %Lu\n", NumPieces);
	printf("Total length of material being cut = %.3Lf units\n", (long double)f/1000);
	printf("Standard length = %.3Lf units \n", (long double)Stocklength/1000);
	printf("Theoretical minimum waste possible = %.3Lf units \n", (long double)(Stocklength-f%Stocklength)/1000);
	printf("Theoretical minimum standard lengths possible = %Lu\n\n", (unsigned int)(f/Stocklength)+1);

	i=0;
	j=0;
	f=Pieces[i];
	do {
		if (f!=Pieces[i])  {
			printf("%3u piece(s) %.3Lf units long.\n", (unsigned)j, (long double)f/1000);
			j=1;
			f=Pieces[i];
		} else {
			j++;
		}
		i++;
	} while (i<=NumPieces);

	printf("\n\nCutlist:\n");
	i=0;
	do {
		lengths++;
		waste+=Stock[i].drop;
		printf("Length #%d - \n",i+1);
		for (j=0;j<Stock[i].cuts;j++)
			printf("   %.3Lf units\n", (long double)Stock[i].size[j]/1000);
	} while (Stock[++i].drop!=Stocklength);

	printf("\n\nActual waste = %.3Lf\n", (long double)waste/1000);
	printf("Actual standard lengths = %d\n\n", lengths);
}

void main(int argc, char **argv)
{
	/*  Initialize variables */
	init();
	/*  If a parameter was passed, assume it is the datfile name */
	if (argc==2) {
		strcpy(Datfilename, argv[1]);
		printf("Reading data file \"%s\"\n",Datfilename);
	} else
		printf("No data file specified.  Defaulting to \"%s\"\n",Datfilename);
	/*  Read the data file */
	readdat();
	/*  Sort the Pieces */
	sortpieces();
	/*  Run the cutlist algorithm */
	cutlist();
	/*  Output the results */
	printresults();
}
