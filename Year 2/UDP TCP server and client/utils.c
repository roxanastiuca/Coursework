/* STIUCA Roxana-Elena, 325CB */

#include "lib.h"

/**
 * Verifica erori de program care duc la inchidere fortata.
 * De exemplu: alocari esuate, parametri gresiti ai programului.
 */
void PROG_ERROR(int cond, char *msg) {
	if(cond) {
		fprintf(stderr, "%s", msg);
		exit(0);
	}
}

/**
 * Maximul dintre 2 numere intregi.
 */
int max(int x, int y) {
	return (x > y) ? x : y;
}

/**
 * Realoca vectorul de clienti.
 */
void resize_clients_list(clients_array clients) {
	clients->size *= 2;
	clients->array = (client_info *) realloc(clients->array,
				clients->size * sizeof(client_info));
	PROG_ERROR(clients->array == NULL, "Memory error.\n");
}

/**
 * Adauga un nou client si ii trimite mesaj de tip HELLO.
 */
void add_client(clients_array clients, int sockfd, char id[MAX_ID + 1]) {
	if(clients->size == clients->count) {
		/* Este nevoie redimensionarea vectorului. */
		resize_clients_list(clients);
	}

	/* Trimit mesaj HELLO. */
	msg_udp connect_msg;
	memset(&connect_msg, 0, sizeof(msg_udp));

	connect_msg.content_type = HELLO;
	strcpy(connect_msg.content, "Welcome! Here is a list of valid commands:\n");
	strcat(connect_msg.content, "\t-> subscribe topic SF\n");
	strcat(connect_msg.content, "\t-> unsubscribe topic\n");
	strcat(connect_msg.content, "\t-> exit");

	int ret = send(sockfd, &connect_msg, sizeof(msg_udp), 0);
	DIE(ret < 0, "sending connect message");

	client_info new_client;
	new_client.sockfd = sockfd;
	memcpy(new_client.id, id, MAX_ID + 1);
	new_client.connected = 1;
	new_client.topics_SF = NULL;
	new_client.topics = NULL;
	new_client.msg_queue = init_queue();

	/* Adaug clientul. */
	clients->array[clients->count] = new_client;
	clients->count++;
}

/**
 * Initializeaza structura pentru vectorul dinamic de clienti.
 */
clients_array init_clients_list() {
	clients_array clients = (clients_array) malloc(sizeof(clients_info));
	PROG_ERROR(clients == NULL, "Memory error.\n");

	clients->array = (client_info *) malloc(2 * sizeof(client_info));
	PROG_ERROR(clients->array == NULL, "Memory error.\n");

	clients->size = 2;
	clients->count = 0;

	return clients;
}

/**
 * Cauta in vectorul in clienti dupa un ID dat. Intoarce NULL daca nu-l
 gaseste.
 */
client_info *find_client_by_id(clients_array clients, char id[MAX_ID + 1]) {
	int i;

	for(i = 0; i < clients->count; i++) {
		if(strncmp(clients->array[i].id, id, MAX_ID) == 0) {
			return &clients->array[i];
		}
	}

	return NULL;
}

/**
 * Cauta in vectorul in clienti dupa un socket dat. Intoarce NULL daca nu-l
 gaseste.
 */
client_info *find_client_by_sockfd(clients_array clients, int sockfd) {
	int i;

	for(i = 0; i < clients->count; i++) {
		if(clients->array[i].sockfd == sockfd) {
			return &clients->array[i];
		}
	}

	return NULL;
}

/**
 * Inchide toate conexiunile prin socketi cu clientii TCP.
 */
void close_clients_sockets(clients_array clients) {
	for(int i = 0; i < clients->count; i++) {
		if(clients->array[i].connected) {
			close(clients->array[i].sockfd);
		}
	}
}

/**
 * Trimite mesajele aflate in coada de asteptare ale unui client.
 */
void send_msg_queue(client_info *client) {
	/* Trimit mesaj de HELLO. */
	msg_udp connect_msg;
	connect_msg.content_type = HELLO;

	if(empty(client->msg_queue)) {
		strcpy(connect_msg.content, "Welcome back! No new messages.");
		int ret = send(client->sockfd, &connect_msg, sizeof(msg_udp), 0);
		DIE(ret < 0, "sending connect message");

		return;
	} else {
		strcpy(connect_msg.content, "Welcome back! Here is what you missed:");
	}

	int ret = send(client->sockfd, &connect_msg, sizeof(msg_udp), 0);
	DIE(ret < 0, "sending connect message");

	/* Parcurg coada si trimit toate mesajele. */
	while(!empty(client->msg_queue)) {
		msg_udp msg = dequeue(client->msg_queue);

		ret = send(client->sockfd, &msg, sizeof(msg_udp), 0);
		DIE(ret < 0, "sending enqueued message");
	}
}

/**
 *
 */
void free_client(client_info client) {
	free_topic_list(&client.topics_SF);
	free_topic_list(&client.topics);

	while(!empty(client.msg_queue)) {
		dequeue(client.msg_queue);
	}

	free(client.msg_queue);
}

/**
 *
 */
void free_clients_array(clients_array clients) {
	for(int i = 0; i < clients->count; i++) {
		free_client(clients->array[i]);
	}

	free(clients->array);
	free(clients);
}

/**
 * DEBUG: Functia nu este folosita. A fost folosita pentru afisarea tuturor
 informatiilor despre clientii conectati la server.
 */
void test_show_clients(clients_array clients) {
	printf("///////////////\n");
	printf("Numar clienti: %d. Size clients: %d\n",
		clients->count, clients->size);

	for(int i = 0; i < clients->count; i++) {
		printf("Id: %s; sockfd: %d; connectat: %d\n",
			clients->array[i].id,
			clients->array[i].sockfd,
			clients->array[i].connected);

		printf("\t Topicurile la care este abonat cu SF = 0: ");
		for(TList p = clients->array[i].topics; p != NULL; p = p->next) {
			printf("%s, ", p->topic);
		}
		printf("\n");

		printf("\t Topicurile la care este abonat cu SF = 1: ");
		for(TList p = clients->array[i].topics_SF; p != NULL; p = p->next) {
			printf("%s, ", p->topic);
		}
		printf("\n");

		printf("\t Mesajele din coada: ");
		list p;
		for(p = clients->array[i].msg_queue->head; p != NULL; p = p->next) {
			printf("%s ", p->msg.topic);
		}
		printf("\n");
	}
	printf("///////////////\n");
}