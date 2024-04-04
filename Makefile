##################################
# Editor Makefile
# Author: Liam M. Murphy
##################################

CC = gcc

SOURCEDIR = src
SOURCES = $(wildcard $(SOURCEDIR)/*.c)
OBJECTS = $(SOURCES:.c=.o)

CFLAGS = -g -Wall

ledit: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

clean:
	rm $(SOURCEDIR)/*.o
	rm ledit

