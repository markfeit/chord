# if you are compiling under DOS, you may have to replace all the ".o" extension
# by ".obj"
# Select the required paper size for your printer
PAPERSIZE = US
#PAPERSIZE = A4

# If your system does not have a getopt fonction, uncomment the next line
#GETOPT = getopt.o

# Use a ansi or K&R compiler
CC = cc 
CFLAGS = -D$(PAPERSIZE)

# where to put the resulting program
BINDIR = /usr/local/bin

# where to put the man page
MANDIR = /usr/man
MANEXT = l

# You should not have to change anything below this line

# name of the program
PROGRAMS = chord 

INCL = chord.h getopt.h
SRC = common.c xpose.c iso.c chord.c grid.c getopt.c chord.man Makefile  
OBJ = common.o xpose.o iso.o chord.o grid.o  ${GETOPT}
SONGS = songs/space-oddity songs/heaven songs/everybody-hurts
DOCS = README chord_guide.ps license.txt
RELNAME = chord35

all: $(PROGRAMS)

chord : $(OBJ) Makefile
	$(CC) $(CFLAGS) -o $@ $(OBJ)
chord.o    : chord.h
common.o   : chord.h
grid.o     : chord.h
xpose.o	   : chord.h
getopt.o   : getopt.h

install : all
	cp $(PROGRAMS) $(BINDIR)
	rm -f $(MANDIR)/cat$(MANEXT)/chord.$(MANEXT)
	cp chord.man $(MANDIR)/man$(MANEXT)/chord.$(MANEXT)
	chmod 666 $(MANDIR)/man$(MANEXT)/chord.$(MANEXT)
	
shar : $(SRC)
	shar -F -L 60 -o $(RELNAME).part $(SRC) $(INCL) $(SONGS) $(DOCS)

tar : $(SRC)
	tar cvf $(RELNAME).tar $(SRC) $(INCL) $(SONGS) $(DOCS)
	compress $(RELNAME).tar

lint : $(SRC)
	lint $(SRC)

clean:
	rm -f core *.o chord 
