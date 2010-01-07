include Makefile.config

SRCSUBDIRS=src
OTHERDIRS=doc examples
SUBDIRS=$(SRCSUBDIRS) $(OTHERDIRS)

PROJECT_NAME=SCEDA
MAJOR=1
MINOR=1
#PATCH=0
#PROJECT_NUMBER=$(MAJOR).$(MINOR).$(PATCH)
PROJECT_NUMBER=$(MAJOR).$(MINOR)

export PROJECT_NAME
export PROJECT_NUMBER

PROJECT=$(PROJECT_NAME)

LIBPREFIX?=lib
LIBNAME=$(LIBPREFIX)$(PROJECT)

TARFILE=$(PROJECT)-$(PROJECT_NUMBER).tar

DISTFILES=INSTALL COPYING COPYING.LESSER AUTHORS
DISTFILES+=Makefile Makefile.config.linux
DISTFILES+=$(SRCC) $(SRCH) 

INSTALLH=
INSTALLLIB=$(LIBNAME).a

all: $(LIBNAME).a doc 

$(LIBNAME).a: Makefile $(SRCSUBDIRS) $(OBJS)
	for dir in $(SRCSUBDIRS); do $(AR) rvs $(LIBNAME).a $$dir/*.o; done

.PHONY: all clean dist list-headers list-objects install subdirs $(SUBDIRS)

subdirs: $(SRCSUBDIRS) $(OTHERDIRS)

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
	for dir in $(SRCSUBDIRS); do \
	  for file in `$(MAKE) -s install-h -C $$dir`; do \
	    echo "$$dir/$$file"; \
	  done; \
	done

list-objects:
	for file in $(INSTALLLIB); do \
	  echo "$$file"; \
	done; \
	for dir in $(SRCSUBDIRS); do \
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
	for dir in $(SRCSUBDIRS); do \
	  for file in `$(MAKE) -s install-h -C $$dir`; do \
	    install -m "u+rw,go+r" "$$dir/$$file" "$(TARGET)/include/$(PROJECT)"; \
	  done; \
	done; \
	for file in $(INSTALLLIB); do \
	  install -m "u+rw,go+r" "$$file" "$(TARGET)/lib"; \
	done; \
	for dir in $(SRCSUBDIRS); do \
	  for file in `$(MAKE) -s install-lib -C $$dir`; do \
	    install -m "u+rw,go+r" "$$dir/$$file" "$(TARGET)/lib"; \
	  done; \
	done

-include .depend
