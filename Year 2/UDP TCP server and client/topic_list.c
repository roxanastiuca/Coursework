/* STIUCA Roxana-Elena, 325CB */

#include "lib.h"

/**
 * Initializeaza un nod din lista.
 */
TList init_node() {
	TList node = (TList) malloc(sizeof(TNode));

	if(node) {
		memset(node->topic, 0, MAX_TOPIC);
		node->next = NULL;
	}

	return node;
}

/**
 * Adaug un nou nod la inceputul listei.
 */
void add_topic(TList *alist, char topic[50]) {
	TList node = init_node();
	PROG_ERROR(node == NULL, "Memory error.\n");

	memcpy(node->topic, topic, 50);
	node->next = *alist;
	*alist = node;
}

/**
 * Sterge din lista nodul corespunzator unui topic.
 * Intoarce 0 daca topic-ul nu a fost gasit, 1 daca da.
 */
int remove_topic(TList *alist, char topic[50]) {
	TList p = *alist, prev = NULL;

	while(p != NULL && strncmp(topic, p->topic, MAX_TOPIC) != 0) {
		prev = p;
		p = p->next;
	}

	if(p == NULL) {
		/* Topic-ul nu a fost gasit. */
		return 0;
	}

	if(prev == NULL) {
		*alist = p->next;
		p->next = NULL;
		free(p);
	} else {
		prev->next = p->next;
		p->next = NULL;
		free(p);
	}

	return 1;
}

/**
 * Verifica daca un topic se gaseste deja in lista.
 * Intoarce 0 daca nu, 1 daca da.
 */
int contains_topic(TList list, char topic[50]) {
	TList p = list;

	while(p != NULL && strncmp(topic, p->topic, MAX_TOPIC) != 0) {
		p = p->next;
	}

	if(p == NULL) {
		return 0;
	}

	return 1;
}

/**
 *
 */
void free_topic_list(TList *alist) {
	TList p = *alist;
	*alist = NULL;

	while(p != NULL) {
		TList aux = p;
		p = p->next;
		free(aux);
	}
}