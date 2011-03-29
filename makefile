CFLAGS=-Wall -c -g
LDFLAGS=-Wall -lssh2
OBJS=jnSSH2.o

jnSSH2: $(OBJS)
	gcc $(LDFLAGS) $< -o $@

%.o: %.c
	gcc $(CFLAGS) -o $@ $^

install: jnSSH2
	sudo cp ./jnSSH2 /usr/bin/

clean:
	rm -f jnSSH2
	rm -f $(OBJS)
