/*
 Andrew Yang 11229926 ay0123
 Minor4 4/22
 minor4server.c
 Server roll
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>

#define MAX(a,b) (((a)>(b))?(a):(b))

#define NR_CLIENT  2
#define NR_BACKLOG 8
#define BUFF_SIZE  1024

char marks[] = {'X', 'O'};
char board[3][3] = {
	{' ', ' ', ' '},
	{' ', ' ', ' '},
	{' ', ' ', ' '},
};

/* wait until 2 clients are ready */
void init(int server_fd, int client_fds[])
{
	int i;
	socklen_t addrlen;
	char buffer[BUFF_SIZE], mark;
	struct sockaddr_in client_addrs[NR_CLIENT];

	for (i = 0; i < NR_CLIENT; i++) {
		addrlen = sizeof(client_addrs[i]);
		client_fds[i] = accept(server_fd,
					(struct sockaddr *)&client_addrs[i],
					&addrlen);

		if (client_fds[i] < 0) {
			perror("accept");
			exit(EXIT_FAILURE);
		}

		/* show greetings */
		mark = marks[i];
		printf("%c<Connected\n", mark);
		sprintf(buffer, "You are player %c%s", mark,
			i == 0 ? ". Waiting on player O to connect" : "");
		printf("%c>%s\n", mark, buffer);
		send(client_fds[i], buffer, sizeof(buffer), 0);
	}
}

void show_board(int client_fds[])
{
	char buffer[BUFF_SIZE], mark;
	int i;

	sprintf(buffer, "\n  board\n");
	sprintf(buffer + strlen(buffer), "  1 2 3\n");
	sprintf(buffer + strlen(buffer), " +-+-+-\n");
	sprintf(buffer + strlen(buffer), "A|%c|%c|%c\n",
			board[0][0], board[0][1], board[0][2]);
	sprintf(buffer + strlen(buffer), " +-+-+-\n");
	sprintf(buffer + strlen(buffer), "B|%c|%c|%c\n",
			board[1][0], board[1][1], board[1][2]);
	sprintf(buffer + strlen(buffer), " +-+-+-\n");
	sprintf(buffer + strlen(buffer), "C|%c|%c|%c",
			board[2][0], board[2][1], board[2][2]);

	for (i = 0; i < NR_CLIENT; i++) {
		mark = marks[i];
		printf("%c>%s\n", mark, buffer);
		send(client_fds[i], buffer, sizeof(buffer), 0);
	}
}

int is_cmd_valid(char buffer[])
{
	return buffer[0] == 'M' ||
		buffer[0] == 'R' ||
		buffer[0] == '?';
}

void gen_help(char buffer[])
{
	sprintf(buffer, "\n?-Display this help\n");
	sprintf(buffer + strlen(buffer), "R-Resign\n");
	sprintf(buffer + strlen(buffer), "Move<R><C>-Move where <R> is a row A, B or C and <C> is a column 1,2 or 3\n");
	sprintf(buffer + strlen(buffer), "   Example Moves: MA1 MC3 MB1");
}

/*
 * return value:
 *   0: valid move
 *  -1: invalid move
 */
int move(char buffer[], int idx)
{
	char ch;
	int i, j;

	sscanf(buffer, "M%c%d", &ch, &j);
	i = ch - 'A';
	j--;

	if (i < 0 || i > 2 || j < 0 || j > 2) {
		sprintf(buffer, "Invalid Move\n");
		sprintf(buffer + strlen(buffer), "  Move should be M<R><C> with no spaces\n");
		sprintf(buffer + strlen(buffer), "  Example: MA1 or MB3");
		return -1;
	}

	if (board[i][j] != ' ') {
		sprintf(buffer, "That spot is already taken");
		return -1;
	}

	board[i][j] = marks[idx];
	return 0;
}

struct point {
	int x;
	int y;
};

/*
 * return value:
 *   0: win
 *  -1: not yet
 */
int check_win(int idx)
{
	struct point conds[8][3] = {
		{{0,0}, {0,1}, {0,2}},
		{{1,0}, {1,1}, {1,2}},
		{{2,0}, {2,1}, {2,2}},
		{{0,0}, {1,0}, {2,0}},
		{{0,1}, {1,1}, {2,1}},
		{{0,2}, {1,2}, {2,2}},
		{{0,0}, {1,1}, {2,2}},
		{{0,2}, {1,1}, {2,0}},
	};

	int i, j;
	char mark;

	mark = marks[idx];
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 3; j++) {
			if (board[conds[i][j].x][conds[i][j].y] != mark)
				break;
		}
		if (j >= 3)
			return 0;
	}

	return -1;
}

/*
 * return value:
 *   0: tie
 *  -1: not yet
 */
int check_tie()
{
	int i, j;

	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			if (board[i][j] == ' ')
				return -1;
	return 0;
}

void play(int client_fds[])
{
	int i, nbytes;
	int turn, valid;
	char buffer[BUFF_SIZE];
	fd_set readfds;

	/* first turn */
	turn = 0;
	while (1) {
		/* show the chessboard */
		show_board(client_fds);

		/* show whose turn */
		sprintf(buffer, "Your turn");
		printf("%c>%s\n", marks[turn], buffer);
		send(client_fds[turn], buffer, sizeof(buffer), 0);

		/* retry until this round is valid */
		valid = 0;
		while (!valid) {
			FD_ZERO(&readfds);
			FD_SET(client_fds[0], &readfds);
			FD_SET(client_fds[1], &readfds);
			select(MAX(client_fds[0], client_fds[1]) + 1, &readfds,
					NULL, NULL, NULL);

			for (i = 0; i < NR_CLIENT; i++) {
				if (!FD_ISSET(client_fds[i], &readfds))
					continue;

				nbytes = recv(client_fds[i], buffer, BUFF_SIZE,
						0);
				if (nbytes <= 0) {
					/* client is broken, end the game */
					goto end;
				}
				buffer[nbytes] = '\0';
				printf("%c<%s", marks[i], buffer);

				/* command validity check */
				if (!is_cmd_valid(buffer)) {
					sprintf(buffer, "Invalid command");
					printf("%c>%s\n", marks[i], buffer);
					send(client_fds[i], buffer,
							sizeof(buffer), 0);
					continue;
				}

				if (buffer[0] == '?') {
					gen_help(buffer);
					printf("%c>%s\n", marks[i], buffer);
					send(client_fds[i], buffer,
							sizeof(buffer), 0);
					continue;
				}

				if (turn != i) {
					sprintf(buffer, "It's not your turn");
					printf("%c>%s\n", marks[i], buffer);
					send(client_fds[i], buffer,
							sizeof(buffer), 0);
					continue;
				}

				if (buffer[0] == 'R') {
					sprintf(buffer, "You win, %c resigned",
							marks[i]);
					printf("%c>%s\n", marks[1-i], buffer);
					send(client_fds[1-i], buffer,
							sizeof(buffer), 0);
					goto end;
				}

				/* here comes the real command */
				if (move(buffer, i) < 0){
					printf("%c>%s\n", marks[i], buffer);
					send(client_fds[i], buffer,
							sizeof(buffer), 0);
					continue;
				}

				/* mark this round valid */
				valid = 1;
				/* check if win */
				if (!check_win(i)) {
					show_board(client_fds);
					sprintf(buffer, "You win");
					printf("%c>%s\n", marks[i], buffer);
					send(client_fds[i], buffer,
							sizeof(buffer), 0);
					sprintf(buffer, "You lose");
					printf("%c>%s\n", marks[1-i], buffer);
					send(client_fds[1-i], buffer,
							sizeof(buffer), 0);
					goto end;
				}
				/* check if tie */
				if (!check_tie()) {
					show_board(client_fds);
					sprintf(buffer, "Tie game");
					printf("%c>%s\n", marks[0], buffer);
					send(client_fds[0], buffer,
							sizeof(buffer), 0);
					printf("%c>%s\n", marks[1], buffer);
					send(client_fds[1], buffer,
							sizeof(buffer), 0);
					goto end;
				}
			}
		}
		/* exchange the turn */
		turn = 1 - turn;
	}
end:
	printf("Game Ended\n");
	return;
}

int main(int argc, char *argv[])
{
	int server_fd, client_fds[NR_CLIENT];
	int option, i;
	struct sockaddr_in server_addr;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s port\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	/* enable SO_REUSEADDR */
	option = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &option,
				sizeof(option)) < 0) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(argv[1]));

	if (bind(server_fd, (struct sockaddr *)&server_addr,
				sizeof(server_addr)) < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}

	if (listen(server_fd, NR_BACKLOG) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	printf("Network Server Starting\n");
	printf("Waiting on Clients\n");

	init(server_fd, client_fds);
	play(client_fds);

	for (i = 0; i < NR_CLIENT; ++i) {
		close(client_fds[i]);
	}
	close(server_fd);
	return 0;
}

