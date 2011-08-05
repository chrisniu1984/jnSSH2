CFLAGS=-Wall -c -g
LDFLAGS=-Wall -lssh2

all: jnSSH2

jnSSH2: jnSSH2.o
	gcc $(LDFLAGS) $< -o $@

%.o: %.c
	gcc $(CFLAGS) -o $@ $^

install: jnSSH2
	sudo cp ./jnSSH2 /usr/bin/

clean:
	rm -f jnSSH2 *.o
