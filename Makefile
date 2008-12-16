include Makefile.config

ASUBDIRS=src
OSUBDIRS=doc
SUBDIRS=$(ASUBDIRS) $(OSUBDIRS)

PROJECT_NAME=SCEDA
PROJECT_NUMBER=1.0.5

export PROJECT_NAME
export PROJECT_NUMBER

PROJECT=$(PROJECT_NAME)

LIBPREFIX?=lib
LIBNAME=$(LIBPREFIX)$(PROJECT)

TARFILE=$(PROJECT)-$(PROJECT_NUMBER).tar

DISTFILES=INSTALL COPYING COPYING.LESSER
DISTFILES+=Makefile Makefile.config.linux
DISTFILES+=$(SRCC) $(SRCH) 

INSTALLH=
INSTALLLIB=$(LIBNAME).a

all: $(LIBNAME).a 

$(LIBNAME).a: Makefile subdirs $(OBJS)
	for dir in $(ASUBDIRS); do $(AR) rvs $(LIBNAME).a $$dir/*.o; done

.PHONY: subdirs $(SUBDIRS)

subdirs: $(ASUBDIRS) $(OSUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ 

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	for dir in $(SUBDIRS); do $(MAKE) clean -C $$dir; done
	rm -f *~ $(OBJS) $(LIBNAME).a 

dist:
	tar cfv $(TARFILE) $(DISTFILES); \
	for dir in $(SUBDIRS); do \
	  for file in `$(MAKE) -s dist-files -C $$dir`; do \
	    tar rfv $(TARFILE) "$$dir/$$file"; \
	  done; \
	done;
	gzip $(TARFILE)

list-headers:
	for file in $(INSTALLH); do \
	  echo "$$file"; \
	done; \
	for dir in $(SUBDIRS); do \
	  for file in `$(MAKE) -s install-h -C $$dir`; do \
	    echo "$$dir/$$file"; \
	  done; \
	done

list-objects:
	for file in $(INSTALLLIB); do \
	  echo "$$file"; \
	done; \
	for dir in $(SUBDIRS); do \
	  for file in `$(MAKE) -s install-lib -C $$dir`; do \
	    echo "$$dir/$$file"; \
	  done; \
	done

install: $(LIBNAME).a
	mkdir -p "$(TARGET)/include/$(PROJECT)"; \
	mkdir -p "$(TARGET)/lib"; \
	for file in $(INSTALLH); do \
	  install -m "u+rw,go+r" "$$file" "$(TARGET)/include/$(PROJECT)"; \
	done; \
	for dir in $(SUBDIRS); do \
	  for file in `$(MAKE) -s install-h -C $$dir`; do \
	    install -m "u+rw,go+r" "$$dir/$$file" "$(TARGET)/include/$(PROJECT)"; \
	  done; \
	done; \
	for file in $(INSTALLLIB); do \
	  install -m "u+rw,go+r" "$$file" "$(TARGET)/lib"; \
	done; \
	for dir in $(SUBDIRS); do \
	  for file in `$(MAKE) -s install-lib -C $$dir`; do \
	    install -m "u+rw,go+r" "$$dir/$$file" "$(TARGET)/lib"; \
	  done; \
	done


-include .depend
