CC=gcc
CFLAGS=-I -g
EXECS = couche1.o \
				cmd_format.o

all : $(EXECS)

clean :
		rm -rf ./*.o

couche1.o : couche1.c
%.o : %.c
	$(CC) -c $< -o $@ $(CFLAGS)

cmd_format.o : cmd_format.c
%.o : %.c
	$(CC) -c $< -o $@ $(CFLAGS)
