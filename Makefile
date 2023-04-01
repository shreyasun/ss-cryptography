CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic -g -gdwarf-4 $(shell pkg-config --cflags gmp)
LFLAGS = $(shell pkg-config --libs gmp)

.PHONY: all clear

all: keygen encrypt decrypt

decrypt: decrypt.o ss.o numtheory.o randstate.o
	$(CC) -o $@ $^ $(LFLAGS)

encrypt: encrypt.o ss.o numtheory.o randstate.o
	$(CC) -o $@ $^ $(LFLAGS)

keygen: keygen.o ss.o numtheory.o randstate.o
	$(CC) -o $@ $^ $(LFLAGS)

ss: ss.o numtheory.o randstate.o
	$(CC) -o $@ $^ $(LFLAGS)

numtheory: numtheory.o randstate.o
	$(CC) -o $@ $^ $(LFLAGS)

ss.o: ss.c
	$(CC) $(CFLAGS) -c $<

numtheory.o: numtheory.c 
	$(CC) $(CFLAGS) -c $<

randstate.o: randstate.c
	$(CC) $(CFLAGS) -c $<

keygen.o: keygen.c
	$(CC) $(CFLAGS) -c $<

decrypt.o: decrypt.c
	$(CC) $(CFLAGS) -c $<

encrypt.o: encrypt.c
	$(CC) $(CFLAGS) -c $<
clean:
	rm *.o decrypt keygen encrypt

format:
	clang-format -i -style=file *.[ch]

