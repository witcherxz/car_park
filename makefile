CC=gcc
CFLAGS=-c -std=gnu11 -O -g
# CFLAGS=-c -std=gnu11 -O0 -g
LIBS= -lSDL_bgi -l SDL2 -pthread -lm

all: cp

cp: CarPark.o Queue.o PQueue.o RandList.o CPSimulator.o Car.h Queue.h PQueue.h RandList.h CarPark.h
	$(CC) CarPark.o Queue.o PQueue.o RandList.o CPSimulator.o -o cp $(LIBS)

Queue.o: Queue.c Queue.h
	$(CC) $(CFLAGS) Queue.c

PQueue.o: PQueue.c PQueue.h
	$(CC) $(CFLAGS) PQueue.c

RandList.o: RandList.c RandList.h
	$(CC) $(CFLAGS) RandList.c

CPSimulator.o: CPSimulator.c
	$(CC) $(CFLAGS) CPSimulator.c

clean:
	rm cp Queue.o PQueue.o RandList.o CPSimulator.o

