CC?=gcc

	ifdef	LIBHOME
SCEDA?=$(LIBHOME)
	else
SCEDA?=$(HOME)/SCEDA
	endif

CFLAGS=-O3 -Wall 
CPPFLAGS=-I$(SCEDA)/include
LDFLAGS=-L$(SCEDA)/lib -lSCEDA

PROJECT=example

SRCC=main.c
SRCC+=
SRCC+=
SRCC+=

SRCH=listset.h hashset.h treeset.h
SRCH+=
SRCH+=
SRCH+=


OBJS=$(SRCC:.c=.o) 

DISTFILES=Makefile $(SRCC) $(SRCH) 

all: .depend $(PROJECT)

$(PROJECT): $(SRCH) $(OBJS) Makefile 
	$(CC) -o $(PROJECT) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

clean:
	rm -f *~ $(OBJS) $(PROJECT) .depend

.depend: $(SRCC)
	$(CC) $(CPPFLAGS) -MM $(SRCC) > .depend

dist-files:
	@echo $(DISTFILES)

-include .depend
