CC = gcc
CFLAGS = -lsodium -lsqlite3 -Wall -Wshadow -Wextra 
OUT = passwdman
IN = main.c

passwdman: src/main.c
	mkdir bin
	$(CC) src/$(IN) -o bin/$(OUT) $(CFLAGS)

debug: src/main.c 
	mkdir bin
	$(CC) src/$(IN) -o bin/$(OUT) $(CFLAGS) -g -O2

clean:
	rm -rf bin/
