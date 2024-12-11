translate: main.o address.o stat.o
	gcc -o translate main.o address.o stat.o

main.o: main.c
	gcc -c main.c

address.o: address.c
	gcc -c address.c

stat.o: stat.c
	gcc -c stat.c

clean: 
	-rm -rf *.o main address stat