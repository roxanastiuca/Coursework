/**
 * Stiuca Roxana-Elena, 325CB
 */

#include "parser.h"

/**
 * Descriere: Calculeaza numarul de biti 1 dintr-un cuvant.
 * Input: Numarul pe 32 de biti.
 * Output: Numarul de biti.
 */
int bit_count(uint32_t num) {
	int count = 0;

	while(num != 0) {
		if((num & 1) != 0) {
			count++;
		}

		num = (num >> 1);
	}

	return count;
}

/**
 * Descriere: Functia parseaza rtable.txt si adauga in trie informatiile
 pentru intrarile din tabela.
 * Input: N\A.
 * Output: Radacina arborelui trie care contine informatiile tabelei de
 rutare.
 */
struct trie_node *read_rtable() {
	FILE *input = fopen("rtable.txt", "rt");
	DIE(input == NULL, "opening rtable.txt");

	char line[50];

	/* Initializez arborele. */
	struct trie_node *root = init_node();

	/* Citesc linie cu linie. */
	while(fgets(line, 50, input) != NULL) {
		char prefix[20], next_hop[20], mask[20];
		int interface;

		sscanf(line, "%s %s %s %d", prefix, next_hop, mask, &interface);

		struct route_table_entry *rtable_entry = (struct route_table_entry*)
							malloc(sizeof(struct route_table_entry));

		rtable_entry->prefix = inet_addr(prefix);
		rtable_entry->next_hop = inet_addr(next_hop);
		rtable_entry->mask = inet_addr(mask);
		rtable_entry->interface = interface;

		/* Inserez in arbore cu cheie prefix, lungimea numarul de biti 1 setati
		in masca si cu informatia din frunza rtable_entry. */
		insert(root, ntohl(rtable_entry->prefix),
			bit_count(rtable_entry->mask), rtable_entry);
	}

	return root;
}

