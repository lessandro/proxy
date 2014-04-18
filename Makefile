CFLAGS+=-std=c99 -Wall
FILES=tunnel.c faketcp.c ./sev/*.c
LIBS=-lev

all:
	$(CC) $(CFLAGS) -o local local.c $(FILES) $(LIBS)
	$(CC) $(CFLAGS) -o remote remote.c $(FILES) $(LIBS)
