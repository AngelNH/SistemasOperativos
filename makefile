all: semaforos mensajes

semaforos: semaforos.c
	gcc -o semaforos semaforos.c
mensajes: mensajes.c
	gcc -o mensajes mensajes.c

clean:
	rm semaforos mensajes