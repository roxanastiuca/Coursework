/* STIUCA Roxana-Elena, 325CB */

#include "lib.h"

/**
 * Initializare coada.
 */
queue init_queue() {
	queue q = (queue) malloc(sizeof(queue_obj));
	PROG_ERROR(q == NULL, "Memory error.\n");

	q->head = NULL;
	q->end = NULL;

	return q;
}

/**
 * Verificare coada goala.
 */
int empty(queue q) {
	return (q->head == NULL);
}

/**
 * Adauga un element in coada.
 */
void enqueue(queue q, msg_udp msg) {
	list node = (list) malloc(sizeof(list_node));
	PROG_ERROR(node == NULL, "Memory error.\n");

	memcpy(node->msg.topic, msg.topic, MAX_TOPIC);
	node->msg.content_type = msg.content_type;
	memcpy(node->msg.content, msg.content, MAX_CONTENT);
	node->msg.udp_ip = msg.udp_ip;
	node->msg.udp_port = msg.udp_port;

	node->next = NULL;

	if(q->end == NULL) {
		q->head = q->end = node;
	} else {
		q->end->next = node;
		q->end = node;
	}
}

/**
 * Scoate primul element din coada si il intoarce.
 */
msg_udp dequeue(queue q) {
	list node = q->head;
	q->head = q->head->next;

	if(q->head == NULL) {
		q->end = NULL;
	}

	msg_udp msg = node->msg;
	free(node);

	return msg;
}