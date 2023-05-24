all: disk
clean:
		rm *.o
disk: disk.o
		gcc disk.o -o disk
disk.o: disk.c header.h
		gcc -c disk.c -o disk.o