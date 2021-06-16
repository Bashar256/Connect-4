all: Client Server clean


Client: client.o 
	gcc -I../lib -g -O2 -D_REENTRANT -Wall -o Client    client.o    ../libunp.a -lpthread

Server: server.o 
	gcc -I../lib -g -O2 -D_REENTRANT -Wall -o Server    server.o    ../libunp.a -lpthread

client.o: client.c 
	gcc -I../lib -g -O2 -D_REENTRANT -Wall   -c -o client.o client.c

server.o: server.c 
	gcc -I../lib -g -O2 -D_REENTRANT -Wall   -c -o server.o server.c

clean: 
	rm -rf *.o 