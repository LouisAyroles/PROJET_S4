CC=gcc
CFLAGS=-W -Wall -pedantic
LDFLAGS=
EXEC = couche3\
			 cmd_format

all: $(EXEC)

couche1: couche1.c couche1.h raid_defines.h
	$(CC) -o $@ $^ $(LDFLAGS)

couche2: couche2.c couche2.h raid_defines.h couche1.c couche1.h
	$(CC) -o $@ $^ $(LDFLAGS)

couche3: couche3.c couche3.h raid_defines.h couche2.c couche2.h couche1.c couche1.h
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o
	rm -rf ./docs/*

doc:
	doxygen Doxyfile

mrproper: clean
	rm -rf $(EXEC)
