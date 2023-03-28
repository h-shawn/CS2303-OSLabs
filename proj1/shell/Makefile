all: shell
clean:
		rm *.o
shell: shell.o
		gcc shell.o -o shell
shell.o: shell.c header.h
		gcc -c shell.c -o shell.o