all:
	gcc -ggdb -march=native -Wimplicit-function-declaration  main.c -o client -lwayland-client

generate-protocols:
	wayland-scanner private-code /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdg-shell-protocol.c && wayland-scanner client-header /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdg-shell-client-protocol.h

clean:
	rm -f client
