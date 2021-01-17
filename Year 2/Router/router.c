/**
 * Stiuca Roxana-Elena, 325CB
 */

#include "skel.h"
#include "parser.h"
#include "trie.h"
#include "queue.h"

/* Tabela ARP si dimensiunea ei. */
struct arp_entry *arp_table;
int arp_table_size;

/* Arbor trie cu tabela de rutare. */
struct trie_node *rtable;

/* Coada de pachete in asteptare. */
queue packets_queue;

/* Sequence number. Folosit doar pentru a seta fiecare pachet cu
alt numar. */
int seq_number;

/**
 * Descriere: Functia gaseste in tabela de rutare cea mai buna
 ruta pentru a ajunge la un IP.
 * Input: IP-ul pentru care cautam.
 * Output: intrarea din tabela sau NULL daca nu a fost gasit.
 */
struct route_table_entry *get_best_route(uint32_t dest_ip) {
	return search(rtable, ntohl(dest_ip));
}

/**
 * Descriere: Functia cauta in tabela ARP intrarea cu IP-ul dat.
 * Input: IP-ul pe care il cauta in tabela.
 * Output: intrarea din tabela sau NULL daca adresa IP nu exista
 in tabela.
 */
struct arp_entry *get_arp_entry(uint32_t ip) {
    for(int i = 0; i < arp_table_size; i++) {
    	if(arp_table[i].ip == ip) {
    		return &arp_table[i];
    	}
    }

    return NULL;
}

/**
 * Descriere: Functia adauga in tabela ARP o noua intrare.
 * Input: IP-ul noii intrai si adresa sa MAC.
 * Output: N\A.
 */
void update_arp_table(uint8_t ipv4[4], uint8_t mac[6]) {
	uint32_t ip = ipv4[0] + (ipv4[1] << 8) + (ipv4[2] << 16) + (ipv4[3] << 24);

	arp_table[arp_table_size].ip = ip;
	memcpy(arp_table[arp_table_size].mac, mac, 6);
	arp_table_size++;
}

/**
 * Descriere: Functie care parcurge coada cu pachete in asteptare si verifica
 daca acum se stie adresa MAC destinatie a lor. Daca da, le trimite, daca nu,
 le pastreaza in coada.
 * Input: N\A.
 * Output: N\A.
 */
void send_pending_packets() {
	/* Initializez o coada auxiliara. */
	queue aux = queue_create();

	/* Parcurg toata coada. */
	while(!queue_empty(packets_queue)) {
		/* Extrag cate un pachet din coada. */
		packet *m = ((packet *)queue_deq(packets_queue));

		/* Extrag headerele Ethernet si IP ale pachetului. */
		struct ether_header *eth_hdr = (struct ether_header*)m->payload;
		struct iphdr *ip_hdr = (struct iphdr*)(m->payload + 
							sizeof(struct ether_header));

		/* Gasesc in tabela de rutare cea mai buna ruta catre destinatia
		acestui pachet si caut in tabela ARP adresa MAC a next_hop-ului. */
		struct route_table_entry *best_route = get_best_route(ip_hdr->daddr);
		struct arp_entry *arpentry = get_arp_entry(best_route->next_hop);

		if(arpentry == NULL) {
			/* Nu cunosc inca adresa MAC, deci pastrez pachetul in coada. */
			queue_enq(aux, m);
		} else {
			/* Cunosc acum adresa MAC, deci pot seta MAC-ul destinatiei.
			Trimit pachetul. */
			memcpy(eth_hdr->ether_dhost, arpentry->mac, 6);
			send_packet(best_route->interface, m);
		}
	}

	/* Pun inapoi din coada auxiliara in coada pentru pachete in asteptare. */
	while(!queue_empty(aux)) {
		queue_enq(packets_queue, queue_deq(aux));
	}
}

/**
 * Descriere: adaug un pachet in coada de pachete in asteptare.
 * Input: pachetul.
 * Output: N\A.
 */
void add_to_pending_packets(packet m) {
	/* Am nevoie de un aux astfel incat sa nu adaug mereu in coada
	aceeasi adresa. */
	packet *aux = (packet *)malloc(sizeof(packet));
	aux->len = m.len;
	memcpy(aux->payload, m.payload, MAX_LEN);
	aux->interface = m.interface;

	/* Adaug pachetul in coada. */
	queue_enq(packets_queue, aux);
}

/**
 * Descriere: Functie care trimite un ARP Request pentru a afla
 adresa hardware pentru un IP dat.
 * Input: adresa IP a host-ului caruia vrem sa-i aflam MAC-ul.
 * Output: N\A.
 */
void send_arp_request(uint32_t ip) {
	const uint8_t BROADCAST[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

	/* Caut in tabela de rutare ruta cea mai buna pana la acel IP.  */
	struct route_table_entry *best_route = get_best_route(ip);

	/* Trimit in m un ARP Request. */
	packet m;
	m.len = sizeof(struct ether_header) + sizeof(struct ether_arp);
	m.interface = best_route->interface;

	/* Extrag headerul de Ethernet. */
	struct ether_header *eth_hdr = (struct ether_header*)m.payload;

	/* Setez adresa MAC a sender-ului. */
	get_interface_mac(best_route->interface, eth_hdr->ether_shost);

	/* Setez adresa MAC destinatie ca fiind broadcast, ff:ff:ff:ff:ff:ff */
	memcpy(eth_hdr->ether_dhost, BROADCAST, 6);

	/* Pachetul este de tip ARP. */
	eth_hdr->ether_type = htons(ETH_P_ARP);

	/* Extrag headerul ARP din pachet si setez campurile. */
	struct ether_arp *arp_hdr = (struct ether_arp*)(m.payload + 
								sizeof(struct ether_header));
	arp_hdr->ea_hdr.ar_hrd = htons(1);
	arp_hdr->ea_hdr.ar_pro = htons(ETH_P_IP);
	arp_hdr->ea_hdr.ar_hln = 6;
	arp_hdr->ea_hdr.ar_pln = 4;
	arp_hdr->ea_hdr.ar_op = htons(ARPOP_REQUEST);

	/* Setez adresele senderului. */
	get_interface_mac(best_route->interface, arp_hdr->arp_sha);
	uint32_t spa = inet_addr(get_interface_ip(best_route->interface));
	memcpy(arp_hdr->arp_spa, &spa, 4);

	/* Setez adresa hardware a destinatiei. */
	memcpy(arp_hdr->arp_tpa, &ip, 4);

	/* Trimit pachetul. */
	send_packet(best_route->interface, &m);
}

/**
 * Descriere: Functie care realizeaza pasii corespunzatori primirii unui
 pachet ARP.
 * Input: pachetul primit.
 * Ouput: N\A.
 */
void handle_arp_packet(packet m) {
	/* Extrag headerul Ethernet din pachet. */
	struct ether_header *eth_hdr = (struct ether_header*)m.payload;

	/* Extrag headerul ARP din pachet. */
	struct ether_arp *arp_hdr = (struct ether_arp*)(m.payload + 
							sizeof(struct ether_header));

	/* Verific ce tip de pachet ARP este, request sau reply. */
	if(ntohs(arp_hdr->ea_hdr.ar_op) == ARPOP_REQUEST) {
		/* Am primit un pachet ARP Request. Ii trimit inapoi un ARP Reply cu
		adresa hardware. */
		packet reply;

		reply.len = sizeof(struct ether_header) + sizeof(struct ether_arp);

		/* Trimit pe aceeasi interfata pe care am primit pachetul request. */
		reply.interface = m.interface;

		/* Extrag headerul de Ethernet al pachetului raspuns. */
		struct ether_header *reply_eth = (struct ether_header*)reply.payload;

		/* Trimit un pachet de tip ARP. */
		reply_eth->ether_type = htons(ETH_P_ARP);

		/* Destinatia pachetului raspuns este sursa pachetului primit. */
		memcpy(reply_eth->ether_dhost, eth_hdr->ether_shost, 6);
		
		/* Iau MAC-ul interfetei si il setez ca sursa mesajului. */
		uint8_t mac[6];
		get_interface_mac(m.interface, mac);
		memcpy(reply_eth->ether_shost, mac, 6);

		/* Extrag headerul ARP al pachetului raspuns. */
		struct ether_arp *reply_arp = (struct ether_arp*)(reply.payload +
										sizeof(struct ether_header));

		/* Setez campurile headerului de ARP. */
		reply_arp->ea_hdr.ar_hrd = arp_hdr->ea_hdr.ar_hrd;
		reply_arp->ea_hdr.ar_pro = arp_hdr->ea_hdr.ar_pro;
		reply_arp->ea_hdr.ar_hln = 6;
		reply_arp->ea_hdr.ar_pln = 4;

		/* Pachetul este de tip ARP Reply. */
		reply_arp->ea_hdr.ar_op = htons(ARPOP_REPLY);

		/* Setez adresele hardware si IP ale sursei, adica ale routerului. */
		memcpy(reply_arp->arp_sha, mac, 6);
		memcpy(reply_arp->arp_spa, arp_hdr->arp_tpa, 4);

		/* Setez adresele hardware si IP destinatie ale raspunsului, fiind
		egale cu adresele sursa ale pachetului primit. */
		memcpy(reply_arp->arp_tha, arp_hdr->arp_sha, 6);
		memcpy(reply_arp->arp_tpa, arp_hdr->arp_spa, 4);

		/* Trimit raspunsul. */
		send_packet(m.interface, &reply);
		return;
	} else if(ntohs(arp_hdr->ea_hdr.ar_op) == ARPOP_REPLY) {
		/* Am primit un ARP Reply. Fac update in tabela de ARP si trimit
		pachetele din asteptare. */
		update_arp_table(arp_hdr->arp_spa, arp_hdr->arp_sha);
		send_pending_packets();
		return;
	}
}

/**
 * Descriere: Functie care trimite un pachet ICMP inapoi sursei care
 a trimis pachetul m.
 * Input: m este pachetul la care trimit raspuns, type este tipul
 pachetului ICMP (ICMP_ECHOREPLY, ICMP_TIME_EXCEEDED, ICMP_DEST_UNREACH).
 * Output: N\A.
 */
void send_icmp_reply(packet m, uint8_t type) {
	packet reply;

	reply.len = sizeof(struct ether_header) + sizeof(struct iphdr) +
				sizeof(struct icmphdr);

	/* Trimit raspunsul pe aceeasi interfata pe care a venit. */
	reply.interface = m.interface;

	/* Extrag headerele Ethernet din ambele pachete. */
	struct ether_header *eth_hdr = (struct ether_header*)m.payload;
	struct ether_header *reply_eth = (struct ether_header*)reply.payload;

	/* Sursa si destinatia pachetului trimis sunt destinatia si sursa
	pachetului primit. */
	memcpy(reply_eth->ether_dhost, eth_hdr->ether_shost, 6);
	memcpy(reply_eth->ether_shost, eth_hdr->ether_dhost, 6);

	/* Raspunsul este un pachet IP. */
	reply_eth->ether_type 	= htons(ETH_P_IP);

	/* Extrag headerele IP din ambele pachete. */
	struct iphdr *ip_hdr = (struct iphdr*)(m.payload + 
						sizeof(struct ether_header));
	struct iphdr *reply_ip = (struct iphdr*)(reply.payload + 
						sizeof(struct ether_header));

	/* Setez campurile IP ale raspunsului. */
	reply_ip->version = 4;
	reply_ip->ihl = 5;
	reply_ip->tos = 0;
	reply_ip->tot_len = htons(sizeof(struct iphdr) + sizeof(struct icmphdr));
	reply_ip->id = htons(seq_number);
	reply_ip->frag_off = 0;
	reply_ip->ttl = 64;
	reply_ip->protocol = IPPROTO_ICMP;
	reply_ip->saddr	= ip_hdr->daddr;
	reply_ip->daddr	= ip_hdr->saddr;

	/* Updatez checksum-ul. */
	reply_ip->check = 0;
	reply_ip->check = checksum(reply_ip, sizeof(struct iphdr));

	/* Extrag campul de ICMP. */
	struct icmphdr *reply_icmp = (struct icmphdr*)(reply.payload +
				sizeof(struct ether_header) + sizeof(struct iphdr));

	/* Setez campurile ICMP ale raspunsului. */
	reply_icmp->code = 0;
	reply_icmp->type = type;
	reply_icmp->un.echo.id = htons(seq_number);
	reply_icmp->un.echo.sequence = htons(seq_number);
	reply_icmp->checksum = 0;
	reply_icmp->checksum = checksum(reply_icmp, sizeof(struct icmphdr));

	/* Trimit pachetul raspuns. */
	send_packet(m.interface, &reply);
}

/**
 * Descriere: Functie care realizeaza pasii corespunzatori primirii
 unui pachet IP.
 * Input: pachetul primit.
 * Output: N\A.
 */
void handle_ip_packet(packet m) {
	/* Extrag headerele de Ethernet si IP. */
	struct ether_header *eth_hdr = (struct ether_header*)m.payload;
	struct iphdr *ip_hdr = (struct iphdr*)(m.payload +
						sizeof(struct ether_header));

	/* Verific daca pachetul este destinat routerului. */
	if(inet_addr(get_interface_ip(m.interface)) == ip_hdr->daddr) {
		/* Verific ca este un pachet de tip ICMP. Daca nu, il arunc. */
		if(ip_hdr->protocol != IPPROTO_ICMP) {
			return;
		}

		/* Extrag headerul de ICMP. */
		struct icmphdr *icmp_hdr = (struct icmphdr*)(m.payload + 
					sizeof(struct ether_header) + sizeof(struct iphdr));

		/* Verific ca este un pachet ICMP Echo Request. Daca nu, il arunc. */
		if(icmp_hdr->type != ICMP_ECHO) {
			return;
		}

		/* Trimit inapoi un pachet ICMP Echo Reply. */
		send_icmp_reply(m, ICMP_ECHOREPLY);
		return;
	}

	/* Verific ca checksum-ul pachetului primit este corect. Daca nu, il
	arunc. */
	if(checksum(ip_hdr, sizeof(struct iphdr)) != 0) {
		return;
	}

	/* Scad TTL si updatez checksum-ul. */
	ip_hdr->ttl--;
	ip_hdr->check = 0;
	ip_hdr->check = checksum(ip_hdr, sizeof(struct iphdr));

	/* Verific ca pachetul nu este expirat. Daca este, trimit pachet
	ICMP TIME EXCEEDED si arunc pachetul. */
	if(ip_hdr->ttl <= 1) {
		send_icmp_reply(m, ICMP_TIME_EXCEEDED);
		return;
	}

	/* Gasesc din tabela de rutare cea mai buna ruta la destinatie. */
	struct route_table_entry *best_route = get_best_route(ip_hdr->daddr);

	/* Verific ca sa existe o ruta catre adresa destinatie. Daca nu exista,
	trimit ICMP DEST UNREACHABLE si arunc pachetul. */
	if(best_route == NULL) {
		send_icmp_reply(m, ICMP_DEST_UNREACH);
		return;
	}

	/* Caut in tabela ARP intrarea corespunzatoare next_hop-ului. */
	struct arp_entry *arpentry = get_arp_entry(best_route->next_hop);

	/* Verific ca am gasit o intrare ARP. Daca nu, adaug pachetul in coada
	de pachete in asteptare si trimit un ARP Request. */
	if(arpentry == NULL) {
		send_arp_request(ip_hdr->daddr);
		add_to_pending_packets(m);
		return;
	}

	/* Setez adresa hardware destinatie a pachetului. */
	memcpy(eth_hdr->ether_dhost, arpentry->mac, 6);

	/* Fac forward la pachet. */
	send_packet(best_route->interface, &m);
}

/**
 * Descriere: functia main a programului, care initializeaza routerul
 si primeste intr-o bucla infinita pachete de la host-uri.
 */
int main(int argc, char *argv[])
{
	packet m;
	int rc;

	init();

	/* Initializez tabela ARP, care momentan nu are nicio intrare. */
	arp_table = malloc(sizeof(struct arp_entry) * 100);
	arp_table_size = 0;

	/* Citesc tabela de rutare si o pun intr-o structura trie. */
	rtable = read_rtable();

	/* Initializez coada pachetelor in asteptare. */
	packets_queue = queue_create();

	/* Resetez sequence number. */
	seq_number = 0;

	while (1) {
		rc = get_packet(&m);
		DIE(rc < 0, "get_message");

		/* Extrag headerul de Ethernet. */
		struct ether_header *eth_hdr = (struct ether_header*)m.payload;

		if(ntohs(eth_hdr->ether_type) == ETH_P_IP) {
			/* Am primit un pachet IP. */
			handle_ip_packet(m);
		} else if(ntohs(eth_hdr->ether_type) == ETH_P_ARP) {
			/* Am primit un pachet ARP. */
			handle_arp_packet(m);
		}

		seq_number++;
	}
}