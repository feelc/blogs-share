CC = gcc

DIR_INC = ../include
DIR_SRC = .

SRC := $(wildcard ${DIR_SRC}/*.c)

OBJ = $(patsubst %.c,%.o,${SRC})

TARGET = main
BIN_TARGET = ${TARGET}

CFLAGS = -g -O2 -Wall -w -std=gnu99
LIBS = -lpthread -levent

${BIN_TARGET} : ${OBJ}
	${CC} ${CFLAGS} ${OBJ} ${LIBS} -o $@

$(OBJ):%.o:%.c
	${CC} ${CFLAGS} -c $< -o $@

.PHONY : clean run
clean:
	-rm -rf ${BIN_TARGET} ${OBJ}

run:
	${BIN_TARGET}
