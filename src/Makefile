include ../Makefile.config

SRCC=list.c listset.c listmap.c
SRCC+=hashset.c hashmap.c
SRCC+=treeset.c treemap.c
SRCC+=graph.c graph_dag.c graph_matching.c graph_antichain.c graph_path.c graph_scc.c
SRCC+=pqueue.c
SRCC+=heap.c
 
SRCH=common.h util.h
SRCH+=list.h listset.h listmap.h queue.h stack.h
SRCH+=hashset.h hashmap.h
SRCH+=treeset.h treemap.h
SRCH+=graph.h graph_dag.h graph_matching.h graph_antichain.h graph_path.h graph_scc.h
SRCH+=pqueue.h
SRCH+=heap.h

DOCH=main.h 
DOCH+=set_doc.h map_doc.h list_doc.h
DOCH+=graph_doc.h graph_alg_doc.h 
DOCH+=pqueue_doc.h heap_doc.h

OBJS=$(SRCC:.c=.o) 

DISTFILES=Makefile $(SRCC) $(SRCH) $(DOCH)
INSTALLH=common.h 
INSTALLH+=list.h listset.h listmap.h queue.h stack.h
INSTALLH+=hashset.h hashmap.h
INSTALLH+=treeset.h treemap.h
INSTALLH+=graph.h graph_dag.h graph_matching.h graph_antichain.h graph_path.h graph_scc.h
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


