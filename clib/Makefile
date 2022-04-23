CC     = cc
CFLAGS = -c
INC    = -framework IOKit
PREFIX = /usr/local
EXEC   = binsensor.o
SOURCES := $(shell find . -name '*.c')

build : $(EXEC)

clean : 
	rm $(EXEC)

$(EXEC) : sensor.c
	$(CC) $(CFLAGS) $(INC) -o $@ $?
