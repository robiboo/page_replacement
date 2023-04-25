.SUFFIXES: .c .o
CC = gcc
CCFLAGS = -Wall -Werror
EXEC = page_rep
OBJS = page_rep.o

${EXEC}: ${OBJS}
	${CC} ${CCFLAGS} -o ${EXEC} ${OBJS}

.c.o:
	${CC} ${CCFLAGS} -c $<

run: ${EXEC}
	./${EXEC}

clean:
	rm -f ${EXEC} ${OBJS}

page_rep.o: page_rep.c