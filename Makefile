prog : fonction.o main.o
	gcc fonction.o main.o -pthread -o system
main.o : main.c
	gcc -c -Wall main.c
fonction.o : fonction.c
	gcc -c -Wall fonction.c
clean :
	rm -f *.o system