all:
	gcc -o server main.c game.c pipe_networking.c word_gen.c sem.c sharedmem.c -g
	gcc -o client client.c pipe_networking.c sem.c sharedmem.c -g

server:
	./server

client:
	./client

clean:
	rm server
	rm client
	rm generated
