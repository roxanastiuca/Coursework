/* STIUCA Roxana-Elena, 325CB */

#include "lib.h"

/**
 * Descriere: Functia corespunde cazului in care clientul primeste
 o comanda de la stdin diferita de "exit".
 * Input:
 	* buffer -> string-ul citit de la stdin;
 	* sockfd -> socket-ul folosit pt conexiunea la server.
 * Output:
 	* N\A.
 * Erori tratate:
 	* Comenzi invalide.
 */
void handle_subscriptions(char buffer[100], int sockfd) {
	char *command = strtok(buffer, " \n");

	if(strncmp(command, "subscribe", 9) == 0) {
		char *topic = strtok(NULL, " \n");

		if(topic == NULL) {
			printf("Invalid arguments for subscribe command.\n");
			return;
		}

		char *nr = strtok(NULL, " \n");

		if(nr == NULL) {
			printf("Invalid arguments for subscribe command.\n");
			return;
		}

		int SF = -1;

		if(strcmp(nr, "0") == 0)		SF = 0;
		else if(strcmp(nr, "1") == 0)	SF = 1;

		if(strlen(topic) == 0 || (SF != 0 && SF != 1) || 
									strtok(NULL, " \n") != NULL) {
			printf("Invalid arguments for subscribe command.\n");
			return;
		}

		msg_tcp msg;
		memset(&msg, 0, sizeof(msg_tcp));
		memset(msg.topic, 0, MAX_TOPIC);

		msg.type = TCP_SUBSCRIBE;
		memcpy(msg.topic, topic, MAX_TOPIC);
		msg.SF = SF;

		/* Trimit mesajul de subscribe la server. */
		int ret = send(sockfd, &msg, sizeof(msg_tcp), 0);
		DIE(ret < 0, "sending subscribe");
	} else if(strncmp(command, "unsubscribe", 11) == 0) {
		char *topic = strtok(NULL, " \n");

		if(topic == NULL && strtok(NULL, " \n") != NULL) {
			printf("Invalid arguments for subscribe command.\n");
			return;
		}

		msg_tcp msg;
		msg.type = TCP_UNSUBSCRIBE;
		memcpy(msg.topic, topic, MAX_TOPIC);

		/* Trimit mesajul de unsubscribe la server. */
		int ret = send(sockfd, &msg, sizeof(msg_tcp), 0);
		DIE(ret < 0, "sending unsubscribe");
	} else {
		printf("Invalid command. Expected subscribe/unsubscribe/exit.\n");
		return;
	}
}

/**
 * Converteste continutul unui mesaj de la server in functie de tipul sau.
 */
void convert_udp_content(msg_udp msg) {
	switch(msg.content_type) {
		case INT: {
			int nr = 0;
			memcpy(&nr, msg.content + 1, 4);
			nr = ntohl(nr);

			if(msg.content[0] == 1) {
				nr *= -1;
			}

			printf("%d", nr);

			return;
		}

		case SHORT_REAL: {
			unsigned short snr;
			memcpy(&snr, msg.content, 2);

			int inr = ntohs(snr);
			float fnr = ((float) inr) / 100;

			printf("%.2f", fnr);

			return;
		}

		case FLOAT: {
			unsigned int module;
			memcpy(&module, msg.content + 1, 4);
			module = ntohl(module);

			int power = msg.content[5];

			float nr = ((float) module) / ((float) pow(10, power));

			if(msg.content[0] == 1) {
				nr *= -1;
			}

			printf("%.4f", nr);

			return;
		}

		case STRING: {
			char buffer[MAX_CONTENT + 1];
			memcpy(buffer, msg.content, MAX_CONTENT);
			buffer[MAX_CONTENT] = '\0';

			printf("%s", buffer);
			
			return;
		}

		default: {
			printf("UNKNOWN");
			return;
		}
	}
}

/**
 * Converteste un int msg_udp.type la string.
 */
char *type_to_string(int type) {
	switch(type) {
		case INT:
			return "INT";
		case SHORT_REAL:
			return "SHORT_REAL";
		case FLOAT:
			return "FLOAT";
		case STRING:
			return "STRING";
		default:
			return "";
	}
}

/**
 * Descriere: Functia corespunde cazului in care clientul primeste un
 mesaj de tip de la server referitorul la unul din topic-urile la care
 este abonat.
 * Input:
 	* msg -> mesajul primit de la server.
 * Output:
 	* N\A.
 */
void handle_news(msg_udp msg) {
	char topic[MAX_TOPIC + 1];
	memcpy(topic, msg.topic, MAX_TOPIC);
	topic[MAX_TOPIC] = '\0';

	printf("%s:%d - %s - %s - ",
		inet_ntoa(msg.udp_ip),
		ntohs(msg.udp_port),
		topic,
		type_to_string(msg.content_type));

	convert_udp_content(msg);

	printf("\n");
}

/**
 * Descriere: Functia corespunde cazului in care clientul primeste un
 mesaj de tip eroare de la server.
 * Input:
 	* msg -> mesajul primit de la server.
 * Output:
 	* N\A.
 */
void handle_error(msg_udp msg) {
	printf("Received error from server:\n <%s>\n", msg.content);
}

/**
 * Input:
 	* argc -> numarul parametrilor;
 	* argv[1] -> ID client.
 	* argv[2] -> IP server.
 	* argv[3] -> Port server.
 * Output:
 	* 0 -> program terminat cu succes.
 * Erori tratate:
 	* Erori apeluri de sistem (declanseaza o inchidere fortata);
 	* Parametrii invalizi (inchidere fortata);
 	* Comenzi de la stdin neacceptate.
 */
int main(int argc, char *argv[]) {
	int ret;

	msg_tcp msg;
	memset(&msg, 0, sizeof(msg_tcp));

	PROG_ERROR(argc < 4, "Invalid number of arguments.\n");

	char id[MAX_ID + 1];
	memcpy(id, argv[1], MAX_ID);

	char serv_ip[20];
	memcpy(serv_ip, argv[2], 20);

	unsigned short serv_portno = atoi(argv[3]);
	DIE(serv_portno == 0, "atoi conversion");

	fd_set read_fds;	/* multimea de citire folosita in select() */
	fd_set tmp_fds;		/* multime auxiliara */

	FD_ZERO(&read_fds);
	FD_ZERO(&tmp_fds);

	/* Deschid legatura cu serverul. */
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	DIE(sockfd < 0, "socket");

	/* Adaug stdin si socket-ul pentru server la multime. */
	FD_SET(0, &read_fds);
	FD_SET(sockfd, &read_fds);

	/* Conectare la server. */
	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(serv_portno);
	ret = inet_aton(serv_ip, &serv_addr.sin_addr);
	DIE(ret == 0, "inet_aton");

	ret = connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
	DIE(ret < 0, "connect");

	/* Trimit mesaj serverului cu ID-ul. */
	msg.type = TCP_CONNECT;
	memcpy(msg.id, id, MAX_ID);

	ret = send(sockfd, &msg, sizeof(msg_tcp), 0);
	DIE(ret < 0, "sending id");

	int continue_while = 1;

	while(continue_while) {
		tmp_fds = read_fds;
		ret = select(sockfd + 1, &tmp_fds, NULL, NULL, NULL);
		DIE(ret < 0, "select");

		if(FD_ISSET(0, &tmp_fds)) {
			/* Se primeste mesaj de la stdin. */
			char buffer[100];
			fgets(buffer, 100, stdin);

			if(strncmp(buffer, "exit", 4) == 0) {
				/* Opreste executia programului. */
				continue_while = 0;
			} else {
				handle_subscriptions(buffer, sockfd);
			}
		} else if(FD_ISSET(sockfd, &tmp_fds)) {
			msg_udp msg;
			ret = recv(sockfd, &msg, sizeof(msg_udp), 0);
			DIE(ret < 0, "receiving message from server");

			if(ret == 0) {
				/* Serverul a inchis conexiunea. */
				printf("Server closed connection.\n");
				continue_while = 0;
			} else {
				if(msg.content_type == ERROR) {
					handle_error(msg);
				} else if(msg.content_type == HELLO) {
					printf("%s\n", msg.content);
				} else {
					handle_news(msg);
				}
			}
		}
	}

	/* Inchid conexiunea. */
	close(sockfd);

	return 0;
}