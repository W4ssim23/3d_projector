#######################################
# Makefile Project Points             #
#                                     #
#######################################

PROG = project_points

all : $(PROG)

# Variables for file compilation
CC        =  gcc
CFLAGS    =  -g -Wall
CPPFLAGS  =  -DDEBUG
LDFLAGS   =  -g
LDLIBS    =  -lm #must come after the object file because it is a linking command

project_points : project_points.o Utils/Util.o Utils/imageFormationUtils.o

clean :
	@rm -f *.o Utils/*.o $(PROG)
