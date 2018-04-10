CFLAGS=-O2 -D_FORTIFY_SOURCE=2 -fno-plt -Wl,-z,relro,-z,now -fPIC -pie -std=c99 -Wall -pedantic -fno-omit-frame-pointer -fstack-protector-all -ftrapv
LDFLAGS=-Wl,-z,relro,-z,now -pie
CC=gcc
DEPS=debugmenot.h test_ptrace.h test_noaslr.h test_vdso.h test_env.h test_parent.h test_ldhook.h test_nearheap.h Makefile
OBJ=main.o test_ptrace.o test_vdso.o test_noaslr.o test_env.o test_parent.o test_ldhook.o test_nearheap.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

debugmenot: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm -f debugmenot $(OBJ)

all: debugmenot

.PHONY: all
