DEBUG = yes
export CC = gcc
export BIN = $(PWD)/bin
export INCLUDE = $(PWD)/include
export LIB = $(PWD)/lib

ifeq ($(DEBUG),yes)
	export CFLAGS = -g -W -Wall -std=c99 -pedantic -c 
	export LDFLAGS = -lcycle -lpthread 
else
	export CFLAGS = -W -Wall -std=c99 -pedantic -c
	export LDFLAGS = -lcycle -lpthread
endif

SRC = src
EXEC = compiler

all : $(EXEC)

$(EXEC) : 
	@(cd $(SRC) && $(MAKE))

.PHONY : clean mrproper $(EXEC)

clean : 
	@(cd $(SRC) && $(MAKE) $@)


mrproper : clean
	@(cd $(SRC) && $(MAKE) $@)
