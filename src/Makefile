include ../Makefile.config

# sets and maps
SRCC=list.c listset.c listmap.c
SRCC+=hashset.c hashmap.c
SRCC+=treeset.c treemap.c
# graphs
SRCC+=graph.c 
SRCC+=graph_traversal.c
SRCC+=graph_dag.c 
SRCC+=graph_matching.c 
SRCC+=graph_antichain.c 
SRCC+=graph_path.c
SRCC+=graph_negcycle.c
SRCC+=graph_meancycle.c
SRCC+=graph_scc.c 
SRCC+=graph_mrc.c
SRCC+=graph_flow.c
# heaps
SRCC+=pqueue.c
SRCC+=heap.c
 
SRCH=common.h util.h
# sets and maps
SRCH+=list.h listset.h listmap.h queue.h stack.h
SRCH+=hashset.h hashmap.h
SRCH+=treeset.h treemap.h
# graphs
SRCH+=graph.h
SRCH+=graph_traversal.h
SRCH+=graph_dag.h 
SRCH+=graph_matching.h
SRCH+=graph_antichain.h 
SRCH+=graph_path.h
SRCH+=graph_negcycle.h
SRCH+=graph_meancycle.h
SRCH+=graph_scc.h 
SRCH+=graph_mrc.h 
SRCH+=graph_flow.h
# heaps
SRCH+=pqueue.h
SRCH+=heap.h

DOCH=main.h 
DOCH+=set_doc.h map_doc.h list_doc.h
DOCH+=graph_doc.h graph_alg_doc.h 
DOCH+=pqueue_doc.h heap_doc.h

OBJS=$(SRCC:.c=.o) 

DISTFILES=Makefile $(SRCC) $(SRCH) $(DOCH)
INSTALLH=common.h 
# sets and maps
INSTALLH+=list.h listset.h listmap.h queue.h stack.h
INSTALLH+=hashset.h hashmap.h
INSTALLH+=treeset.h treemap.h
# graphs
INSTALLH+=graph.h 
INSTALLH+=graph_traversal.h
INSTALLH+=graph_dag.h 
INSTALLH+=graph_matching.h
INSTALLH+=graph_antichain.h
INSTALLH+=graph_path.h
INSTALLH+=graph_negcycle.h
INSTALLH+=graph_meancycle.h
INSTALLH+=graph_scc.h
INSTALLH+=graph_mrc.h
INSTALLH+=graph_flow.h
# heaps
INSTALLH+=pqueue.h
INSTALLH+=heap.h
INSTALLLIB=

all: .depend $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *~ $(OBJS) .depend

.depend: $(SRCC)
	$(CC) -MM $(SRCC) > .depend

dist-files:
	@echo $(DISTFILES)

install-h:
	@echo $(INSTALLH)

install-lib:
	@echo $(INSTALLLIB)

-include .depend


