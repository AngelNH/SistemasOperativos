all: trenes

trenes: trenes.c
	gcc -o trenes trenes.c

clean:
	rm trenes