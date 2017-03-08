
# Makefile for OS project
# To build, first `make dep`, them `make`. Everything should be automatic.
# Will compile all *.c and *.S files in the current directory.


# Flags to use when compiling, preprocessing, assembling, and linking
CFLAGS 	+= -Wall
LDFLAGS += -lpthread -std=c++11
CC=g++

#If you have any .h files in another directory, add -I<dir> to this line
CPPFLAGS += -g -std=c++11

# This generates the list of source files
SRC =  $(wildcard *.S) $(wildcard *.cpp)

# This generates the list of .o files. The order matters, boot.o must be first
OBJS  = main.o
# OBJS += $(filter-out main.o,$(patsubst %.S,%.o,$(filter %.S,$(SRC))))
OBJS += $(filter-out main.o,$(patsubst %.cpp,%.o,$(filter %.cpp,$(SRC))))

progm: Makefile $(OBJS)
	$(CC) $(OBJS) -o progm $(LDFLAGS)

dep: Makefile.dep

Makefile.dep: $(SRC)
	$(CC) -MM $(CPPFLAGS) $(SRC) > $@

.PHONY: clean
clean: 
	rm -f *.o Makefile.dep progm

ifneq ($(MAKECMDGOALS),dep)
ifneq ($(MAKECMDGOALS),clean)
include Makefile.dep
endif
endif
