all: is

OS := $(shell uname)
LBITS := $(shell getconf LONG_BIT)

include toolchain.mk


.PHONY: is
is: toolchain
	@$(MAKE) -f application.is.mk

.PHONY: mp
mp: toolchain
	@$(MAKE) -f application.is.mk mp

.PHONY: clean
clean:
	@$(MAKE) -f application.is.mk clean

debug: toolchain
	@$(MAKE) -f application.is.mk debug

flash: toolchain
	@$(MAKE) -f application.is.mk flash
	
setup:
	@$(MAKE) -f application.is.mk $(MAKECMDGOALS)	

dump:
	@$(MAKE) -f application.is.mk dump
