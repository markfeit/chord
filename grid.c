static char copyright[] = "Copyright 1991-1993 by Martin Leclerc & Mario Dorion";

#include "chord.h"

struct chord_struct chordtab[MAX_CHORD];
int    nb_chord = 0;
int	first_ptr = 0;

/* char	known_chords[256][CHORD_NAME_SZ]; */
struct kcs {
	char	chord_name[CHORD_NAME_SZ];
	int	displ;
	int	s1,s2,s3,s4,s5,s6;
	int	origin;
	int	difficult;
} known_chords[MAX_CHORD];


int	nb_known_chord = 0;

extern int hpos,vpos;
extern int grid_size, no_easy_grids, n_pages;
extern int first_time_in_chordrc;
extern char *mesg;
extern int in_chordrc;
extern int sort_type;

/*--------------------------------------------------------------------------------*/
void do_init_grid_ps()
{
	printf("%% routines for the drawing of the chords\n");
	printf("/grid { \n");
	printf("	gsave currentpoint\n");
	printf("	6 { 0 cagesizeX rlineto incr cagesize neg incr add rmoveto } repeat\n");
	printf("	moveto\n");
	printf("	5 { cagesize 0 rlineto 0 cagesize sub incr rmoveto } repeat\n");
	printf("	1 setlinewidth stroke grestore\n");
	printf("	gsave\n");
	printf("	0 cagesizeX rmoveto cagesize 0 rlineto\n");
	printf("	cagesize 30 div setlinewidth stroke\n");
	printf("	grestore	\n");
	printf("} def\n");
	printf("\n");
	printf("/dot {\n");
	printf("	gsave\n");
	printf("	/fret exch def /str exch def\n");
	printf("	/x 5  fret abs sub incr mul half-incr sub def\n");
	printf("	/y 6 str sub incr mul def\n");
	printf("	y x rmoveto\n");
	printf("	0 setgray\n");
	printf("	fret -1 eq\n");
	printf("	  {\n");
	printf("	  0 incr rmoveto\n");
	printf("	  check neg  check neg rmoveto\n");
	printf("	  check check rlineto check check rlineto\n");
	printf("	  check neg check neg rmoveto check neg check rmoveto\n");
	printf("  	  check check neg rlineto check check neg rlineto\n");
	printf("	  cagesize 50 div setlinewidth stroke\n");
	printf("	  }\n");
	printf("	  {  fret 0 ne\n");
	printf("	    { currentpoint dot-size 0 360 arc fill}\n");
	printf("	    { currentpoint\n");
	printf("	      check 0 360 newpath arc\n");
	printf("	      cagesize 50 div setlinewidth\n");
	printf("	    }\n");
	printf("	    ifelse \n");
	printf("	    stroke \n");
	printf("	  }\n");
	printf("	  ifelse\n");
	printf("	grestore\n");
	printf("} def	\n");
	printf("\n");
	printf("/dots {\n");
	printf("	grid\n");
	printf("	/origin exch def\n");
	printf("	/fret-displ exch def\n");
	printf("	  1 1 6\n");
	printf("	  {\n");
	printf("	    /str exch def\n");
	printf("	    /fret exch def\n");
	printf("	    fret %d ne {str fret dot} if\n", FRET_NONE);
	printf("	  } for\n");
	printf("\n");
	printf("	fret-displ 1 ne\n");
	printf("	{\n");
	printf("    	  gsave displ-font setfont\n");
/*	printf("    	  0 cagesize 3 div sub cagesizeX incr sub rmoveto\n"); */
	printf("    	  0 cagesize fret-displ 10 ge {1.75} {3} ifelse div sub\n");
	printf("	  cagesizeX incr sub rmoveto\n");
	printf("    	  /char 6 string def fret-displ char cvs show grestore \n");
	printf("	} if\n");
	printf("\n");
	printf("	/chord-name exch def\n");
	printf("	gsave displ-font setfont\n");
	printf("	0 cagesizeX cagesizeX 15 div add dot-size 3 mul add rmoveto\n");
	printf("	chord-name stringwidth pop 2 div cagesize 2 div exch sub 0 rmoveto\n");
	printf("	chord-name show\n");
	printf("	0 cagesize 10 div rmoveto\n");
	printf("	origin 1 eq { (*) show} if\n");	
	printf("	origin 2 eq { (**) show} if\n");	
	printf("	chord-font setfont grestore\n");
	printf("} def\n");
	printf("\n");
	printf("/cagesize %d def\n",grid_size);
	printf("/incr cagesize 5 div def\n");
	printf("/cagesizeX incr 4 mul def\n");
	printf("/half-incr incr 2 div def\n");
	printf("/dot-size cagesize 15 div def\n");
	printf("/check cagesize 20 div def\n");
	printf("\n");
	printf("/chord-font /Times-Roman findfont cagesize 5 div scalefont def\n");
	printf("/displ-font /Times-Roman findfont cagesize 3 div scalefont def\n");
	printf("%% end of the routines for the drawing of the chords\n");
	printf("\n");
}

/*--------------------------------------------------------------------------------*/

int add_to_chordtab(chord)
char *chord;
{
	int i, prev_i;
	int n;
	char chord1[CHORD_NAME_SZ],chord2[CHORD_NAME_SZ];

  	/* printf(mesg, "attempting to add (%s)\n",chord);
	debug (mesg); */

	i= first_ptr; prev_i = -1;
	while (TRUE)
	{
		if (nb_chord > 0 && i != -1) {
			
			strcpy(chord1, chord);
			strcpy(chord2, chordtab[i].chord_name);
			n= chordcompare(chord1, chord2);
			if (n == 0)
				return(i);
			else if (n>0) {
				prev_i= i;
				i= chordtab[i].next_ptr;
				continue;
				}
			}
 		/* sprintf(mesg, "inserting between %d and %d\n",prev_i,i); 
		debug(mesg); */

		strcpy(chordtab[nb_chord].chord_name,      chord);
		if (nb_chord != 0)
			chordtab[nb_chord].next_ptr= i;
		else
			chordtab[nb_chord].next_ptr = -1;
		if (prev_i != -1)
			chordtab[prev_i].next_ptr = nb_chord;
		if (i== first_ptr)
			first_ptr= nb_chord;
                if (nb_chord >= MAX_CHORD-1) {
		    fprintf(stderr, "too many chords, bumping %s\n",
			chordtab[nb_chord-1].chord_name);
		    return(nb_chord - 1);
		}
		nb_chord++;

		return(nb_chord - 1);
	}
}

/*--------------------------------------------------------------------------------*/
/* ChordCompare - provided by Leo Bicknell (ab147@freenet.acsu.buffalo.edu) */
int chordcompare(chord1, chord2)
char *chord1, *chord2;
{
    chord1[0]=tolower(chord1[0]);
    chord2[0]=tolower(chord2[0]);

    if (chord1[0] != chord2[0])
    {
        return(chord1[0] - chord2[0]);
    }
    else
    {
        switch (chord1[1]) {
            case 'b':
                switch (chord2[1]) {
                    case 'b':
                        return (chordcompare(&chord1[1], &chord2[1]));
                    case '#':
                        return (-1);
                    case 'm':
                        return (-1);
                    default:
                        return (-1);
                    }
            case '#':
                switch (chord2[1]) {
                    case 'b':
                        return (1);
                    case '#':
                        return (chordcompare(&chord1[1], &chord2[1]));
                    case 'm':
                        return (1);
                    default:
                        return (1);
                    }
            case 'm':
                switch (chord2[1]) {
                    case 'b':
                        return (1);
                    case '#':
                        return (-1);
                    case 'm':
                        return (chordcompare(&chord1[1], &chord2[1]));
                    default:
                        return (1);
                    }
            default:
                switch(chord2[1]) {
                    case 'b':
                        return (1);
                    case '#':
                        return (-1);
                    case 'm':
                        return (-1);
                    default:
                        return (strcmp(chord1, chord2));
                    }
            }
    }
}


/*--------------------------------------------------------------------------------*/
void moveto(new_hpos,new_vpos)
int	new_hpos,new_vpos;
{
	if (new_hpos + grid_size + L_MARGIN > WIDTH)
		{
		new_hpos = L_MARGIN;
		new_vpos -= 2*grid_size;
		}

	if (new_vpos < BOTTOM)
		{
		do_end_of_page(FALSE);
		do_start_of_page();
		new_vpos= TOP-2*grid_size;
		}

	printf ("%d %d moveto\n", new_hpos, new_vpos);
	hpos= new_hpos;vpos = new_vpos;
}

/*--------------------------------------------------------------------------------*/
void draw_chords()
	{
	int ptr;
	int idx;

	moveto(WIDTH - grid_size - grid_size - L_MARGIN, vpos); 

	ptr= first_ptr;
	if (nb_chord != 0)
		{
		while (ptr != -1) 
			{
			if ((idx= is_chord_known(chordtab[ptr].chord_name))== -1)
				{
				moveto(hpos + 2 * grid_size, vpos);
				sprintf (mesg, "chord \'%s\' has never been defined",
					chordtab[ptr].chord_name);
				error(mesg);
	
				printf("(%s) -2 -2 -2 -2 -2 -2 1 0 dots\n",
					 chordtab[ptr].chord_name);
				}
			else if (!no_easy_grids || no_easy_grids && known_chords[idx].difficult) 
				{
				moveto(hpos + 2 * grid_size, vpos);
				printf("(");
				ps_puts(known_chords[idx].chord_name);
				printf(") %d %d %d %d %d %d %d %d dots\n",
					known_chords[idx].s1,
					known_chords[idx].s2,
					known_chords[idx].s3,
					known_chords[idx].s4,
					known_chords[idx].s5,
					known_chords[idx].s6,
					known_chords[idx].displ,
					known_chords[idx].origin);
				}
			ptr = chordtab[ptr].next_ptr;
			}
		}
	}

/*--------------------------------------------------------------------------------*/
learn_chord(chord, s1, s2, s3, s4, s5, s6, displ, origin, difficult)
char 	*chord;
int	displ;
int	s1,s2,s3,s4,s5,s6;
int	origin, difficult;
{
        int i,not_found;

        not_found= TRUE;
        for (i= 0;i < nb_known_chord && not_found; i++) {
                not_found= strcmp(chord,known_chords[i].chord_name);
        };

        if (not_found) {
                if (nb_known_chord >= MAX_CHORD-1) {
		    fprintf(stderr, "too many chords, can't learn %s\n", chord);
		    return(0);
		}
                nb_known_chord++;
		} else {
		i--;
		} 

	if (displ == 0) displ = 1;
        strcpy(known_chords[i].chord_name,chord);
	known_chords[i].displ= displ;
	known_chords[i].s1= s1;
	known_chords[i].s2= s2;
	known_chords[i].s3= s3;
	known_chords[i].s4= s4;
	known_chords[i].s5= s5;
	known_chords[i].s6= s6;
	known_chords[i].origin= origin;
	known_chords[i].difficult= difficult;
	
	return(0);
}

/*--------------------------------------------------------------------------------*/
int check_old_define_syntax(temp_str, chord_name)
char 	*temp_str;
char	*chord_name;
{
	char 	*str;
	int 	n;
	int 	ok = TRUE;
	int	fret_displ;
	int	dot_array[6];
	int 	i;
	char 	error_str[MAXLINE];

	fret_displ=atoi(temp_str);

	for (n=0; n<=5; n++) {
		str=toupper_str(strtok(NULL, DELIM_STR));
		if (!strcmp(str,FRET_NONE_STR) || !strcmp(str, FRET_X_STR))
			dot_array[n]=FRET_X;
		else
			dot_array[n]=atoi(str);

		ok = ok && (str);
		}

	if (strtok(NULL, DELIM_STR))
		ok=FALSE;

	if (ok ) {
		if (!in_chordrc || first_time_in_chordrc) {
			sprintf(error_str,
				"definition of chord '%s' uses obsolete format",
				chord_name);
			error(error_str);
			fprintf(stderr,
				"   Please edit your input file and replace its %s",
				"definition by the following:\n");
			fprintf(stderr, "   {define: %s base-fret %d frets ",
			   chord_name, fret_displ);
			for (i=5; i>=0; i--)
				if (dot_array[i] < 0)
					fprintf (stderr, "x ");
				else
					fprintf (stderr, "%d ", dot_array[i]);
			fprintf (stderr, "}\n\n");
			}


		learn_chord(chord_name,
			dot_array[5],dot_array[4],dot_array[3],
			dot_array[2],dot_array[1],dot_array[0],
			fret_displ, in_chordrc ? CHORD_DEFINED : CHORD_IN_CHORDRC,
			CHORD_HARD);
		}

	return(ok);
}
	

/*--------------------------------------------------------------------------------*/
int do_define_chord()
{
	int dot_array[6];
	char *chord_name;
	char *temp_str;
	int fret_displ;
	int n;
	int hardtoplay = 0;

	for ( n= 0; n<6; n++)
		dot_array[n] = 0;

	chord_name= strtok(NULL, DELIM_STR);
	if (chord_name== NULL) {
		error("syntax error in chord definition: no chord name");
		return(0);
	}

	temp_str= tolower_str(strtok(NULL, DELIM_STR));
	if (strcmp(temp_str, BASE_FRET_STR)) {
		if (!check_old_define_syntax(temp_str,chord_name)) 
	 		error("syntax error in chord definition: keyword <base-fret> missing");
		return(0); 
	}

	temp_str= strtok(NULL, DELIM_STR);
	if (temp_str== NULL) {
		error("syntax error in chord definition: no base fret value");
		return(0);
	}
	fret_displ= atoi(temp_str);
	if (fret_displ == 0) fret_displ = 1;

	temp_str= tolower_str(strtok(NULL, DELIM_STR));
	if (strcmp(temp_str, FRETS_STR)) {
		error("syntax error in chord definition: keyword <frets> missing");
		return(0);
	}

	for (n= 0; n< 6; n++)
		{
		strcpy(temp_str, toupper_str(strtok(NULL, DELIM_STR)));

		if (temp_str!= NULL) {
			if (!strcmp(temp_str, FRET_NONE_STR) ||
			    !strcmp(temp_str, FRET_X_STR))
				dot_array[n]= FRET_X;
			else {
				dot_array[n]= atoi(temp_str);
				if (dot_array[n] < 0) dot_array[n]= FRET_X;
			}
			hardtoplay= hardtoplay||(dot_array[n]>LONG_FINGERS);
			}
		else 
			{
			error("syntax error in chord definition : too few arguments");
			return(0);
			}
		}

	temp_str= strtok(NULL, DELIM_STR);
	if (temp_str!=NULL) {
		error("syntax error in chord definition: too many arguments");
		return(0);
	}

	/* CHORD is OK */
	if (hardtoplay)
		{
		sprintf(mesg,
		  "Warning: You'll need long fingers to play \"%s\" this way!", chord_name);
		error (mesg);
		}

	learn_chord(chord_name,
			dot_array[0],dot_array[1],dot_array[2],
			dot_array[3],dot_array[4],dot_array[5],
			fret_displ, in_chordrc ? CHORD_DEFINED : CHORD_IN_CHORDRC,
			CHORD_HARD);

	return(0);
}


/*--------------------------------------------------------------------------------*/
void init_known_chords()
{
	nb_known_chord= 0;

#define N FRET_X

	learn_chord("Ab",	 1, 3, 3, 2, 1, 1,	 4, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Ab+",	 N, N, 2, 1, 1, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Ab4",	 N, N, 1, 1, 2, 4,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Ab7",	 N, N, 1, 1, 1, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Ab11",	 1, 3, 1, 3, 1, 1,	 4, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Absus",	 N, N, 1, 1, 2, 4,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Absus4",	 N, N, 1, 1, 2, 4,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Abdim",	 N, N, 0, 1, 0, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Abmaj",	 1, 3, 3, 2, 1, 1,	 4, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Abmaj7",	 N, N, 1, 1, 1, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Abmin",	 1, 3, 3, 1, 1, 1,	 4, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Abm",	 1, 3, 3, 1, 1, 1,	 4, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Abm7",	 N, N, 1, 1, 1, 1,	 4, CHORD_BUILTIN, CHORD_HARD);

	learn_chord("A",	 N, 0, 2, 2, 2, 0,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("A+",	 N, 0, 3, 2, 2, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("A4",	 0, 0, 2, 2, 0, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("A6",	 N, N, 2, 2, 2, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("A7",	 N, 0, 2, 0, 2, 0,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("A7+",	 N, N, 3, 2, 2, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("A7(9+)",	 N, 2, 2, 2, 2, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("A9",	 N, 0, 2, 1, 0, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("A11",	 N, 4, 2, 4, 3, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("A13",	 N, 0, 1, 2, 3, 1,	 5, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("A7sus4",	 0, 0, 2, 0, 3, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("A9sus",	 N, 0, 2, 1, 0, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Asus",	 N, N, 2, 2, 3, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Asus2",	 0, 0, 2, 2, 0, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Asus4",	 N, N, 2, 2, 3, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Adim",	 N, N, 1, 2, 1, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Amaj",	 N, 0, 2, 2, 2, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Amaj7",	 N, 0, 2, 1, 2, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Adim",	 N, N, 1, 2, 1, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Amin",	 N, 0, 2, 2, 1, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("A/D",	 N, N, 0, 0, 2, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("A/F#",	 2, 0, 2, 2, 2, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("A/G#",	 4, 0, 2, 2, 2, 0,	 1, CHORD_BUILTIN, CHORD_HARD);

	learn_chord("Am",	 N, 0, 2, 2, 1, 0,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("Am#7",	 N, N, 2, 1, 1, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Am(7#)",	 N, 0, 2, 2, 1, 4,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Am6",	 N, 0, 2, 2, 1, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Am7",	 N, 0, 2, 2, 1, 3,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("Am7sus4",	 0, 0, 0, 0, 3, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Am9",	 N, 0, 1, 1, 1, 3,	 5, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Am/G",	 3, 0, 2, 2, 1, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Amadd9",	 0, 2, 2, 2, 1, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Am(add9)",	 0, 2, 2, 2, 1, 0,	 1, CHORD_BUILTIN, CHORD_HARD);

	learn_chord("A#",	 N, 1, 3, 3, 3, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("A#+",	 N, N, 0, 3, 3, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("A#4",	 N, N, 3, 3, 4, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("A#7",	 N, N, 1, 1, 1, 2,	 3, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("A#sus",	 N, N, 3, 3, 4, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("A#sus4",	 N, N, 3, 3, 4, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("A#maj",	 N, 1, 3, 3, 3, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("A#maj7",	 N, 1, 3, 2, 3, N,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("A#dim",	 N, N, 2, 3, 2, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("A#min",	 N, 1, 3, 3, 2, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("A#m",	 N, 1, 3, 3, 2, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("A#m7",	 N, 1, 3, 1, 2, 1,	 1, CHORD_BUILTIN, CHORD_HARD);

	learn_chord("Bb",	 N, 1, 3, 3, 3, 1,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("Bb+",	 N, N, 0, 3, 3, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Bb4",	 N, N, 3, 3, 4, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Bb6",	 N, N, 3, 3, 3, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Bb7",	 N, N, 1, 1, 1, 2,	 3, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Bb9",	 1, 3, 1, 2, 1, 3,	 6, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Bb11",	 1, 3, 1, 3, 4, 1,	 6, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Bbsus",	 N, N, 3, 3, 4, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Bbsus4",	 N, N, 3, 3, 4, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Bbmaj",	 N, 1, 3, 3, 3, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Bbmaj7",	 N, 1, 3, 2, 3, N,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Bbdim",	 N, N, 2, 3, 2, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Bbmin",	 N, 1, 3, 3, 2, 1,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("Bbm",	 N, 1, 3, 3, 2, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Bbm7",	 N, 1, 3, 1, 2, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Bbm9",	 N, N, N, 1, 1, 3,	 6, CHORD_BUILTIN, CHORD_HARD);

	learn_chord("B",	 N, 2, 4, 4, 4, 2,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("B+",	 N, N, 1, 0, 0, 4,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("B4",	 N, N, 3, 3, 4, 1,	 2, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("B7",	 0, 2, 1, 2, 0, 2,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("B7+",	 N, 2, 1, 2, 0, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("B7+5",	 N, 2, 1, 2, 0, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("B7#9",	 N, 2, 1, 2, 3, N,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("B7(#9)",	 N, 2, 1, 2, 3, N,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("B9",	 1, 3, 1, 2, 1, 3,	 7, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("B11",	 1, 3, 3, 2, 0, 0,	 7, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("B11/13",	 N, 1, 1, 1, 1, 3,	 2, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("B13",	 N, 2, 1, 2, 0, 4,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Bsus",	 N, N, 3, 3, 4, 1,	 2, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Bsus4",	 N, N, 3, 3, 4, 1,	 2, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Bmaj",	 N, 2, 4, 3, 4, N,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Bmaj7",	 N, 2, 4, 3, 4, N,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Bdim",	 N, N, 0, 1, 0, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Bmin",	 N, 2, 4, 4, 3, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("B/F#",	 0, 2, 2, 2, 0, 0,	 2, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("BaddE",	 N, 2, 4, 4, 0, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("B(addE)",	 N, 2, 4, 4, 0, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("BaddE/F#",	 2, N, 4, 4, 0, 0,	 1, CHORD_BUILTIN, CHORD_HARD);

	learn_chord("Bm",	 N, 2, 4, 4, 3, 2,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("Bm6",	 N, N, 4, 4, 3, 4,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Bm7",	 N, 1, 3, 1, 2, 1,	 2, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("Bmmaj7",	 N, 1, 4, 4, 3, N,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Bm(maj7)",	 N, 1, 4, 4, 3, N,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Bmsus9",	 N, N, 4, 4, 2, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Bm(sus9)",	 N, N, 4, 4, 2, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Bm7b5",	 1, 2, 4, 2, 3, 1,	 1, CHORD_BUILTIN, CHORD_HARD);

	learn_chord("C",	 N, 3, 2, 0, 1, 0,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("C+",	 N, N, 2, 1, 1, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("C4",	 N, N, 3, 0, 1, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("C6",	 N, 0, 2, 2, 1, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("C7",	 0, 3, 2, 3, 1, 0,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("C9",	 1, 3, 1, 2, 1, 3,	 8, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("C9(11)",	 N, 3, 3, 3, 3, N,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("C11",	 N, 1, 3, 1, 4, 1,	 3, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Csus",	 N, N, 3, 0, 1, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Csus2",	 N, 3, 0, 0, 1, N,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Csus4",	 N, N, 3, 0, 1, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Csus9",	 N, N, 4, 1, 2, 4,	 7, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Cmaj",	 0, 3, 2, 0, 1, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Cmaj7",	 N, 3, 2, 0, 0, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Cmin",	 N, 1, 3, 3, 2, 1,	 3, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Cdim",	 N, N, 1, 2, 1, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("C/B",	 N, 2, 2, 0, 1, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Cadd2/B",	 N, 2, 0, 0, 1, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("CaddD",	 N, 3, 2, 0, 3, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("C(addD)",	 N, 3, 2, 0, 3, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Cadd9",	 N, 3, 2, 0, 3, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("C(add9)",	 N, 3, 2, 0, 3, 0,	 1, CHORD_BUILTIN, CHORD_HARD);

	learn_chord("Cm",	 N, 1, 3, 3, 2, 1,	 3, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("Cm7",	 N, 1, 3, 1, 2, 1,	 3, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("Cm11",	 N, 1, 3, 1, 4, N,	 3, CHORD_BUILTIN, CHORD_HARD);

	learn_chord("C#",	 N, N, 3, 1, 2, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("C#+",	 N, N, 3, 2, 2, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("C#4",	 N, N, 3, 3, 4, 1,	 4, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("C#7",	 N, N, 3, 4, 2, 4,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("C#7(b5)",	 N, 2, 1, 2, 1, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("C#sus",	 N, N, 3, 3, 4, 1,	 4, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("C#sus4",	 N, N, 3, 3, 4, 1,	 4, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("C#maj",	 N, 4, 3, 1, 1, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("C#maj7",	 N, 4, 3, 1, 1, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("C#dim",	 N, N, 2, 3, 2, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("C#min",	 N, N, 2, 1, 2, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("C#add9",	 N, 1, 3, 3, 1, 1,	 4, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("C#(add9)",	 N, 1, 3, 3, 1, 1,	 4, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("C#m",	 N, N, 2, 1, 2, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("C#m7",	 N, N, 2, 4, 2, 4,	 1, CHORD_BUILTIN, CHORD_HARD);

	learn_chord("Db",	 N, N, 3, 1, 2, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Db+",	 N, N, 3, 2, 2, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Db7",	 N, N, 3, 4, 2, 4,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Dbsus",	 N, N, 3, 3, 4, 1,	 4, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Dbsus4",	 N, N, 3, 3, 4, 1,	 4, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Dbmaj",	 N, N, 3, 1, 2, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Dbmaj7",	 N, 4, 3, 1, 1, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Dbdim",	 N, N, 2, 3, 2, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Dbmin",	 N, N, 2, 1, 2, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Dbm",	 N, N, 2, 1, 2, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Dbm7",	 N, N, 2, 4, 2, 4,	 1, CHORD_BUILTIN, CHORD_HARD);

	learn_chord("D",	 N, N, 0, 2, 3, 2,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("D+",	 N, N, 0, 3, 3, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("D4",	 N, N, 0, 2, 3, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("D6",	 N, 0, 0, 2, 0, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("D7",	 N, N, 0, 2, 1, 2,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("D7#9",	 N, 2, 1, 2, 3, 3,	 4, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("D7(#9)",	 N, 2, 1, 2, 3, 3,	 4, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("D9",	 1, 3, 1, 2, 1, 3,	10, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("D11",	 3, 0, 0, 2, 1, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Dsus",	 N, N, 0, 2, 3, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Dsus2",	 0, 0, 0, 2, 3, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Dsus4",	 N, N, 0, 2, 3, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("D7sus2",	 N, 0, 0, 2, 1, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("D7sus4",	 N, 0, 0, 2, 1, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Dmaj",	 N, N, 0, 2, 3, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Dmaj7",	 N, N, 0, 2, 2, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Ddim",	 N, N, 0, 1, 0, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Dmin",	 N, N, 0, 2, 3, 1,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("D/A",	 N, 0, 0, 2, 3, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("D/B",	 N, 2, 0, 2, 3, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("D/C",	 N, 3, 0, 2, 3, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("D/C#",	 N, 4, 0, 2, 3, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("D/E",	 N, 1, 1, 1, 1, N,	 7, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("D/G",	 3, N, 0, 2, 3, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("D5/E",	 0, 1, 1, 1, N, N,	 7, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Dadd9",	 0, 0, 0, 2, 3, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("D(add9)",	 0, 0, 0, 2, 3, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("D9add6",	 1, 3, 3, 2, 0, 0,	10, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("D9(add6)",	 1, 3, 3, 2, 0, 0,	10, CHORD_BUILTIN, CHORD_HARD);

	learn_chord("Dm",	 N, N, 0, 2, 3, 1,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("Dm6(5b)",	 N, N, 0, 1, 0, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Dm7",	 N, N, 0, 2, 1, 1,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("Dm#5",	 N, N, 0, 3, 3, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Dm(#5)",	 N, N, 0, 3, 3, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Dm#7",	 N, N, 0, 2, 2, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Dm(#7)",	 N, N, 0, 2, 2, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Dm/A",	 N, 0, 0, 2, 3, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Dm/B",	 N, 2, 0, 2, 3, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Dm/C",	 N, 3, 0, 2, 3, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Dm/C#",	 N, 4, 0, 2, 3, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Dm9",	 N, N, 3, 2, 1, 0,	 1, CHORD_BUILTIN, CHORD_HARD);

	learn_chord("D#",	 N, N, 3, 1, 2, 1,	 3, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("D#+",	 N, N, 1, 0, 0, 4,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("D#4",	 N, N, 1, 3, 4, 4,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("D#7",	 N, N, 1, 3, 2, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("D#sus",	 N, N, 1, 3, 4, 4,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("D#sus4",	 N, N, 1, 3, 4, 4,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("D#maj",	 N, N, 3, 1, 2, 1,	 3, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("D#maj7",	 N, N, 1, 3, 3, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("D#dim",	 N, N, 1, 2, 1, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("D#min",	 N, N, 4, 3, 4, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("D#m",	 N, N, 4, 3, 4, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("D#m7",	 N, N, 1, 3, 2, 2,	 1, CHORD_BUILTIN, CHORD_HARD);

	learn_chord("Eb",	 N, N, 3, 1, 2, 1,	 3, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Eb+",	 N, N, 1, 0, 0, 4,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Eb4",	 N, N, 1, 3, 4, 4,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Eb7",	 N, N, 1, 3, 2, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Ebsus",	 N, N, 1, 3, 4, 4,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Ebsus4",	 N, N, 1, 3, 4, 4,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Ebmaj",	 N, N, 1, 3, 3, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Ebmaj7",	 N, N, 1, 3, 3, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Ebdim",	 N, N, 1, 2, 1, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Ebadd9",	 N, 1, 1, 3, 4, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Eb(add9)",	 N, 1, 1, 3, 4, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Ebmin",	 N, N, 4, 3, 4, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Ebm",	 N, N, 4, 3, 4, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Ebm7",	 N, N, 1, 3, 2, 2,	 1, CHORD_BUILTIN, CHORD_HARD);

	learn_chord("E",	 0, 2, 2, 1, 0, 0,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("E+",	 N, N, 2, 1, 1, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("E5",	 0, 1, 3, 3, N, N,	 7, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("E6",	 N, N, 3, 3, 3, 3,	 9, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("E7",	 0, 2, 2, 1, 3, 0,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("E7#9",	 0, 2, 2, 1, 3, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("E7(#9)",	 0, 2, 2, 1, 3, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("E7(5b)",	 N, 1, 0, 1, 3, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("E7b9",	 0, 2, 0, 1, 3, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("E7(b9)",	 0, 2, 0, 1, 3, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("E7(11)",	 0, 2, 2, 2, 3, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("E9",	 1, 3, 1, 2, 1, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("E11",	 1, 1, 1, 1, 2, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Esus",	 0, 2, 2, 2, 0, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Esus4",	 0, 2, 2, 2, 0, 0,	 0, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Emaj",	 0, 2, 2, 1, 0, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Emaj7",	 0, 2, 1, 1, 0, N,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Edim",	 N, N, 2, 3, 2, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Emin",	 0, 2, 2, 0, 0, 0,	 1, CHORD_BUILTIN, CHORD_HARD);

	learn_chord("Em",	 0, 2, 2, 0, 0, 0,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("Em6",	 0, 2, 2, 0, 2, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Em7",	 0, 2, 2, 0, 3, 0,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("Em/B",	 N, 2, 2, 0, 0, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Em/D",	 N, N, 0, 0, 0, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Em7/D",	 N, N, 0, 0, 0, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Emsus4",	 0, 0, 2, 0, 0, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Em(sus4)",	 0, 0, 2, 0, 0, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Emadd9",	 0, 2, 4, 0, 0, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Em(add9)",	 0, 2, 4, 0, 0, 0,	 1, CHORD_BUILTIN, CHORD_HARD);

	learn_chord("F",	 1, 3, 3, 2, 1, 1,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("F+",	 N, N, 3, 2, 2, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("F+7+11",	 1, 3, 3, 2, 0, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("F4",	 N, N, 3, 3, 1, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("F6",	 N, 3, 3, 2, 3, N,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("F7",	 1, 3, 1, 2, 1, 1,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("F9",	 2, 4, 2, 3, 2, 4,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("F11",	 1, 3, 1, 3, 1, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Fsus",	 N, N, 3, 3, 1, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Fsus4",	 N, N, 3, 3, 1, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Fmaj",	 1, 3, 3, 2, 1, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Fmaj7",	 N, 3, 3, 2, 1, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Fdim",	 N, N, 0, 1, 0, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Fmin",	 1, 3, 3, 1, 1, 1,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("F/A",	 N, 0, 3, 2, 1, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("F/C",	 N, N, 3, 2, 1, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("F/D",	 N, N, 0, 2, 1, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("F/G",	 3, 3, 3, 2, 1, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("F7/A",	 N, 0, 3, 0, 1, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Fmaj7/A",	 N, 0, 3, 2, 1, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Fmaj7/C",	 N, 3, 3, 2, 1, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Fmaj7(+5)", N, N, 3, 2, 2, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Fadd9",	 3, 0, 3, 2, 1, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("F(add9)",	 3, 0, 3, 2, 1, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("FaddG",	 1, N, 3, 2, 1, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("FaddG",	 1, N, 3, 2, 1, 3,	 1, CHORD_BUILTIN, CHORD_HARD);

	learn_chord("Fm",	 1, 3, 3, 1, 1, 1,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("Fm6",	 N, N, 0, 1, 1, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Fm7",	 1, 3, 1, 1, 1, 1,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("Fmmaj7",	 N, 3, 3, 1, 1, 0,	 1, CHORD_BUILTIN, CHORD_HARD);

	learn_chord("F#",	 2, 4, 4, 3, 2, 2,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("F#+",	 N, N, 4, 3, 3, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("F#7",	 N, N, 4, 3, 2, 0,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("F#9",	 N, 1, 2, 1, 2, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("F#11",	 2, 4, 2, 4, 2, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("F#sus",	 N, N, 4, 4, 2, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("F#sus4",	 N, N, 4, 4, 2, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("F#maj",	 2, 4, 4, 3, 2, 2,	 0, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("F#maj7",	 N, N, 4, 3, 2, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("F#dim",	 N, N, 1, 2, 1, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("F#min",	 2, 4, 4, 2, 2, 2,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("F#/E",	 0, 4, 4, 3, 2, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("F#4",	 N, N, 4, 4, 2, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("F#m",	 2, 4, 4, 2, 2, 2,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("F#m6",	 N, N, 1, 2, 2, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("F#m7",	 N, N, 2, 2, 2, 2,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("F#m7-5",	 1, 0, 2, 3, 3, 3,	 2, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("F#m/C#m",	 N, N, 4, 2, 2, 2,	 1, CHORD_BUILTIN, CHORD_HARD);

	learn_chord("Gb",	 2, 4, 4, 3, 2, 2,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("Gb+",	 N, N, 4, 3, 3, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Gb7",	 N, N, 4, 3, 2, 0,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("Gb9",	 N, 1, 2, 1, 2, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Gbsus",	 N, N, 4, 4, 2, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Gbsus4",	 N, N, 4, 4, 2, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Gbmaj",	 2, 4, 4, 3, 2, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Gbmaj7",	 N, N, 4, 3, 2, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Gbdim",	 N, N, 1, 2, 1, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Gbmin",	 2, 4, 4, 2, 2, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Gbm",	 2, 4, 4, 2, 2, 2,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("Gbm7",	 N, N, 2, 2, 2, 2,	 1, CHORD_BUILTIN, CHORD_HARD);

	learn_chord("G",	 3, 2, 0, 0, 0, 3,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("G+",	 N, N, 1, 0, 0, 4,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("G4",	 N, N, 0, 0, 1, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("G6",	 3, N, 0, 0, 0, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("G7",	 3, 2, 0, 0, 0, 1,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("G7+",	 N, N, 4, 3, 3, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("G7b9",	 N, N, 0, 1, 0, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("G7(b9)",	 N, N, 0, 1, 0, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("G7#9",	 1, 3, N, 2, 4, 4,	 3, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("G7(#9)",	 1, 3, N, 2, 4, 4,	 3, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("G9",	 3, N, 0, 2, 0, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("G9(11)",	 1, 3, 1, 3, 1, 3,	 3, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("G11",	 3, N, 0, 2, 1, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Gsus",	 N, N, 0, 0, 1, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Gsus4",	 N, N, 0, 0, 1, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("G6sus4",	 0, 2, 0, 0, 1, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("G6(sus4)",	 0, 2, 0, 0, 1, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("G7sus4",	 3, 3, 0, 0, 1, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("G7(sus4)",	 3, 3, 0, 0, 1, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Gmaj",	 3, 2, 0, 0, 0, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Gmaj7",	 N, N, 1, 2, 3, 4,	 2, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Gmaj7sus4", N, N, 0, 0, 1, 2,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Gmaj9",	 1, 1, 4, 1, 2, 1,	 2, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Gmin",	 1, 3, 3, 1, 1, 1,	 3, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("Gdim",	 N, N, 2, 3, 2, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Gadd9",	 1, 3, N, 2, 1, 3,	 3, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("G(add9)",	 1, 3, N, 2, 1, 3,	 3, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("G/A",	 N, 0, 0, 0, 0, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("G/B",	 N, 2, 0, 0, 0, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("G/D",	 N, 2, 2, 1, 0, 0,	 4, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("G/F#",	 2, 2, 0, 0, 0, 3,	 1, CHORD_BUILTIN, CHORD_HARD);

	learn_chord("Gm",	 1, 3, 3, 1, 1, 1,	 3, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("Gm6",	 N, N, 2, 3, 3, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("Gm7",	 1, 3, 1, 1, 1, 1,	 3, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("Gm/Bb",	 3, 2, 2, 1, N, N,	 4, CHORD_BUILTIN, CHORD_HARD);

	learn_chord("G#",	 1, 3, 3, 2, 1, 1,	 4, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("G#+",	 N, N, 2, 1, 1, 0,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("G#4",	 1, 3, 3, 1, 1, 1,	 4, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("G#7",	 N, N, 1, 1, 1, 2,	 1, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("G#sus",	 N, N, 1, 1, 2, 4,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("G#sus4",	 N, N, 1, 1, 2, 4,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("G#maj",	 1, 3, 3, 2, 1, 1,	 4, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("G#maj7",	 N, N, 1, 1, 1, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("G#dim",	 N, N, 0, 1, 0, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("G#min",	 1, 3, 3, 1, 1, 1,	 4, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("G#m",	 1, 3, 3, 1, 1, 1,	 4, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("G#m6",	 N, N, 1, 1, 0, 1,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("G#m7",	 N, N, 1, 1, 1, 1,	 4, CHORD_BUILTIN, CHORD_EASY);
	learn_chord("G#m9maj7",	 N, N, 1, 3, 0, 3,	 1, CHORD_BUILTIN, CHORD_HARD);
	learn_chord("G#m9(maj7)",N, N, 1, 3, 0, 3,	 1, CHORD_BUILTIN, CHORD_HARD);

#undef N
}

/*--------------------------------------------------------------------------------*/
int is_chord_known(chord)
char *chord;
{
        int i;

        for (i= 0;i < nb_known_chord;i++)
                if (!strcmp(chord,known_chords[i].chord_name))
			return (i);
	return(-1);
}

/*--------------------------------------------------------------------------------*/
void dump_fret(fretnum)
int fretnum;
{
	if (fretnum == FRET_X) printf("  %2s", FRET_X_STR);
	else printf("  %2d", fretnum);
}

/*--------------------------------------------------------------------------------*/
void dump_chords(PostScript)
int	PostScript;
{
	int i;
	int ptr,idx;

	if (PostScript)
	{
		init_values();
		init_ps();
		init_known_chords();
		do_title("CHORD CHART");
		do_subtitle("Generated with the CHORD program");
		do_subtitle("Copyright Martin.Leclerc@Sun.COM and Mario.Dorion@Sun.COM 1992-1993");
	}
	else
	{
		init_known_chords();
		printf("#	CHORD CHART\n");
		printf("#	Generated with the CHORD program\n");
		printf("#	Copyright Martin.Leclerc@Sun.COM and Mario.Dorion@Sun.COM 1992-1993\n");
		printf("#\n");
		printf("#\t\t\t\t\t       '------ strings ------`\n");
		printf("#Chord\t\t\t\t\t        E   A   D   G   B   E\n");
	}

	read_chordrc();
	for (i= 0;i < nb_known_chord;i++) {
		add_to_chordtab(known_chords[i].chord_name);
	}

	if (PostScript) 
		{
		draw_chords();
		do_end_of_page(TRUE);
        	printf ("%%%%Trailer\n");
        	printf ("%%%%Pages: %d 1\n", n_pages);
        	printf ("%%%%EOF\n");
		}
	else {
		ptr= first_ptr;
		while (ptr != -1) {
			idx= is_chord_known(chordtab[ptr].chord_name);
			printf("{define %s:\t\tbase-fret %2d\tfrets",
				known_chords[idx].chord_name, known_chords[idx].displ);
			dump_fret(known_chords[idx].s1);
			dump_fret(known_chords[idx].s2);
			dump_fret(known_chords[idx].s3);
			dump_fret(known_chords[idx].s4);
			dump_fret(known_chords[idx].s5);
			dump_fret(known_chords[idx].s6);
			printf("}");
			printf("\t%s\n",
				known_chords[idx].origin== CHORD_BUILTIN ? "" : "(local)");
			ptr = chordtab[ptr].next_ptr;
			}
		}
}
