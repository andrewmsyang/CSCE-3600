/*
 Andrew Yang 11229926 ay0123
 Minor4 4/22
 minor4client.c
 Client roll
*/ 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>

#define MAX(a,b) (((a)>(b))?(a):(b))

#define BUFF_SIZE 1024

/*
 * return value:
 *   0: go on
 *   1: end
 */
int __play(int fd)
{
	int ret = 0;
	fd_set readfds;

	FD_ZERO(&readfds);
	/* add connected fd to fd_set */
	FD_SET(fd, &readfds);
	/* add stdin fd to fd_set */
	FD_SET(STDIN_FILENO, &readfds);

	select(MAX(fd, STDIN_FILENO) + 1, &readfds, NULL, NULL, NULL);

	if (FD_ISSET(fd, &readfds)) {
		char buffer[BUFF_SIZE];
		int nbytes;

		nbytes = recv(fd, buffer, BUFF_SIZE, 0);
		if (nbytes <= 0) {
			/* server connection is broken, end the game */
			ret = 1;
			goto out;
		}
		buffer[nbytes] = '\0';
		printf("%s\n", buffer);
	}
	if (FD_ISSET(STDIN_FILENO, &readfds)) {
		char buffer[BUFF_SIZE];
		int nbytes;

		if (fgets(buffer, BUFF_SIZE, stdin)) {
			nbytes = strlen(buffer);
			send(fd, buffer, nbytes, 0);
		}
	}

out:
	return ret;
}

void play(int fd) {
	while(!__play(fd));
}

int main(int argc, char *argv[])
{
	int fd;
	struct addrinfo hints, *__servers, *server;

	if (argc < 3) {
		fprintf(stderr, "Usage: %s hostname port\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	memset (&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	/* take hostname and port from argv */
	if (getaddrinfo(argv[1], argv[2], &hints, &__servers)) {
		perror("getaddrinfo");
		exit(EXIT_FAILURE);
	}

	for (server = __servers; server != NULL; server = server->ai_next) {
		fd = socket(server->ai_family, server->ai_socktype,
				server->ai_protocol);
		if (fd < 0)
			continue;

		/* break the loop whenever connect succeeded */
		if (!connect(fd, server->ai_addr, server->ai_addrlen))
			break;

		close(fd);
	}

	if (!server) {
		fprintf(stderr, "Failed to connect server %s\n", argv[1]);
		exit(EXIT_FAILURE);
	}
	freeaddrinfo(__servers);

	play(fd);

	close(fd);
	return 0;
}

