# chord

This is an archival copy of a chord printing utility that was posted
to Usenet in the early 1990s and has largely been replaced by Johan
Vromans' [Chordii](https://www.vromans.org/projects/Chordii).

The original README is replicated below and reformatted.

-----

This is a description of a Share-Ware utility called CHORD.

It's purpose is to provide guitar players with a tool to produce good
looking, self-descriptive music sheet from a text file.

CHORD read a text file containing the lyrics of a song, the chords to
be played, their description and some other optional data to produce a
PostScript document that includes:

	Centered titles
	Chord names above the words
	Graphical representation of the chords at the end of the songs

CHORD also provides support for 

	Multiple logical pages per physical pages ( 1, 2 or 4)
	Configurable fonts for the lyrics and the chord names
	Multiple songs inside one file
	The complete ISO 8859-1 character set
and
	Chorus marking

## NEW IN VERSION 3.5

 - Generation of PostScript indexes.
 - Generation of PostScript chord-charts.
 - Additional 'comments' style (box and italic).
 - Directive to set a monospace font (ie: not-proportional) suitable for
  tabs.
 - Support for A4 paper format.
 - Page numbers are now "physical page numbers".
 - Chord-charts (-dump option) now include chords from .chordrc file.
 - Option to print/not-print page numbers.
 - Option to print/not-print chord grids.
 - Option to print grids only for "difficult" chords.
 - Directive to force a physical page break.
 - Directive to force a virtual page break.
 - Chords are printed in a logical order.
 - Special chord [N.C.] (no-chord) without a chord grid.
 - New (and improved!) {define} syntax.
 - Chord names and directives are case-insensitive.
 - Public domain getopt() routine (for portability).
 - (almost) lint free (better portability)
 - Ansi-C compliant
 - ... and bug fixes

## NEW IN VERSION 1.2

 - More flexible Page Numbering
  o Left/right
  o always-present
  o an option for simgle spacing on lines with no chords
 - Distinction between an unplayed string and a buffed one
 - TOC generation
 - Some bug fixes

## NEW IN VERSION 1.1

 - CHORD supports transposition. See the man page.
 - miscellanious bug fixes, including dubious coding that kept IBM and
  SGI from executing CHORD properly
 - the grids now indicate string played open
 - Error messages noe correctly report the name of the file and
  the line number where the error was found

## PLATFORM

CHORD has been developped on SPARCStations running Solaris 2.2 and
OpenWindows 3.2. The PostScript previewer (PageView), SparcPrinters and
LaserWriter II have had no problem with the output. Please report any
changes your system requires.

## CREDITS

CHORD has been developped and is still being maintained by Martin
Leclerc and Mario Dorion. 

CHORD uses the GNU getopt() routine made available to the community by
the Free Software Foundation.

We would like to thanks the following individuals:

Steve Putz, who has kindly contributed a number of suggestions, bug
fixes and enhancements(?).

Jim Gerland (GERLAND@ubvms.cc.buffalo.edu), who has performed the initial
VMS port.

Leo Bicknell (ab147@freenet.acsu.buffalo.edu) has provided the chord sorting
routine.

Additional thanks to Fred handloser (and his pal Duncan) and many
others for providing many suggestions of features that were implemented
in CHORD 3.5.


# COMPILING

CHORD is written in fairly portable C and should work on most UNIX
systems with little or no change. Let us know of your problems.

Simply edit the Makefile to meet your environment and 'make'.

Note: If your environment does not offer a getopt() routine edit the
Makefile to de-comment the lines (INCL, SRC and OBJ) that mention the
included getopt routine ... We reccomend using your system's getopt()
if it is available.


# ARCHIVES

James B. from the University of Nevada (nevada.edu) has
graciously open the doors of their ftp site for archiving
songs in CHORD format. Look for files with the '.chordpro' 
or '.chopro' extension.

# COPYRIGHT

CHORD is a Share-Ware that is licensed following the conditions of the
general GNU license. You are authorized to use this program free of
charge. You are authorized to distribute this program freely as long as
the full source is included. You are not allowed to remove the
`copyright' notices from the authors nor are you allowed to pretend you
wrote it. You are not allowed to charge any money for CHORD. You are
not allowed to distribute a modified version of CHORD without written
authorizations from the authors. You are not allowed to use parts of
CHORD in any other commercial or public-domain software. Sorry for all
the negatives rules ... but we've been bitten once!



As we said, CHORD is a Share-Ware. The authors spent a great deal of
time making it as feature-full and usable as possible. If you like this
program, please take the time to send a postcard from your home town or
locality to each of the two authors and let them know what you think of
CHORD. Please send the cards to the following addresses:

	Martin Leclerc		Mario Dorion
	19 Browning		18 Georgia Crescent
	Dollard des Ormeaux	Pointe Claire
	Quebec, H9G 2K5		Quebec, H9R 5V7
	CANADA			CANADA


Martin Leclerc (Martin.Leclerc@canada.sun.com)
Mario Dorion   (Mario.Dorion@canada.sun.com)	
