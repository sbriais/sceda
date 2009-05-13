CC?=gcc

SCEDA?=$(HOME)/SCEDA

CFLAGS=-O3 -Wall 
CPPFLAGS=-I$(SCEDA)/include
LDFLAGS=-L$(SCEDA)/lib -lSCEDA

PROJECT=example

SRCC=main2.c
SRCC+=
SRCC+=
SRCC+=

SRCH=
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
