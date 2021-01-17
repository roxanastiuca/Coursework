/**
 * Stiuca Roxana-Elena, 325CB
 */

#pragma once

#include <stdio.h>
#include <unistd.h>
#include "skel.h"

struct route_table_entry {
	uint32_t prefix;
	uint32_t next_hop;
	uint32_t mask;
	int interface;
} __attribute__((packed));

struct arp_entry  {
	uint32_t ip;
	uint8_t mac[6];
};

uint32_t xch_bytes(uint32_t num);
struct trie_node* read_rtable();
int read_arp_table(struct arp_entry *arp_table);
// uint8_t* reverse_mac(uint8_t* mac);
