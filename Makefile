CC=gcc
HEADER=smatool.h mysql.h
OBJ=smatool.o mysql.o
LDFLAGS=-L/usr/lib64/mysql
#LDFLAGS=-L/opt/lib/mysql
LIBS=-lbluetooth -lcurl -lmysqlclient
CFLAGS=-g -Wall
# -E

smatool: $(OBJ) $(HEADER)
	gcc $(LDFLAGS) $(LIBS) $(CFLAGS) -o $@ $(OBJ)

%.o: %.c ${HEADER}
	$(CC) ${CFLAGS} ${INCLUDES} -c -o $@ $<

clean:
	rm smatool *.o

refresh:
	hg pull https://code.google.com/p/sma-bluetooth/

diff:
	hg diff
