SUBDIRS=eratosthene
SUBDIRS+=graph graph_alg
SUBDIRS+=hashmap treemap listmap 
SUBDIRS+=listset hashset treeset 
SUBDIRS+=heap pqueue 
SUBDIRS+=list queue stack 

DISTFILES=Makefile

all: $(SUBDIRS)

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ 

clean:
	for dir in $(SUBDIRS); do $(MAKE) clean -C $$dir; done


