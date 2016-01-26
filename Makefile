CFLAGS	= -g

.PHONY: all

all: vm

vm: vm.c vm.h
	$(CC) vm.c -o vm -Wall $(CFLAGS)
