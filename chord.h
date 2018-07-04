#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


#define VERSION "3.5"
#define PATCH_LEVEL "0"

#define TRUE 1
#define FALSE 0

#define MAXLINE 256
#define MAXFONTS 16   /* Maximum number of different fonts in one execution */
#define MAXTOKEN 256
#define MAX_CHORD 1024
#define CHORD_NAME_SZ   10
#define MAXNOTE 8

#define LONG_FINGERS	4
#define FRET_NONE_STR	"-"		/* fret value for unplayed strings */
#define FRET_NONE	-2		/* internal numeric value */
#define FRET_X_STR	"X"		/* fret value for muted strings */
#define FRET_X		-1		/* internal value (must be -1) */
#define NO_CHORD_STR	"N.C."		/* Indicates No-Chord */
#define BASE_FRET_STR	"base-fret"
#define FRETS_STR	"frets"

#ifdef US
#define TOP 756         /* 10.5 inches in points */
#define BOTTOM 36        /* 0.5 inch in points */
#define L_MARGIN 72      /* 1 inch in points */
#define WIDTH 612        /* 8.5 inches in points */
#else
	/* Note: Not having access to an actual A4 PostScript printer
		 the CHORD authors had to rely on input from beta-testers
		 on what were the proper values to use for these fields.
		 We though 813 was the right value for TOP.
		 Some beta testers agreed, some thought it was better
		 to leave the US value (756). We left 756 in and commented
		 out the 813 entry. If the A4 page does not look to good for
		 your taste, you may want to recompile with the other value.
		 Thanks for your co-operation ... The authors */

#define TOP 756         /* 10.5 inches in points */
/* #define TOP 813 */        /* 28.7 cm or 11.30 inches in points */
#define BOTTOM 36      /* 1.25 cm or 0.5 inch in points */
#define L_MARGIN 72     /* 2.5 cm or 1 inch in points */
#define WIDTH 595       /* 21 cm or 8.27 inches in points */

#endif /* US */

#define DELIM_STR       ": \t"

#define DEF_TEXT_SIZE 12
#define DEF_CHORD_SIZE 9
#define DEF_GRID_SIZE 30
#define DEF_TEXT_FONT "Times-Roman"
#define DEF_CHORD_FONT "Helvetica-Oblique"
#define MONOSPACED_FONT "Courier"

#define CHORD_BUILTIN	0
#define CHORD_DEFINED	1
#define CHORD_IN_CHORDRC	2

#define CHORD_EASY	0
#define CHORD_HARD	1

#ifdef US
#define scale2 .75 	/* 8.5" / 11" - margin */
#define scale4 .475	/* 1/2 - margin */
#else
#define scale2 .69	/* 21(cm) / 29.7(cm) - margin */
#define scale4 .475	/* 1/2 - margin */
#endif /* US */

struct chord_struct {
	int  next_ptr;
	char chord_name[CHORD_NAME_SZ];
	};


int do_define_chord();
void build_ps_toc();
void do_chorus_line();
void do_end_of_page();
void do_end_of_phys_page();
void do_end_of_song();
void do_init_grid_ps();
void do_new_song();
void do_start_of_page();
void do_subtitle();
void do_title();
void draw_chords();
void dump_chords();
void init_known_chords();
void init_ps();
void print_chord_line ();
void print_re_encode();
void print_text_line();
void print_version();
void read_chordrc();
void set_chord_font();
void use_chord_font();
void use_text_font();

#ifdef  __STDC__
int add_to_chordtab(char *chord);
int do_transpose(char *chord);
int is_chord_known (char *chord);
void advance(int amount);
void debug(char *dbg_str);
void do_chord (int i_text, char *chord);
void do_comment(char *comment, int style);
void do_directive(char *directive);
void do_help (char *command) ;
void dump_fret(int fretnum);
void error(char *error);
void error_rt(char *error);
void moveto(int new_hpos, int new_vpos);
void process_file(FILE *source_fd);
void ps_fputc(FILE *fd, int c);
void ps_fputs(FILE *fd, char *string);
void ps_puts(char *string);
void put_in_string(char array[], int *p_index, int c, int max_index, int *p_ov_flag);
void re_encode(char *font);
void read_input_file(char source[], FILE *source_fd);
void set_text_font(int size);
char *tolower_str(char *string);
char *toupper_str(char *string);
extern      char *strtok(char *s1, const char *s2);
#else /* __STDC__ */
int add_to_chordtab();
int do_transpose();
int is_chord_known ();
void advance();
void debug();
void do_chord ();
void do_comment();
void do_directive();
void do_help ();
void do_translate();
void dump_fret();
void error();
void error_rt();
void moveto();
void process_file();
void ps_fputc();
void ps_fputs();
void ps_puts();
void put_in_string();
void re_encode();
void read_input_file();
void set_text_font();
char *tolower_str();
char *toupper_str();
extern char *strtok();

#endif /* ANSI_C */
