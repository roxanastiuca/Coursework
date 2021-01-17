/* STIUCA Roxana-Elena, 325CB */

#include "lib.h"

/**
 * Descriere: Functia trimite unui client TCP un mesaj de eroare.
 * Input:
 	* error -> mesajul;
 	* sockfd -> socket-ul pe care se trimite mesajul.
 * Output: N\A.
 */
void send_tcp_error(char error[MAX_CONTENT], int sockfd) {
	msg_udp msg;
	msg.content_type = ERROR;
	memcpy(msg.content, error, MAX_CONTENT);

	int ret = send(sockfd, &msg, sizeof(msg_udp), 0);
	DIE(ret < 0, "sending tcp error msg");
}

/**
 * Descriere: Functia corespunde cazului in care socket-ul setat este cel
 TCP pasiv, adica serverul detecteaza o conexiune noua TCP.
 * Input:
 	* read_fds -> adresa multimii de file descriptori pe care serverul
 	poate primi mesaje;
 	* tmp_fds -> adresa multimii de file descriptori auxiliara;
 	* tcpfd -> socket-ul TCP pasiv;
 	* fdmax -> adresa pentru maximul unui fd din read_fds;
 	* clients -> lista de clienti TCP ai serverului.
 * Output:
 	* N\A.
 * Erori tratate:
 	* Format mesaj TCP necunoscut.
 	* ID netrimis.
 	* Exista deja un client conectat cu ID-ul dat.
 */
void handle_new_tcp_connection(
	fd_set *read_fds, fd_set *tmp_fds, int tcpfd, int *fdmax,
	clients_array clients
	) {

	/* Am primit o cerere de conexiune prin portul pasiv TCP. */
	socklen_t clilen = sizeof(struct sockaddr_in);
	struct sockaddr_in cli_addr;
	int newtcpfd = accept(tcpfd, (struct sockaddr*) &cli_addr, &clilen);
	DIE(newtcpfd < 0, "accepting new tcp connection");

	/* Dezactivez algoritmul Neagle. */
	int ok = 1;
	int ret = setsockopt(newtcpfd, IPPROTO_TCP, TCP_NODELAY,
						(char *) &ok, sizeof(int));

	/* Adaug noul socket la multimea read_fds. */
	FD_SET(newtcpfd, read_fds);
	*fdmax = max(*fdmax, newtcpfd);

	*tmp_fds = *read_fds;

	/* Serverul se asteapta sa primeasca mesaj cu ID-ul noului client. */
	ret = select(*fdmax + 1, tmp_fds, NULL, NULL, NULL);

	if(!FD_ISSET(newtcpfd, tmp_fds)) {
		/* Nu s-a primit mesaj pe socket-ul corect. Noul client este ignorat
		deoarece nu a specificat ID-ul sau.*/
		send_tcp_error("ID remains unknown. Closing connection.", newtcpfd);
		FD_CLR(newtcpfd, read_fds);
		close(newtcpfd);
		return;
	}

	/* Primeste mesaj de la noul client. */
	msg_tcp msg;
	ret = recv(newtcpfd, &msg, sizeof(msg_tcp), 0);
	DIE(ret < 0, "receiving new tcp connection's id");

	if(msg.type != TCP_CONNECT) {
		/* Formatul mesajului este incorect. */
		send_tcp_error("Invalid TCP message. Expected TCP_CONNECT.", newtcpfd);
		FD_CLR(newtcpfd, read_fds);
		close(newtcpfd);
		return;
	}

	char id[MAX_ID + 1];
	memcpy(id, msg.id, MAX_ID);
	id[MAX_ID] = '\0';

	/* Caut in lista clientilor deja existenti. */
	client_info *client = find_client_by_id(clients, msg.id);

	if(client == NULL) {
		/* Clientul este unul nou. */
		add_client(clients, newtcpfd, msg.id);
	} else {
		if(client->connected) {
			/* Exista deja un client cu ID-ul dorit. Discard
			noului client. */
			send_tcp_error("ID is already in use. Closing connection.",
				newtcpfd);
			FD_CLR(newtcpfd, read_fds);
			close(newtcpfd);

			return;
		} else {
			/* Un client vechi s-a reconectat. */
			client->connected = 1;
			client->sockfd = newtcpfd;
			send_msg_queue(client);
		}
	}

	/* Afiseaza mesaj conexiune noua. */
	printf("New client (%s) connected from %s:%d.\n",
		id, inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
}

/**
 * Descriere: Functia corespunde cazului in care socket-ul setat este al
 unui client TCP.
 * Input:
 	* read_fds -> adresa multimii de file descriptori pe care serverul
 	poate primi mesaje;
 	* client -> clientul de pe care serverul asteapta un mesaj;
 	* sockfd -> socket-ul setat (cel al clientului).
 * Output:
 	* N\A.
 * Erori tratate:
 	* Clientul incearca sa se dezaboneze de la un topic la care
 	nu este abonat.
 	* Clientul trimite un mesaj cu format necunoscut.
 */
void handle_tcp_message(fd_set *read_fds, client_info *client, int sockfd) {
	msg_tcp msg;
	int ret = recv(sockfd, &msg, sizeof(msg_tcp), 0);
	DIE(ret < 0, "receiving new tcp message");

	if(ret == 0) {
		/* Clientul a inchis conexiunea. */
		client->connected = 0;
		FD_CLR(sockfd, read_fds);

		printf("Client (%s) disconnected.\n", client->id);
	} else {
		if(msg.type == TCP_SUBSCRIBE) {
			/* Clientul se poate reabona la un topic cu parametrii diferiti.
			Sterg versiunea veche si adaug topic-ul cu parametrii curenti. */
			if(msg.SF == 0) {
				remove_topic(&client->topics, msg.topic);
				remove_topic(&client->topics_SF, msg.topic);
				add_topic(&client->topics, msg.topic);
			} else if(msg.SF == 1) {
				remove_topic(&client->topics, msg.topic);
				remove_topic(&client->topics_SF, msg.topic);
				add_topic(&client->topics_SF, msg.topic);
			}
		} else if(msg.type == TCP_UNSUBSCRIBE) {
			if(!remove_topic(&client->topics, msg.topic)) {
				if(!remove_topic(&client->topics_SF, msg.topic)) {
					/* Clientul nu este abonat la topic-ul respectiv. */
					send_tcp_error("Client is not subscribed to that topic.",
						sockfd);
				}
			}
		} else {
			/* Mesaj cu format necunoscut. */
			send_tcp_error("Invalid TCP message.", sockfd);
		}
	}
}

/**
 * Descriere: Functia corespunde cazului in care socket-ul setat este cel
 UDP.
 * Input:
 	* udpfd -> socketul UDP;
 	* clients -> lista de clienti TCP ai serverului.
 * Output:
 	* N\A.
 */
void handle_udp_message(int udpfd, clients_array clients) {
	msg_udp msg;
	memset(&msg, 0, sizeof(msg_udp));

	struct sockaddr_in udp_addr;
	socklen_t socklen = sizeof(udp_addr);

	int ret = recvfrom(udpfd, &msg, sizeof(msg_udp) - 10, 0,
						(struct sockaddr*) &udp_addr, &socklen);
	DIE(ret < 0, "receiving new udp message");

	msg.udp_ip = udp_addr.sin_addr;
	msg.udp_port = udp_addr.sin_port;

	/* Trimit mesajul mai departe clientilor abonati. */
	for(int i = 0; i < clients->count; i++) {
		client_info *client = &clients->array[i];

		if(!client->connected) {
			/* Clientul nu este conectat. */
			if(!contains_topic(client->topics_SF, msg.topic)){
				/* Clientul nu este abonat. */
				continue;
			}

			/* Clientul este abonat cu optiunea SF=1. */
			enqueue(client->msg_queue, msg);
			continue;
		}

		if(!contains_topic(client->topics, msg.topic) &&
			!contains_topic(client->topics_SF, msg.topic)) {
			/* Clientul nu este abonat. */
			continue;
		}

		ret = send(client->sockfd, &msg, sizeof(msg_udp), 0);
		DIE(ret < 0, "sending topic message to subscriber");
	}
}

/**
 * Input:
 	* argc -> numarul parametrilor;
 	* argv[1] -> portul serverului.
 * Output:
 	* 0 -> program terminat cu succes.
 * Erori tratate:
 	* Erori apeluri de sistem (declanseaza o inchidere fortata);
 	* Parametrii invalizi (inchidere fortata);
 	* Comenzi de la stdin neacceptate.
 */
int main(int argc, char *argv[]) {
	int ret;

	PROG_ERROR(argc < 2, "Invalid number of arguments.\n");

	int portno = atoi(argv[1]);
	DIE(portno == 0, "atoi conversion");

	fd_set read_fds;	/* multimea de citire folosita in select() */
	fd_set tmp_fds;		/* multime auxiliara */
	int fdmax;			/* valoarea maxima din multimea read_fds */

	FD_ZERO(&read_fds);
	FD_ZERO(&tmp_fds);

	/* Deschid legatura UDP. */
	int udpfd = socket(AF_INET, SOCK_DGRAM, 0);
	DIE(udpfd < 0, "upd socket");

	/* Bind pentru socket-ul UDP. */
	struct sockaddr_in udp_addr;
	memset((char *) &udp_addr, 0, sizeof(struct sockaddr_in));
	udp_addr.sin_family = AF_INET;
	udp_addr.sin_port = htons(portno);
	udp_addr.sin_addr.s_addr = INADDR_ANY;

	ret = bind(udpfd, (struct sockaddr*) &udp_addr,
		sizeof(struct sockaddr_in));
	DIE(ret < 0, "binding udp");

	/* Adaug socket-ul in multimea read_fds. */
	FD_SET(udpfd, &read_fds);
	fdmax = udpfd;

	/* Deschid legatura TCP. */
	int tcpfd = socket(AF_INET, SOCK_STREAM, 0);
	DIE(tcpfd < 0, "tcp socket");

	/* Bind pentru socket-ul TCP pasiv. */
	struct sockaddr_in tcp_addr;
	memset((char *) &tcp_addr, 0, sizeof(struct sockaddr_in));
	tcp_addr.sin_family = AF_INET;
	tcp_addr.sin_port = htons(portno);
	tcp_addr.sin_addr.s_addr = INADDR_ANY;

	ret = bind(tcpfd, (struct sockaddr*) &tcp_addr,
		sizeof(struct sockaddr_in));
	DIE(ret < 0, "binding tcp");

	ret = listen(tcpfd, MAX_CLIENTS);
	DIE(ret < 0, "listen");

	/* Adaug socket-ul in mutlimea read_fds. */
	FD_SET(tcpfd, &read_fds);
	fdmax = max(fdmax, tcpfd);

	/* Adaug stdin in multimea read_fds. */
	FD_SET(0, &read_fds);

	/* Initializez lista clientilor TCP. */
	clients_array clients = init_clients_list();

	int continue_while = 1;

	while(continue_while) {
		tmp_fds = read_fds;
		ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
		DIE(ret < 0, "select");

		if(FD_ISSET(0, &tmp_fds)) {
			/* Serverul primeste mesaj de la stdin. */
			char buf[100];
			fgets(buf, 100, stdin);

			if(strncmp(buf, "exit", 4) == 0) {
				/* Opresc executia programului. */
				continue_while = 0;
			} else {
				printf("Invalid command. Expected exit.\n");
			}
		} else if(FD_ISSET(tcpfd, &tmp_fds)) {
			/* Serverul primeste o noua conexiune TCP. */
			handle_new_tcp_connection(&read_fds, &tmp_fds, tcpfd, &fdmax, clients);
		} else if(FD_ISSET(udpfd, &tmp_fds)) {
			/* Serverul primeste mesaj UDP. */
			handle_udp_message(udpfd, clients);
		} else {
			/* Serverul primeste mesaj pe unul din socketii activi ai
			clientilor TCP. */
			int sockfd = 0;

			for(int i = 0; i <= fdmax; i++) {
				if(FD_ISSET(i, &tmp_fds)) {
					sockfd = i;
				}
			}

			/* Caut in lista clientilor TCP caruia ii corespunde acest
			socket. */
			client_info *client = find_client_by_sockfd(clients, sockfd);
			PROG_ERROR(client == NULL, "Client not found.\n");

			handle_tcp_message(&read_fds, client, sockfd);
		}

		/* Decomenteaza pentru DEBUG. */
		//test_show_clients(clients);
	}

	/* Inchid toate conexiunile prin socketi. */
	close_clients_sockets(clients);
	close(udpfd);
	close(tcpfd);

	/* Dezaloc memoria folosita. */
	free_clients_array(clients);

	return 0;
}