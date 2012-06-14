CFLAGS      +=-ggdb -Wall -pedantic -std=gnu99
LDFLAGS     += -lbluetooth -lm -lcurl -lmysqlclient

all: smatool

OBJS=smatool.o in_bluetooth.o in_smadata2plus.o db_mysql.o utils.o logging.o iniparser.o dictionary.o

HEADER=smatool.h logging.h


smatool: ${OBJS}
	${CC} ${CFLAGS} -o smatool ${OBJS} ${LDFLAGS}

%.o: %.c ${HEADER}
	$(CC) ${CFLAGS} ${INCLUDES} -c -o $@ $<
	
clean:
	rm -f smatool *.o

all: smatool

