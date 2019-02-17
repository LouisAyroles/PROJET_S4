CC=gcc
CFLAGS=-W -Wall -pedantic
LDFLAGS=
EXEC = couche2 

all: $(EXEC)

couche1: couche1.c couche1.h raid_defines.h
	$(CC) -o $@ $^ $(LDFLAGS)

couche2: couche2.c couche2.h raid_defines.h couche1.c couche1.h
		$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
