all: fs
clean:
		rm *.o
fs: fs.o
		gcc fs.o -o fs
fs.o: fs.c header.h
		gcc -c fs.c -o fs.o