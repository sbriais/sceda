include ../Makefile.config

DISTFILES=Makefile Doxyfile
INSTALLH=
INSTALLLIB=

all:
	$(DOXYGEN)

clean:
	rm -rf html latex *~

dist-files:
	@echo $(DISTFILES)

install-h:
	@echo $(INSTALLH)

install-lib:
	@echo $(INSTALLLIB)
