CC=gcc

all: dv

dv: dv.c
	$(CC) -O3 -Wall -o dv dv.c `pkg-config --cflags gtk+-2.0` \
	`pkg-config --libs gtk+-2.0`

clean:
	rm -f dv
