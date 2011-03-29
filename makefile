CFLAGS=-Wall -c -g
LDFLAGS=-Wall -lssh2
OBJS=jnSSH2.o

jnSSH2: $(OBJS)
	gcc $(LDFLAGS) $< -o $@

%.o: %.c
	gcc $(CFLAGS) -o $@ $^

clean:
	rm -f jnSSH2
	rm -f $(OBJS)
