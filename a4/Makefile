PORT=56992
CFLAGS= -DPORT=\$(PORT) -g -std=gnu99 -Wall -Werror

friend_server: friend_server.o friends.o
	gcc $(CFLAGS) -o friend_server friend_server.o friends.o

friendme.o: friendme.c friends.h
	gcc $(CFLAGS) -c friendme.c

friends.o: friends.c friends.h
	gcc $(CFLAGS) -c friends.c

clean:
	rm -f *.o friend_server