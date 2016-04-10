# Makefile for UD CISC user-level thread library

CC = gcc
CFLAGS = -g

LIBOBJS = ud_thread.o

TSTOBJS = T1.o T1x.o

# specify the executable 

EXECS = T1 T1x

# specify the source files

LIBSRCS = ud_thread.c 

TSTSRCS = T1.c T1x.c

# ar creates the static thread library

ud_thread.a: ${LIBOBJS} Makefile
	ar rcs ud_thread.a ${LIBOBJS}

# here, we specify how each file should be compiled, what
# files they depend on, etc.

ud_thead.o: ud_thread.c ud_thread.h Makefile
	${CC} ${CFLAGS} -c ud_thead.c

T1.o: T1.c ud_thread.h Makefile
	${CC} ${CFLAGS} -c T1.c

T1: T1.o ud_thread.a Makefile
	${CC} ${CFLAGS} T1.o ud_thread.a -o T1

T1x.o: T1x.c ud_thread.h Makefile
	${CC} ${CFLAGS} -c T1x.c

T1x: T1x.o ud_thread.a Makefile
	${CC} ${CFLAGS} T1x.o ud_thread.a -o T1x

clean:
	rm -f ud_thread.a ${EXECS} ${LIBOBJS} ${TSTOBJS} 
