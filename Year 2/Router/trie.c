/**
 * Stiuca Roxana-Elena, 325CB
 */

#include "trie.h"

/**
 * Descriere: Initializeaza un nod din arbore.
 * Input: N\A.
 * Output: Nodul.
 */
struct trie_node* init_node() {
	struct trie_node *node = NULL;
	node = (struct trie_node*)malloc(sizeof(struct trie_node));

	if(node) {
		node->rtable_entry = NULL;
		node->children[0] = node->children[1] = NULL;
	}

	return node;
}

/**
 * Descriere: Functia insereaza o cheie in arbore si pune in frunza
 sa intrarea din adresa de rutare.
 * Input: Radacina arborelui, cheia, lungimea pe care vor fi pusi bitii
 cheii, intrarea din tabela de rutare.
 * Output: N\A.
 */
void insert(
	struct trie_node *root,
	uint32_t key, 
	int length,
	struct route_table_entry *rtable_entry
	) {

	uint32_t bit;
	struct trie_node *crawl = root;

	/* Merg prin biti de la cel mai semnificativ si ii adaug in
	cate un nod din trie. */
	int shiftr = 31;

	for(int i = 0; i < length; i++) {
		bit = ((key >> shiftr) & 1);
		shiftr--;

		if(crawl->children[bit] == NULL) {
			/* Daca nu exista deja, initializez nodul copil. */
			crawl->children[bit] = init_node();
		}

		crawl = crawl->children[bit];
	}

	crawl->rtable_entry = rtable_entry;
}

/**
 * Descriere: Cauta o cheie in tabela. 
 * Input: Radacina arborelui trie, cheie pe care o cauta.
 * Output: Ultima intrare din tabela de rutare gasita cautand cheia.
 */
struct route_table_entry* search(struct trie_node *root, uint32_t key) {
	struct route_table_entry *rtable_entry= NULL;

	uint32_t bit;
	struct trie_node *crawl = root;

	int shiftr = 31;

	/* Merg prin toti bitii cheii sau cat timp nu am ajuns la o frunza
	a arborelui */
	for(int i = 0; i < 32 && crawl != NULL; i++) {
		if(crawl->rtable_entry != NULL) {
			rtable_entry = crawl->rtable_entry;
		}

		bit = ((key >> shiftr) & 1);
		shiftr--;

		crawl = crawl->children[bit];
	}

	if(crawl != NULL && crawl->rtable_entry != NULL) {
		rtable_entry = crawl->rtable_entry;
	}	

	return rtable_entry;
}