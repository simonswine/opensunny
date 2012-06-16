CFLAGS      +=-ggdb -Wall -pedantic -std=gnu99
LDFLAGS     += -lbluetooth

all: opensunny

OBJS=opensunny.o in_bluetooth.o in_smadata2plus.o utils.o logging.o

opensunny: ${OBJS}
	${CC} ${CFLAGS} -o opensunny ${OBJS} ${LDFLAGS}

%.o: %.c ${HEADER}
	$(CC) ${CFLAGS} ${INCLUDES} -c -o $@ $<
	
clean:
	rm -f opensunny *.o

all: opensunny

