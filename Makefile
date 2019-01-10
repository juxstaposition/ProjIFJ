# @Brief		Project in class IFJ, iperative language IFJ18
#				
#		MAKEFILE
#
# @Authors: 	Romana Džubárová,		xdzuba00
#				Šárka Ščavnícká,		xscavn00
#				Daniel Miloslav Očenáš,	xocena06
#				Michal Vanka,			xvanka00

CC=gcc
src = $(wildcard *.c)
obj = $(src:.c=.o)
CFLAGS=-std=gnu99 -Wall -pedantic -g

ifj18: $(obj)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) main