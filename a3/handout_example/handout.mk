
# This is a test makefile for A3. It uses the files from the early version 
# of the demo that we did in week 6.


main : linked_list.o main.o
	gcc -Wall -g -std=gnu99 -o main linked_list.o main.o

main.o : main.c linked_list.h 
	gcc -Wall -g -std=gnu99 -c main.c

linked_list.o : linked_list.c linked_list.h
	gcc -Wall -g -std=gnu99 -c linked_list.c

# notice that this target has no dependencies
clean :
	rm -f main 
	rm -f linked_list.o 
	rm -f main.o
