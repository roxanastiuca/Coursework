/* STIUCA Roxana-Elena, 325CB */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>

#include <math.h>

#define MAX_TOPIC 			50
#define MAX_CONTENT 		1500
#define MAX_ID				10
#define MAX_CLIENTS			5

/**
 * PRELUAT DIN LAB PC.
 * Macro de verificare a erorilor
 * Exemplu:
 *     int fd = open(file_name, O_RDONLY);
 *     DIE(fd == -1, "open failed");
 */

#define DIE(assertion, call_description)	\
	do {									\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",	\
					__FILE__, __LINE__);	\
			perror(call_description);		\
			exit(EXIT_FAILURE);				\
		}									\
	} while(0)

/* Structura pentru mesajele primite de server de la un client TCP. */
typedef struct {
	int type;				/* tipul mesajului; una din valorile de mai jos */
	char id[MAX_ID + 1];	/* folosit in cazul TCP_CONNECT */
	char topic[MAX_TOPIC];	/* folosit pt TCP_SUBSCRIBE si TCP_UNSUBSCRIBE */
	int SF;					/* --//-- */
} msg_tcp;

/* Valori acceptate pentru msg_tcp.type. */
#define TCP_CONNECT			0
#define TCP_SUBSCRIBE 		1
#define TCP_UNSUBSCRIBE		2


/* Structura pentru mesajele primite de server de la un client UDP,
adaptata si pentru mesajele trimise de server unui client TCP. */
typedef struct {
	char topic[MAX_TOPIC];
	char content_type;		/* una din valorile de mai jos */
	char content[MAX_CONTENT];
	struct in_addr udp_ip;
	unsigned short udp_port;
} msg_udp;

/* Valori acceptate pentru msg_udp.content_type. */
#define INT 				0
#define SHORT_REAL 			1
#define FLOAT				2
#define STRING 				3
#define ERROR 				4
#define HELLO	 			5

/* Structura pentru lista de topics. */
typedef struct node {
	char topic[MAX_TOPIC];
	struct node *next;
} TNode, *TList;

/* Structura pentru o lista de mesaje. */
typedef struct list_node {
	msg_udp msg;
	struct list_node *next;
} list_node, *list;

/* Coada de mesaje. Se foloseste de listele de deasupra. */
typedef struct {
	list head;
	list end;
} queue_obj, *queue;

/* Structura ce curprinde informatiile unui client TCP. */
typedef struct {
	char id[10];
	int sockfd;
	int connected;
	TList topics_SF;
	TList topics;
	queue msg_queue;
} client_info;

/* Structura pentru un vector dinamic de clienti. */
typedef struct {
	client_info *array;
	int size;
	int count;
} clients_info, *clients_array;

/* Functii din utils.c */
void PROG_ERROR(int cond, char *msg);
int max(int x, int y);

void resize_clients_list(clients_array clients);
void add_client(clients_array clients, int sockfd, char id[10]);
clients_array init_clients_list();
client_info *find_client_by_id(clients_array clients, char id[10]);
client_info *find_client_by_sockfd(clients_array clients, int sockfd);
void close_clients_sockets(clients_array clients);
void send_msg_queue(client_info *client);
void free_client(client_info clients);
void free_clients_array(clients_array clients);

void test_show_clients(clients_array clients);

/* Functii din topic_list.c */
TList init_node();
void add_topic(TList *alist, char topic[50]);
int remove_topic(TList *alist, char topic[50]);
int contains_topic(TList list, char topic[50]);
void free_topic_list(TList *alist);

/* Functii din queue.c */
queue init_queue();
int empty(queue q);
void enqueue(queue q, msg_udp msg);
msg_udp dequeue(queue q);