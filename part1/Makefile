# Makefile for UD CISC user-level thread library

CC = gcc
CFLAGS = -g -MMD -Wall -Wextra

LIBSRCS = ud_thread.c
TSTSRCS = T1.c T1x.c

LIBOBJS = $(LIBSRCS:.c=.o)
TSTOBJS = $(TSTSRCS:.c=.o)
EXECS = $(TSTSRCS:.c=)

all : ud_thread.a $(EXECS)

# ar creates the static thread library
ud_thread.a: ${LIBOBJS}
	ar rcs $@ $^

LOADLIBES = ud_thread.a

.PHONY: all clean

clean:
	rm -f ud_thread.a ${EXECS} ${LIBOBJS} ${TSTOBJS}

-include $(LIBSRCS:.c=.d) $(TSTSRCS:.c=.d)
