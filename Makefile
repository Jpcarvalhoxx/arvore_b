all: main.o 
	@gcc main.o  -o exe -lm
	@rm -r main.o

main.o: main.c
	@gcc main.c -c -Wall

run:
	@./exe