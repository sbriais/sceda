SUBDIRS=eratosthene
SUBDIRS+=graph graph_alg graph_path graph_flow graph_mcf graph_mrc graph_scc
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

dist-files:
	@echo $(DISTFILES)
	for dir in $(SUBDIRS); do \
	  for file in `$(MAKE) -s dist-files -C $$dir`; do \
	    echo "$$dir/$$file"; \
	  done; \
	done;

clean:
	for dir in $(SUBDIRS); do $(MAKE) clean -C $$dir; done


