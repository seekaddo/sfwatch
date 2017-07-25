##
## @file Makefile
##
## @author Dennis Addo <ic16b026@technikum-wien.at> (tdxxx)
## @date 22/07/2017
##
## @version 0.1 $
##
## @todo A simple file watcher
##
## Last Modified: $Author: Dennis $
##

CC=gcc
CFLAGS=-DDEBUG -Wall -pedantic -Werror -Wextra -Wstrict-prototypes \
                    -Wwrite-strings -fno-common -g -O3 -std=gnu11

OBJECTS=main.o array.o


##
## ----------------------------------------------------------------- rules --
##


%.o: %.c
	$(CC) $(CFLAGS) -c $<

##
## --------------------------------------------------------------- targets --
##

.PHONY: all install
all: sfwatch

install:
	sudo cp ./sfwatch /usr/local/bin

sfwatch: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^
run:
	./sfwatch

.PHONY: clean
clean:
	$(RM) *.o *~ sfwatch
