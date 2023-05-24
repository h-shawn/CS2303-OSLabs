all: disk fs client
clean:
		rm *.o
disk: disk.o
		gcc disk.o -o disk
fs: fs.o
		gcc fs.o -o fs
client: client.o
		gcc client.o -o client
disk.o: disk.c header.h
		gcc -c disk.c -o disk.o
fs.o: fs.c header.h
		gcc -c fs.c -o fs.o
client.o: client.c header.h
		gcc -c client.c -o client.o

