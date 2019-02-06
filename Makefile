CC=gcc
CFLAGS=-W -Wall -pedantic
LDFLAGS=
EXEC=couche1

all: $(EXEC)

couche1: couche1.c couche1.h
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
