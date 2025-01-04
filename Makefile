parser.out: main.o files.o info.o messages.o sort.o strings.o
	gcc -o parser.out main.o files.o info.o messages.o sort.o strings.o

main.o: main.c
	gcc -c main.c

files.o: files.c
	gcc -c files.c

info.o: info.c
	gcc -c info.c

messages.o: messages.c
	gcc -c messages.c

sort.o: sort.c
	gcc -c sort.c

strings.o: strings.c
	gcc -c strings.c

clean:
	rm *.o
	rm *.out
	rm -rf ./REPORTES