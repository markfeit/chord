static char SccsId[] = "@(#)chord.c	3.5\t1 Nov. 1993";
static char copyright[] = "Copyright 1991-1993 by Martin Leclerc & Mario Dorion";


#include "chord.h"

static FILE *source_fd;
FILE *toc_file_fd = NULL;		/* optional table of contents file */

char
	text_line[MAXLINE],	/* Lyrics Buffer */
	chord[MAXTOKEN],	/* Buffer for the name of the chord */
	title1[MAXLINE];	/* Holds the first title line */

char
	source[MAXTOKEN],
	directive[MAXLINE];	/* Buffer for the directive */

char
	i_input;		/* Input line pointer */

char
	mesgbuf[MAXTOKEN],
	*mesg,
	*text_font, *rt_text_font, *rc_text_font, 	/* font for text */
	*chord_font, *rt_chord_font, *rc_chord_font, 	/* font for chord */
	*mono_font = MONOSPACED_FONT,	/* non-proprotional font for tabs */
	*current_file,
	*chord_line[MAXLINE],
	*toc_file,
	*command_name;

int
	c,			/* Current character in input file */
	i_chord,		/* Index to 'chord' */
	i_directive,		/* Index to 'directive' */
	i_text, 		/* Index to 'text_line' */
	in_chord,		/* Booleans indicating parsing status */
	lyrics_only = FALSE,
	dump_only = FALSE,
	in_tab = FALSE,
	postscript_dump = FALSE,
	auto_space = FALSE,	/* applies lyrics_only when no chords */
	left_foot_even = -1,	 /* 0 for even page numbers on the right */
				/* 1 for even page numbers on the left */
	number_all = FALSE,	/* number the first page (set with -p 1) */
	no_grid, rt_no_grid, rc_no_grid,
	page_label = 1,		/* page number for footers and index */
	i_chord_ov,		/* Overflow Booleans */
	i_directive_ov = FALSE,
	i_text_ov = FALSE,
	in_directive = FALSE,
	in_chordrc = FALSE,
	first_time_in_chordrc = TRUE,
	in_chorus = FALSE,
	has_directive = FALSE,
	has_chord = FALSE,
	title1_found = FALSE,
	debug_mode = FALSE,
	need_soc = FALSE,
	pagination = 1,		/* virtual pages per physical pages */
	transpose = 0,		/* transposition value */
	vpos,			/* Current PostScript position, in points */
	hpos,
	start_of_chorus,	/* Vertical positions of the chorus */
	end_of_chorus,
	chord_size, rt_chord_size, rc_chord_size,  	/* font size for 'chord_font' */
	cur_text_size = 0,
	text_size, rt_text_size, rc_text_size,  	/* font size for 'text_font' */
	grid_size, rt_grid_size, rc_grid_size,
	no_easy_grids = FALSE,
	n_pages = 1,		/* total page counter */
	v_pages = 1,		/* virtual pages */
	n_lines = 1,		/* line number in input file */
	song_pages = 1,		/* song page counter */
	blank_space = 0;	/* consecutive blank line counter */

float
	chord_inc,
	scale = 1.0,		/* Current scale factor */
	rotation = 0.0;		/* Current rotation */

extern int nb_chord, first_ptr;
extern struct chord_struct chordtab[MAX_CHORD];

extern char *optarg;
extern int optind, opterr;


/* --------------------------------------------------------------------------------*/
void ps_fputc(fd, c)
FILE *fd;
int c;
        {
        if (c >128)
		{
                fprintf (fd, "\\%o", c);
		}
        else 
		{
		if ( c == ')' || c == '(' )
               		fprintf (fd, "\\%c", c);
		else
			fprintf (fd, "%c",c);
		}
        }
/* --------------------------------------------------------------------------------*/
void ps_fputs(fd, string)
FILE *fd;
char *string;
        {
        int i;

	/* sprintf (mesg, "ps_fputs:%s ",string);
	debug(mesg); */

        for (i= 0; string[i] != '\0'; i++)
                ps_fputc (fd, string[i]);

        }


/* --------------------------------------------------------------------------------*/
void ps_puts(string)
char *string;
	{
	ps_fputs(stdout, string);
	}
/* --------------------------------------------------------------------------------*/
void set_text_font(size)
int size;
	{
#define	MONO_SIZE_DECR	2	/* TABsize is smaller by this nb of points */
	if (( size != cur_text_size))
		{
		printf ("/TEXT_FONT { /%s findfont %d scalefont } def\n", text_font, size);
		re_encode (text_font);
		printf ("/MONO_FONT { /%s findfont %d scalefont } def\n",
			mono_font, size - MONO_SIZE_DECR);
		re_encode (mono_font);
		cur_text_size= size;
		/* sprintf(mesg, "Changing text size to %d", size);
		debug (mesg); */
		}
#undef MONO_SIZE_DECR
	}

/* --------------------------------------------------------------------------------*/
void use_text_font()
	{
	if (! in_tab) printf ("TEXT_FONT setfont\n");
	else printf ("MONO_FONT setfont\n");
	}
/* --------------------------------------------------------------------------------*/
void use_mono_font()
	{
	printf ("MONO_FONT setfont\n");
	}
/* --------------------------------------------------------------------------------*/
void do_translate(vert, horiz)
float vert, horiz;
	{
	printf ("%f %f translate\n", vert , horiz );
	/* debug ("changing translation"); */
	}
/* --------------------------------------------------------------------------------*/
void do_start_of_page()
/*logical page ! */
	{
	v_pages++;

	if (v_pages == 1)
		{
		n_pages++;
		page_label++;
		printf ("%%%%Page: \"%d\" %d\n",n_pages, n_pages);
		printf ("%%%%BeginPageSetup\n");
		printf ("/pgsave save def\n");
		printf ("%f %f scale\n", scale, scale);
		printf ("%f rotate\n",rotation);
		printf ("%%%%EndPageSetup\n");
		}

	/* Ugly stuff ahead!
		The translate factors could be derived from page-size and
		page-scale information from chord.h ... 
		I did not feel like working out the maths right now!
	*/
	if (pagination== 4) {
#ifdef US
		if (v_pages== 1) do_translate(0.0, 850.0);
		else if (v_pages== 2) do_translate(600.0, 0.0);
		else if (v_pages== 3) do_translate(-600.0, -800.0);
		else if (v_pages== 4) do_translate(600.0, 0.0);
#else
		if (v_pages== 1) do_translate(0.0, 910.0);
		else if (v_pages== 2) do_translate(600.0, 0.0);
		else if (v_pages== 3) do_translate(-600.0, -870.0);
		else if (v_pages== 4) do_translate(600.0, 0.0);
#endif
	}

	if (pagination== 2) {
#ifdef US
		if (v_pages== 1) do_translate(-40.0, -800.0);
		else if (v_pages== 2) do_translate(500.0, 0.0);
#else
		if (v_pages== 1) do_translate(-40.0, -850.0);
		else if (v_pages== 2) do_translate(600.0, 0.0);
#endif
	}

	vpos = TOP;
	hpos= L_MARGIN;
	song_pages++;
	if ( in_chorus )
		{
		start_of_chorus = vpos;
		}
	}

/* --------------------------------------------------------------------------------*/
void build_ps_toc()
	{
	char line[MAXTOKEN];

	/*debug("Debut de build_ps_toc");*/

	fclose(toc_file_fd);
	toc_file_fd= fopen(toc_file,"r");

	strcpy (title1, "Index");

	if (v_pages % pagination)
		do_end_of_page(TRUE);
				
	if (pagination == 4) 
	{
		pagination = 1;
		scale = 1.0;
	}

	v_pages=0;
	do_start_of_page();
	number_all= FALSE;
	song_pages= 0;
	set_text_font (text_size+10);
	use_text_font();
	printf ("(");
	ps_puts (&title1[0]);
	printf (") dup stringwidth pop 2 div\n");
	printf ("%d 2 div exch sub %d moveto\n", WIDTH, vpos);
	printf ("show\n");
	advance(text_size);

	while (fgets(line, MAXTOKEN, toc_file_fd) != NULL)
		{
		/* sprintf(mesg, "bpt:\"%s\"", line);
		debug(mesg); */

		if (line[0] == '$')
			{
			if ( vpos < BOTTOM + 3 * text_size)
				{
				advance (vpos);
				song_pages= 0;
				}
			advance(text_size+8);
			set_text_font(text_size + 5);
			use_text_font();
			printf("72 %d moveto\n", vpos);
			printf("(");
			ps_puts(&line[1]);
			printf(") show\n");
			fgets(line, MAXTOKEN, toc_file_fd);
			printf("500 %d moveto\n", vpos);
			printf("(%d) show\n", atoi(line));
			}
		else
			{
			advance(text_size);
			set_text_font(text_size);
			use_text_font();
			printf ("108 %d moveto\n", vpos);
			printf("(");
			ps_puts(line);
			printf(") show\n");
			}
		}

	vpos = vpos - text_size - 5;
	set_text_font (text_size);
	unlink (toc_file);
	}

/* --------------------------------------------------------------------------------*/
void use_chord_font()
	{
	printf ("CHORD_FONT setfont\n");
	}

/* --------------------------------------------------------------------------------*/
void set_chord_font()
	{
	printf ("/CHORD_FONT { /%s findfont %d scalefont } def\n", chord_font, chord_size);
	re_encode (chord_font);
	}

/* --------------------------------------------------------------------------------*/
void do_help (command) 
char *command;
	{
	fprintf (stderr, "Usage: %s [options] file [file ...]\n", command);
	fprintf (stderr, "Options:\n");
	fprintf (stderr, "	-A                 : About CHORD...\n");
	fprintf (stderr, "	-a                 : Automatic single space lines without chords\n");
	fprintf (stderr, "	-c n               : Set chord size [9]\n");
	fprintf (stderr, "	-C postscript_font : Set chord font\n");
	fprintf (stderr, "	-D                 : Dumps chords definitions (PostScript)\n");
	fprintf (stderr, "	-d                 : Dumps chords definitions (Text)\n");
	fprintf (stderr, "	-G                 : Disable printing of chord grids\n");
	fprintf (stderr, "	-g                 : Don't print grids for builtin \"easy\" chords.\n");
	fprintf (stderr, "	-h                 : This message\n");
	fprintf (stderr, "	-i                 : Generates a table of contents\n");
	fprintf (stderr, "	-L                 : Even pages numbers on left\n");
	fprintf (stderr, "	-l                 : Only print lyrics\n");
	fprintf (stderr, "	-o filename        : Saves the output to file\n");
	fprintf (stderr, "	-p n               : Starting page number [1]\n");
	fprintf (stderr, "	-s n               : Set chord grid size [30]\n");
	fprintf (stderr, "	-t n               : Set text size [12]\n");
	fprintf (stderr, "	-T postscript_font : Set text font\n");
	fprintf (stderr, "	-V                 : Print version and patchlevel\n");
	fprintf (stderr, "	-x n               : Transpose by 'n' half-tones\n");
	fprintf (stderr, "	-2                 : 2 pages per sheet\n");
	fprintf (stderr, "	-4                 : 4 pages per sheet\n");

	exit(0);
	}

/* --------------------------------------------------------------------------------*/
void do_about ()
	{
	printf("CHORD: A lyrics and chords formatting program.\n");
	printf("===== \n");
	printf("\n");;
	printf("CHORD will read an ASCII file containing the lyrics of one or many\n");
	printf("songs plus chord information. CHORD will then generate a photo-ready,\n");
	printf("professional looking, impress-your-friends sheet-music suitable for printing\n");
	printf("on your nearest PostScript printer.\n");
	printf("\n");
	printf("To learn more about CHORD, look for the man page or do \"chord -h\" for\n");
	printf("the list of options.\n");
	printf("\n");
	printf("			--0--\n");
	printf("\n");
	printf("Copyright (C) 1991-1993 by Martin Leclerc & Mario Dorion\n");
	printf("\n");
	printf("This program is free software; you can redistribute it and/or modify\n");
	printf("it under the terms of the GNU General Public License as published by\n");
	printf("the Free Software Foundation; either version 2 of the License, or\n");
	printf("(at your option) any later version.\n");
	printf("\n");
	printf("This program is distributed in the hope that it will be useful,\n");
	printf("but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
	printf("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
	printf("GNU General Public License for more details.\n");
	printf("\n");
	printf("You should have received a copy of the GNU General Public License\n");
	printf("along with this program; if not, write to the Free Software\n");
	printf("Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n");
	printf("\n");
	printf("Send all questions, comments and bug reports to the original authors:\n");

		printf("	Martin.Leclerc@Sun.COM and Mario.Dorion@Sun.COM\n");
	printf("\n");
	}

/* --------------------------------------------------------------------------------*/
void do_comment(comment, style)
char 	*comment;
int	style;
	{
	for (; *comment == ' '; comment++); /* strip leading blanks */
	advance(blank_space);
	blank_space = 0;
	advance(text_size);
	text_line[i_text] = '\0';
	use_text_font();

	if (style == 1) {
		printf (".9  setgray\n");
		printf ("%d  setlinewidth\n", text_size);
		printf ("newpath\n");
		printf ("%d %d moveto\n", hpos - 2, vpos + text_size/2 - 2);
		printf ("(");
		ps_puts(comment);
		printf (" ) stringwidth rlineto\n");
		printf ("stroke\n");
		printf ("%d %d moveto\n", hpos, vpos);
		printf ("0  setgray\n");
		printf ("1 setlinewidth\n");
		printf ("(");
		ps_puts(comment);
		printf (") show\n");
	}
	else if (style == 2) {
		printf ("/");
		ps_puts (chord_font);
		printf (" findfont %d scalefont setfont\n",text_size);
		printf ("%d %d moveto\n", hpos, vpos);
		printf ("(");
		ps_puts(comment);
		printf (") show\n");
		use_text_font();
	}
	else if (style == 3) {
		printf ("newpath\n");
		printf ("%d %d moveto\n",hpos + 2, vpos - 2);
		printf ("(");
		ps_puts(comment);
		printf (") stringwidth /vdelta exch def /hdelta exch def\n");
		printf ("hdelta vdelta rlineto 0 %d rlineto hdelta neg 0 rlineto closepath\n",
				text_size);
		printf ("stroke\n");
		printf ("%d %d moveto\n", hpos, vpos);
		printf ("0  setgray\n");
		printf ("(");
		ps_puts(comment);
		printf (") show\n");
	}
	
	i_text = 0;
	}
/* --------------------------------------------------------------------------------*/
void do_chorus_line()
	{
	printf ("1  setlinewidth\n");
	printf ("newpath\n");
	printf ("%d %d moveto\n", L_MARGIN - 10, start_of_chorus);
	printf ("0 %d rlineto\n", -(start_of_chorus - end_of_chorus));
	printf ("closepath\n");
	printf ("stroke\n");
	}
/* --------------------------------------------------------------------------------*/
void do_chord (i_text, chord)
int i_text;
char *chord;
	{
	int j;
	if ((transpose != 0) && (strcmp(toupper_str(chord), NO_CHORD_STR)))
		if (do_transpose (chord) != 0)
			{
			sprintf (mesg, "Don't know how to transpose [%s]", chord);
			error (mesg);
			}
			
	for (j= i_text; chord_line[j] != NULL; j++);
		
	if (j < MAXLINE)
		if (strcmp(toupper_str(chord), NO_CHORD_STR)) 
			chord_line[j] = chordtab[add_to_chordtab(chord)].chord_name;
		else	chord_line[j] = NO_CHORD_STR;
	
	}

/* --------------------------------------------------------------------------------*/
void print_chord_line ()
	{
	int i, j;		/* Counter */
	int min_ascii= -1;
	
	printf ("/minhpos 0 def\n");

	for (j= 0; j<MAXLINE; j++)
		{
		if (chord_line[j] != NULL )
			{
			use_text_font();

			printf ("(" ); 
			for (i= 0; (i<j) && (text_line[i] != '\0');
				ps_fputc (stdout,text_line[i++]));

			printf (") stringwidth  pop %d add \n", hpos);
			printf ("dup minhpos lt\n");
			printf ("     {pop minhpos} if\n");
			printf ("dup /minhpos exch (");
			ps_puts(chord_line[j]);
			printf (") stringwidth pop add def\n");
			printf ("%d moveto\n",vpos);


			use_chord_font();
			printf ("(");
			ps_puts(chord_line[j]);
			printf (") show\n");
		
			chord_line[j]= NULL;
			}
		}
	}

/* --------------------------------------------------------------------------------*/
void init_ps()
	{
	printf ("%%!PS-Adobe-1.0\n");
	printf ("%%%%Title: A song\n");
	printf ("%%%%Creator: Martin Leclerc & Mario Dorion\n");
	printf ("%%%%Pages: (atend)\n");
	printf ("%%%%BoundingBox: 5 5 605 787\n");
	printf ("%%%%EndComments\n");
	printf ("/inch {72 mul } def\n");

	print_re_encode();
	set_chord_font();
	set_text_font(text_size);
	do_init_grid_ps();

	printf ("%%%%EndProlog\n");

	printf ("%%%%Page: \"%d\" %d\n",n_pages, n_pages);
	printf ("%%%%BeginPageSetup\n");
	printf ("/pgsave save def\n");
	printf ("%f %f scale\n", scale, scale);
	printf ("%f rotate\n", rotation);
	printf ("%%%%EndPageSetup\n");

	vpos = TOP;
	hpos  = L_MARGIN;

#ifdef US
	if (pagination== 4) do_translate (0.0,   850.0);
	else if (pagination== 2) do_translate (-40.0, -800.0);
#else
	if (pagination== 4) do_translate (0.0,   910.0);
	else if (pagination== 2) do_translate (-40.0, -850.0);
#endif

	}


/* --------------------------------------------------------------------------------*/
void do_end_of_phys_page()
/* physical */
	{

	/* Note: Processing of page number in 2-up mode is an UGLY KLUDGE! */
	/* 				Mario */
	/*debug ("end_of_phys_page");*/

	if (pagination != 2)
		printf ("pgsave restore\n");

	if (number_all)
		{
		int pnum = (number_all ? page_label : song_pages);
		set_text_font(DEF_TEXT_SIZE - 2);
		use_text_font();
		if (page_label % 2 == left_foot_even) {  /* left side */
			printf ("1 inch %d 3 div moveto\n", BOTTOM); 
			if (pagination == 2)
#ifdef US
				printf ("-500 0 rmoveto\n");
#else
				printf ("-600 0 rmoveto\n");
#endif
			printf ("(Page %d)\n", pnum);
		} else {                                /* right side */
			printf ("(Page %d) dup stringwidth pop\n", pnum);
			printf ("%d exch sub 1 inch sub %d 3 div moveto\n",
				WIDTH, BOTTOM); 
		}
		if (pagination == 2)
			printf ("1 %f div dup scale\n",scale2); 
		printf ("show\n");
	}

	if (pagination == 2)
		printf ("pgsave restore\n");


	printf ("showpage\n");
	printf ("%%%%EndPage: \"%d\" %d\n",n_pages, n_pages);
	}

/* --------------------------------------------------------------------------------*/
void do_end_of_page(force_physical)
int	force_physical;
/* Logical */
	{
	printf ("1  setlinewidth\n");
	printf ("0  setgray\n");
	printf ("newpath\n");
	printf ("%d %d 10 sub moveto\n", L_MARGIN, BOTTOM); 
	printf ("%d 0 rlineto\n", WIDTH - L_MARGIN * 2);
	printf ("stroke\n");

	if ((song_pages > 1) && (v_pages == 1) && title1_found) 
		{
		printf ("(");
		ps_puts(&title1[0]);
		printf (") dup stringwidth pop 2 div\n");
		printf ("%d 2 div exch sub %d 3 div moveto\n",
			WIDTH, BOTTOM);
		printf ("show\n");
		set_text_font(text_size);
		}

	if ( in_chorus )
		{
		end_of_chorus = vpos;
		do_chorus_line();
		}

	if ((v_pages == pagination) || force_physical) 
		{
		do_end_of_phys_page();
		v_pages = 0;
		}
	}

/* --------------------------------------------------------------------------------*/
void do_end_of_song()
{
	if ((! lyrics_only && ! no_grid)) draw_chords();
	do_end_of_page(FALSE);
}
		 
/* --------------------------------------------------------------------------------*/
void set_sys_def()
	{
	text_size= DEF_TEXT_SIZE;
	chord_size= DEF_CHORD_SIZE;
	grid_size = DEF_GRID_SIZE;
	text_font = DEF_TEXT_FONT;
	chord_font = DEF_CHORD_FONT;
	text_font = DEF_TEXT_FONT;
	no_grid = FALSE;
	}

/* --------------------------------------------------------------------------------*/
void set_rc_def()
	{
	read_chordrc();
	if (rc_text_size != 0) text_size = rc_text_size;
	if (rc_chord_size != 0) chord_size =  rc_chord_size;
	if (rc_grid_size != 0) grid_size =  rc_grid_size;
	if (rc_no_grid != 0) no_grid =  rc_no_grid;
	if (rc_text_font != NULL) text_font =  rc_text_font;
	if (rc_chord_font != NULL) chord_font =  rc_chord_font;
	}

/* --------------------------------------------------------------------------------*/
void set_rt_def()
	{
	if (rt_text_size != 0) text_size = rt_text_size;
	if (rt_chord_size != 0) chord_size =  rt_chord_size;
	if (rt_grid_size != 0) grid_size =  rt_grid_size;
	if (rt_no_grid != 0) no_grid =  rt_no_grid;
	if (rt_text_font != NULL) text_font =  rt_text_font;
	if (rt_chord_font != NULL) chord_font =  rt_chord_font;
	}

/* --------------------------------------------------------------------------------*/
void init_values()
	{
	set_sys_def();
	set_rc_def();
	set_rt_def();
	}

/* --------------------------------------------------------------------------------*/
void do_new_song()
	{
	do_end_of_song();
	nb_chord= first_ptr= 0;
	song_pages = 0;
	in_tab = FALSE;
	strcpy (title1, "");
	do_start_of_page();
	init_known_chords();

	/* reset default */
	init_values();

	set_text_font(text_size);
	}
/* --------------------------------------------------------------------------------*/
void advance(amount)
int amount;
	{
	vpos = vpos - amount;     /* Affect text positionning ! */
	if (vpos < BOTTOM )
		{
		do_end_of_page(FALSE);
		do_start_of_page();
		}
	}

/* --------------------------------------------------------------------------------*/
void print_text_line()
	{
	int i;

	text_line[i_text] = '\0';

	for (i= 0; text_line[i] == ' '; i++);

	if (!((auto_space || in_tab)  && !has_chord))
		{
		advance(blank_space);
		blank_space = 0;
		advance (chord_size + 1);

		if ( ( text_line[i] != '\0' )
		  && ( vpos - text_size <= BOTTOM))
			advance (text_size);

		if (need_soc)
			{
			start_of_chorus = vpos + chord_size;
			need_soc = FALSE;
			}
		if (! lyrics_only)
			print_chord_line();
		}

	if ( text_line[i] == '\0')
		{
		blank_space += text_size - 2;
		}
	else
		{
		advance (blank_space);
		blank_space = 0;
		advance (text_size - 1);
		if (need_soc)
			{
			start_of_chorus = vpos + text_size;
			need_soc = FALSE;
			}
		use_text_font();
		printf ("%d %d moveto\n", hpos, vpos);
		printf ("(");
		ps_puts(&text_line[0]);
		printf (") show\n");
		}

	i_text = 0;
	i_text_ov = FALSE;
	hpos = L_MARGIN;
	has_chord = FALSE;
	}

/* --------------------------------------------------------------------------------*/
void do_title(title)
char	*title;
	{

	set_text_font (text_size+5);
	use_text_font();
	printf ("(");
	ps_puts(title);
	printf (") dup stringwidth pop 2 div\n");
	printf ("%d 2 div exch sub %d moveto\n", WIDTH, vpos);
	printf ("show\n");
	vpos = vpos - text_size - 5;
	strcpy (&title1[0], title);
	title1_found = TRUE;
	set_text_font (text_size);
	if (toc_file_fd && song_pages == 1)	/* generate index entry */
		fprintf(toc_file_fd, "$%s\n%d\n", title, page_label);
	}
/* --------------------------------------------------------------------------------*/
void do_subtitle(sub_title)
char	*sub_title;
	{
	use_text_font();
	printf ("(");
	ps_puts(sub_title);
	printf (") dup stringwidth pop 2 div\n");
	printf ("%d 2 div exch sub %d moveto\n", WIDTH , vpos);
	printf ("show\n");
	vpos = vpos - text_size ;
	if (toc_file_fd && song_pages == 1)	/* generate index entry */
		fprintf(toc_file_fd, "%s\n", sub_title);
	}
	

/* --------------------------------------------------------------------------------*/
void do_directive(directive)
char *directive;
	{
	int   i;
	char  *command, *comment;

	command= tolower_str(strtok(directive, ": "));

	if (!strcmp(command, "start_of_chorus") || !strcmp(command,"soc"))
		{
		/* start_of_chorus = vpos - blank_space; */
		need_soc = TRUE;
		in_chorus = TRUE;
		}
	else if (!strcmp (command, "end_of_chorus") || !strcmp(command, "eoc"))
		{
		if ( in_chorus )
			{
			end_of_chorus = vpos;
			do_chorus_line();
			in_chorus = FALSE;
			}
		else
			error ("Not in a chorus.");
		}
	else if (!strcmp (command, "textfont") || !strcmp (command, "tf"))
		{
		if (in_chordrc) rc_text_font = strtok(NULL, ": ");
		else		   text_font = strtok(NULL, ": ");
		}
	else if (!strcmp (command, "chordfont") || !strcmp (command, "cf"))
		{
		if (in_chordrc) rc_chord_font = strtok(NULL, ": ");
		else
			{
			chord_font = strtok(NULL, ": ");
			set_chord_font();
			}
		}
	else if (!strcmp (command, "chordsize") || !strcmp (command, "cs"))
		{
		i = atoi(strtok(NULL, ": "));
		if ( i == 0 )
			error ("invalid value for chord_size");
		else
			if (in_chordrc) rc_chord_size = i;
			else
				{
				chord_size = i;
				set_chord_font();
				}
		}
	else if (!strcmp (command, "textsize") || !strcmp (command, "ts"))
		{
		i = atoi(strtok(NULL, ": "));
		if ( i == 0 )
			error ("invalid value for text_size");
		else
			if (in_chordrc) rc_text_size = i;
			else
				{
				text_size = i;
				set_text_font (text_size);
				}
		}
	else if (!strcmp (command, "comment") || !strcmp (command, "c"))
		{
		comment = strtok(NULL, "\0");
		do_comment(comment, 1);
		}
	else if (!strcmp (command, "comment_italic") || !strcmp (command, "ci"))
		{
		comment = strtok(NULL, "\0");
		do_comment(comment, 2);
		}
	else if (!strcmp (command, "comment_box") || !strcmp (command, "cb"))
		{
		comment = strtok(NULL, "\0");
		do_comment(comment, 3);
		}
	else if (!strcmp(command, "new_song") || !strcmp(command,"ns"))
		{
		do_new_song();
		}
	else if (!strcmp(command, "title") || !strcmp(command,"t"))
		{
		do_title(strtok(NULL, "\0"));
		}  
	else if (!strcmp(command, "subtitle") || !strcmp(command,"st"))
		{
		do_subtitle(strtok(NULL, "\0"));
		}  
	else if (!strcmp(command, "define") || !strcmp(command,"d"))
		{
		do_define_chord();
		}  
	else if (!strcmp(command, "no_grid") || !strcmp(command,"ng"))
		{
		if (in_chordrc) rc_no_grid = TRUE;
		else 		   no_grid = TRUE;
		}
	else if (!strcmp(command, "grid") || !strcmp(command,"g"))
		{
		if (in_chordrc) rc_no_grid = FALSE;
		else 		   no_grid = FALSE;
		}
	else if (!strcmp(command, "new_page") || !strcmp(command,"np"))
		{
		do_end_of_page(FALSE);
		do_start_of_page();
		}
	else if (!strcmp(command, "start_of_tab") || !strcmp(command,"sot"))
		{
		in_tab = TRUE;
		}
	else if (!strcmp(command, "end_of_tab") || !strcmp(command,"eot"))
		{
		in_tab = FALSE;
		}
	else if (!strcmp(command, "new_physical_page") || !strcmp(command,"npp"))
		{
		do_end_of_page(TRUE);
		do_start_of_page();
		}
	else
		{
		sprintf (mesg, "Invalid Directive : [%s]", command);
		error(mesg);
		has_directive = FALSE;
		}
	}

/* --------------------------------------------------------------------------------*/
void put_in_string(array, p_index, c, max_index, p_ov_flag)
char array[MAXLINE];
int *p_index;
int c;
int max_index;
int *p_ov_flag;
	{
	if (*p_index < max_index)
		array[(*p_index)++] = (char) c;
	else
		{
		if (!*p_ov_flag)
			{
			error ("Buffer Overflow");
			*p_ov_flag = TRUE;
			}
		}
	}
/* --------------------------------------------------------------------------------*/
void do_eol()
{
if ( in_directive )
	error ("Line ends while in a directive !"); 
if ( in_chord )
	error ("Line ends while in a chord !"); 
if (has_directive == FALSE)
	{
	if (in_chordrc) {
		if (strcmp(text_line, "\0"))
			error("line is NOT a directive");
			}
	else if (! in_tab || ! lyrics_only)
			print_text_line();
	}
else
	has_directive = FALSE;
n_lines++;
i_input = 0;
in_directive = FALSE;
in_chord = FALSE;
i_text = 0;
i_text_ov = FALSE;
text_line[0]='\0';
}
/* --------------------------------------------------------------------------------*/
void process_file(source_fd)
FILE *source_fd;
	{
	/*debug("start of process_file");*/

	n_lines = 0;

	while ( (c= getc(source_fd)) != EOF)
		{
		i_input++;
		switch ((char)c) {

		case '[':
			if (! in_tab) {
				if ( in_chord )
					error("Opening a chord within a chord!");
				else
					in_chord = TRUE;
				i_chord = 0;
			}
			else put_in_string(text_line, &i_text, c, MAXLINE, &i_text_ov);
			break;
		
		case ']':
			if (! in_tab) {
				if ( in_chord )
					{
					in_chord = FALSE;
					chord[i_chord]= '\0';
					do_chord(i_text, &chord[0]);
					has_chord = TRUE; 
					i_chord = 0;
					i_chord_ov = FALSE;
					}
				else
					error("']' found with no matching '['");
			}
			else put_in_string(text_line, &i_text, c, MAXLINE, &i_text_ov);
			break;
		
		case '{':
			in_directive = TRUE;
			i_directive = 0;
			has_directive = TRUE;
			break;

		case '}':
			if ( in_directive)
				{
				in_directive = FALSE;
				directive[i_directive]= '\0';
				for (; (c= getc(source_fd)) != '\n'; );
				i_input = 0;
				do_directive(&directive[0]);
				has_directive = FALSE;
				n_lines++;
				i_directive = 0;
				i_directive_ov = FALSE;
				}
			else
				error("'}' found with no matching '{'");
			break;

		case '\n':
			do_eol();
			break;
		case '(':
		case ')':
			if (in_directive)
				{
				put_in_string(directive, &i_directive, c, MAXTOKEN, &i_directive_ov);
				break;
				}
			else if (in_chord) /* allow parens in chord names */
				{
				put_in_string (chord, &i_chord, c, MAXLINE, &i_text_ov);
				break;
				}
			else
				{
				put_in_string (text_line, &i_text, c, MAXLINE, &i_text_ov);
				break;
				}
	
	/* This case HAS to be the last before the default statement !!! */

		case '#':
			if (i_input == 1)
				{
				for (; (c= getc(source_fd)) != '\n'; );
				n_lines++;
				i_input = 0;
				break;
				}

		default :
			if (in_chord )
				{
				if ( c != ' ' )
					put_in_string(chord, &i_chord, c, MAXTOKEN, &i_text_ov);
				}
			else if (in_directive)
				{
				put_in_string(directive, &i_directive, c, MAXTOKEN, &i_directive_ov);
				}
			else
				{
				put_in_string(text_line, &i_text, c, MAXLINE, &i_text_ov);
				}
			break;
			}
		}
	if (i_input != 0 ) do_eol();
	if (! in_chordrc) print_text_line();
	}

/* --------------------------------------------------------------------------------*/
/* read the file $HOME/.chordrc as a set of directive */
void read_chordrc()
	{
	char chordrc[MAXTOKEN];
	FILE *chordrc_fd;
	int n_lines_save;

	strcpy (chordrc, getenv ("HOME"));
	strcat (chordrc,"/.chordrc\0");
	current_file = chordrc;
	chordrc_fd = fopen (chordrc, "r");
	if (chordrc_fd != NULL)
		{
		n_lines_save= n_lines;
		n_lines= 1;
		in_chordrc = TRUE;
		process_file(chordrc_fd);
		in_chordrc = FALSE;
		n_lines= n_lines_save;
		fclose(chordrc_fd);
		}
	current_file = &source[0];
	first_time_in_chordrc = FALSE;
	}


/* --------------------------------------------------------------------------------*/
main(argc, argv)
int argc;
char **argv;
	{
	int c,i;

	mesg = mesgbuf;
/* Option Parsing */

	command_name= argv[0];

	while ((c = getopt(argc, argv, "aAc:C:dDgGhilLo:p:s:t:T:Vx:24")) != -1)
		switch (c) {

		case 'h':
			do_help(argv[0]);
			break;

		case 'd':
			dump_only = TRUE;
			break;

		case 'D':
			dump_only = TRUE;
			postscript_dump = TRUE;
			break;

		case 'c':
			i = atoi (optarg);
			if ( i == 0 )
				error_rt("invalid value for chord_size");
			else
				rt_chord_size = i;
			break;

		case 't':
			i = atoi (optarg);
			if ( i == 0 )
				error_rt("invalid value for text_size");
			else
				rt_text_size= i;
			break;

		case 'x':
			i = atoi (optarg);
			if ( i == 0 )
				error_rt("invalid value for transposition");
			else
				transpose = i;
			break;

		case 'T':
			rt_text_font = optarg;
			break;

		case 'C':
			rt_chord_font = optarg;
			break;

		case 's':
			i = atoi (optarg);
			if ( i == 0 )
				error_rt("invalid value for grid_size");
			else
				rt_grid_size = i;
			break;

		case 'g':
			no_easy_grids = TRUE;
			break;

		case 'G':
			rt_no_grid = TRUE;
			break;

		case 'l':
			lyrics_only= TRUE;
			break;

		case 'V':
			print_version();
			exit(0);
			break;

		case '2':
			pagination= 2;
			scale= scale2;
			rotation= 90.0;
			break;

		case '4':
			pagination= 4;
			scale= scale4;
			break;

		case 'i': /* generate in index */

			toc_file= tmpnam(NULL);
			toc_file_fd = fopen(toc_file, "w");
			if (toc_file_fd == NULL) {
				error ("unable to create temp file");
			}

			number_all = TRUE;
			break;

		case 'a':
			auto_space= TRUE;
			break;

		case 'p':
			i = atoi (optarg);
			if ( i == 0 )
				error_rt("invalid value for initial page number");
			else {
				page_label = i;
				number_all = TRUE;
			}
			break;

		case 'L':
			left_foot_even = 0;
			number_all= TRUE;
			break;

        	case 'o':
                	if ( freopen(optarg, "w", stdout) == NULL)
                        	{
                        	fprintf (stderr, "Unable to open \"%s\" for output\n", optarg);
                        	exit(1);
                        	}
	                break;

		case 'A':
			do_about();
			exit(0);
			break;

		case '?':
			do_help(argv[0]);
			break;
		}

/* Is there anything? */

	if (argc == 1)
		do_help(argv[0]);

/* Is there input? */

	if ((optind == argc) && isatty(0) && !dump_only)
	{
		fprintf (stderr, "Error: CHORD does not expect you to type the song on your keyboard.\n");
		fprintf (stderr, "Please either specify an input filename on the command line\n");
		fprintf (stderr, "or have the input redirected or piped in.\n");
		fprintf (stderr, "Exemples:\n");
		fprintf (stderr, "   %% chord my_song.cho > myfile.ps\n");
		fprintf (stderr, "   %% chord < mysong.cho > myfile.ps\n");
		fprintf (stderr, "Do \"chord -h\" to learn about CHORD's options\n");
		exit(1);
	}

/* Is there output? */

	if (isatty(1) && (!dump_only || postscript_dump)) /* 1 == stdout */
	{
		fprintf (stderr, "Error: CHORD will not send PostScript to your terminal.\n");
		fprintf (stderr, "Please either redirect (>) or pipe (|) the output.\n");
		fprintf (stderr, "Exemples:\n");
		fprintf (stderr, "   %% chord my_song.cho > myfile.ps\n");
		fprintf (stderr, "   %% chord my_song.cho | lpr\n");
		exit(1);
	}


/* File Processing */

	if (dump_only) 
	{
		dump_chords(postscript_dump);
		exit(0);
	}

	init_known_chords();
	init_values();
	init_ps();

	chord_inc = chord_size * 1.5;

	for ( ; optind < argc; optind++ )
		{
		strcpy(source, argv[optind]);
		read_input_file(source, source_fd);
		if (optind < argc - 1)
			do_new_song();
		}


	do_end_of_song();
	 
	if (toc_file_fd)	/* generate index  page */
		{
		build_ps_toc();
		do_end_of_page(FALSE);
		}

	if (v_pages != 0)
		{
		do_end_of_phys_page();
		}


	printf ("%%%%Trailer\n");
	printf ("%%%%Pages: %d 1\n", n_pages);
	printf ("%%%%EOF\n");

	exit (0);	
	return(0);
	}
