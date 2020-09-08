.PHONY: default clean install uninstall lib world c-world cpp-world java-world

default: lib

export PROJECT_ROOT := $(CURDIR)

lib:
	$(MAKE) -C c lib

install:
	$(MAKE) -C c install
	$(MAKE) -C cpp install

uninstall:
	$(MAKE) -C cpp uninstall
	$(MAKE) -C c uninstall

clean:
	$(MAKE) -C c clean
	$(MAKE) -C cpp clean
	$(MAKE) -C java clean

world: c-world cpp-world java-world

c-world: lib
	$(MAKE) -C c world

cpp-world: lib
	$(MAKE) -C cpp world

java-world: lib
	$(MAKE) -C java world
