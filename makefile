all:
	gcc -ggdb -march=native  main.c -o client -lwayland-client
clean:
	rm -f client
