/**
 * Stiuca Roxana-Elena, 325CB
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "skel.h"

struct trie_node {
	struct trie_node *children[2];
	struct route_table_entry *rtable_entry;
};

struct trie_node* init_node();
void insert(struct trie_node *root, uint32_t key,
	int length, struct route_table_entry *rtable_entry);
struct route_table_entry* search(struct trie_node *root, uint32_t key);