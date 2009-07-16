include ../Makefile.config

DISTFILES=Makefile Doxyfile
INSTALLH=
INSTALLLIB=

all:
	$(DOXYGEN)

.PHONY: all clean dist-files install-h install-lib

clean:
	rm -rf html latex *~

dist-files:
	@echo $(DISTFILES)

install-h:
	@echo $(INSTALLH)

install-lib:
	@echo $(INSTALLLIB)
