/*
 *	可以使用密码参数的 SSH 客户端程序 v1.0
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Copyright (c) 2011 Niu.Chenguang <chrisniu1984@gmail.com>
 */

#include <string.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <libgen.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <termios.h>
 
#include <libssh2.h>

#define COPYRIGHT "jnSSH2 v1.0\nCopyright (C) 2011 Niu.Chenguang <chrisniu1984@gmail.com>\n\n"
 
struct termios _saved_tio;
int 	tio_saved = 0;
 
static int _raw_mode(void)
{
	int rc;
	struct termios tio;

	rc = tcgetattr(fileno(stdin), &tio);
	if (rc != -1) {
		_saved_tio = tio;
		tio_saved = 1;
		cfmakeraw(&tio);
		rc = tcsetattr(fileno(stdin), TCSADRAIN, &tio);
	}

	return rc;
}
 
static int _normal_mode(void)
{
	if (tio_saved)
		return tcsetattr(fileno(stdin), TCSADRAIN, &_saved_tio);

	return 0;
}
 
int main (int argc, char *argv[])
{
	int sock = 0;
	unsigned long hostaddr = 0;
	short port = 22;
	char *username = NULL;
	char *password = NULL;
	struct sockaddr_in sin;
	LIBSSH2_SESSION *session;
	LIBSSH2_CHANNEL *channel;
	int nfds = 1;
	char buf;
	LIBSSH2_POLLFD *fds = NULL;
 
	/* Struct winsize for term size */ 
	struct winsize w_size;
	struct winsize w_size_bck;

	/* For select on stdin */ 
	fd_set set;
	struct timeval timeval_out;
	timeval_out.tv_sec = 0;
	timeval_out.tv_usec = 10;

	printf(COPYRIGHT);

	if (argc > 4) {
		hostaddr = inet_addr(argv[1]);
		port = htons(atoi(argv[2]));
		username = argv[3];
		password = argv[4];
	}
	else {
		fprintf(stderr, "Usage: %s ip port user password\n", basename(argv[0]));
		return -1;
	}

	if (libssh2_init (0) != 0) {
		fprintf (stderr, "libssh2 initialization failed\n");
		return -1;
	}

	sock = socket (AF_INET, SOCK_STREAM, 0);
	sin.sin_family = AF_INET;
	sin.sin_port = port;
	sin.sin_addr.s_addr = hostaddr;
	if (connect(sock, (struct sockaddr *) &sin, sizeof(struct sockaddr_in)) != 0) {
		fprintf (stderr, "Failed to established connection!\n");
		return -1;
	}

	/* Open a session */ 
	session = libssh2_session_init();
	if (libssh2_session_startup (session, sock) != 0) {
		fprintf(stderr, "Failed Start the SSH session\n");
		return -1;
	}
	
	/* Authenticate via password */ 
	if (libssh2_userauth_password(session, username, password) != 0) {
		fprintf(stderr, "Failed to authenticate\n");
		close(sock);
		goto ERROR;
	}
	
	/* Open a channel */ 
	channel  = libssh2_channel_open_session(session);
	
	if ( channel == NULL ) {
		fprintf(stderr, "Failed to open a new channel\n");
		close(sock);
		goto ERROR;
	}
	
	/* Request a PTY */ 
	if (libssh2_channel_request_pty( channel, "xterm") != 0) {
		fprintf(stderr, "Failed to request a pty\n");
		close(sock);
		goto ERROR;
	}
	
	/* Request a shell */ 
	if (libssh2_channel_shell(channel) != 0) {
		fprintf(stderr, "Failed to open a shell\n");
		close(sock);
		goto ERROR;
	}
	
	if (_raw_mode() != 0) {
		fprintf(stderr, "Failed to entered in raw mode\n");
		close(sock);
		goto ERROR;
	}
	
	while (1) {
		FD_ZERO(&set);
		FD_SET(fileno(stdin),&set);
	
		ioctl(fileno(stdin), TIOCGWINSZ, &w_size);
		if ((w_size.ws_row != w_size_bck.ws_row) ||
			(w_size.ws_col != w_size_bck.ws_col)) {
			w_size_bck = w_size;
			libssh2_channel_request_pty_size(channel, w_size.ws_col, w_size.ws_row);
		}
	
		if ((fds = malloc (sizeof (LIBSSH2_POLLFD))) == NULL)
			break;
	
		fds[0].type = LIBSSH2_POLLFD_CHANNEL;
		fds[0].fd.channel = channel;
		fds[0].events = LIBSSH2_POLLFD_POLLIN;
		fds[0].revents = LIBSSH2_POLLFD_POLLIN;
	
		if (libssh2_poll(fds, nfds, 0) >0) {
			libssh2_channel_read(channel, &buf, 1);
			fprintf(stdout, "%c", buf);
			fflush(stdout);
		}
	
		if (select(fileno(stdin)+1,&set,NULL,NULL,&timeval_out) > 0)
			if (read(fileno(stdin), &buf, 1) > 0)
				libssh2_channel_write(channel, &buf, 1);
	
		free (fds);
	
		if (libssh2_channel_eof(channel) == 1)
		  break;
	}
	
	if (channel) {
		libssh2_channel_free (channel);
		channel = NULL;
	}
	
	_normal_mode();
	
	libssh2_exit();

	return 0;
ERROR:
	libssh2_session_disconnect(session, "Session Shutdown, Thank you for playing");
	libssh2_session_free(session);
	return -1;
}
