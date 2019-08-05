#include "functions_lib.h"

/**
 * Description: Assign memory for a list node.
 * Input: N\A.
 * Output: NULL if no memory/ the address of the node.
 **/
TList AssignNode() {
	TList node = (TList) malloc(sizeof(TNode));
	if(node) {
		node->next = NULL;
		node->info = NULL;
	}

	return node;
}

/**
 * Description: Create the main list of cards (LC).
 * Input: The number of LSCs needed to be added.
 * Output: The start of the list.
 **/
TList CreateLC(int max_cards) {
	TList new_LC = NULL, end = NULL;

	int i;
	for(i = 0; i < max_cards; i++) {
		TList aux = AssignNode();
		if(!aux) {
			DestroyList(&new_LC);
			return NULL;
		}

		if(new_LC == NULL)
			new_LC = aux;
		else
			end->next = aux;

		end = aux;
	}

	return new_LC;
}

/**
 * Description: Calculates the index the correct LSC.
 * Input: The card number, the divided (the maximum number of cards in LC).
 * Output: The index.
 **/
int PositionInLC(long long int number, int divider) {
	int sum = 0;

	while(number) {
		sum += (number % 10);
		number /= 10;
	}

	return sum % divider;
}

/**
 * Description: Calculates the number of digits in a number.
 * Input: The given number.
 * Output: The number of digits.
 **/
int DigitsOf(long long int x) {
	int cnt = 0;

	while(x) {
		cnt++;
		x /= 10;
	}

	return cnt;
}

/**
 * Description: Verify if a card already exists in the LC.
 * Input: The number of the card, the LC, its LSC's index.
 * Output: 1 if found/ 0 if not found.
 **/
int CheckCardAlreadyExists(long long int card_number, TList LC, int position) {
	int i;
	for(i = 0; i < position; i++)
		LC = LC->next;

	TList p;
	for(p = (TList)LC->info; p != NULL; p = p->next) {
		if(((TCardInfo *)p->info)->card_number == card_number)
			return 1;
	}

	return 0;
}

/**
 * Description: Write in the output file all given card's information.
 * Input: The output file, the address for the card's information.
 * Output: N\A.
 **/
void PrintCardInfo(FILE *fout, TCardInfo *card) {
	fprintf(fout, "(card number: %016lld", card->card_number);
	fprintf(fout, ", PIN: %s, ", card->pin);
	fprintf(fout, "expiry date: %s, ", card->exp_date);
	fprintf(fout, "CVV: %03d", card->cvv);
	fprintf(fout, ", balance: %lld, ", card->balance);
	fprintf(fout, "status: ");
	if(card->status == 'n')
		fprintf(fout, "NEW, ");
	else if(card->status == 'a')
		fprintf(fout, "ACTIVE, ");
	else
		fprintf(fout, "LOCKED, ");
	fprintf(fout, "history: [");
	
	TList p = NULL;
	for(p = card->history; p != NULL && p->next != NULL; p = p->next) {
		fprintf(fout, "%s, ", (char *) p->info);
	}
	if(p != NULL)
		fprintf(fout, "%s])\n", (char *) p->info);
	else
		fprintf(fout, "])\n");
}

/**
 * Description: Find card in LC.
 * Input: LC, card number, maximum number of cards in LC.
 * Output: The address for the card's information/ NULL if not found.
 **/
TCardInfo *FindCard(TList LC, long long int card_number, int max_cards) {
	int position = PositionInLC(card_number, max_cards), i;

	/** Go to the correct LSC: **/
	for(i = 0; i < position; i++)
		LC = LC->next;

	/** Search for card by its number: **/
	TList p;
	for(p = (TList)LC->info; p != NULL; p = p->next)
		if(card_number == ((TCardInfo *)p->info)->card_number)
			break;

	if(!p)
		return NULL;

	return (TCardInfo *)p->info;
}

/**
 * Description: Verify if a PIN is valid.
 * Input: the PIN.
 * Output: -1 if not valid/ 0 if valid.
 **/
int InvalidPin(char *pin) {
	int length = strlen(pin), i;

	if(length != 4)
		return -1;

	for(i = 0; i < length; i++)
		if(pin[i] > '9' || pin[i] < '0')
			return -1;

	return 0;
}

/**
 * Description: Add a node to a card's history.
 * Input: The address of the card's information, the string that must be added
 in history.
 * Output: 0 for no errors/ -1 for memory error.
 **/
int AddToHistory(TCardInfo *card, char *info) {
	/** Assign empty node: **/
	TList aux = AssignNode();
	if(!aux)
		return -1;

	/** Assign memory for the string: **/
	int dim = sizeof(char) * (strlen(info) + 1);
	aux->info = malloc(dim);
	if(!aux->info) {
		free(aux);
		return -1;
	}

	memcpy(aux->info, info, dim);

	/** Insert node at the beginning of the history list: **/
	aux->next = card->history;
	card->history = aux;

	return 0;
}

/**
 * Description: Search for given string and delete history node that contains
 it.
 * Input: The address for the card's information, the searched string.
 * Output: N\A.
 **/
void RemoveFromHistory(TCardInfo *card, char *info) {
	TList ant, p;

	for(ant = NULL, p = card->history; p != NULL; ant = p, p = p->next) {
		if(strcmp(info, (char *)p->info) == 0) {
			break;
		}
	}

	if(p != NULL) {
		if(ant != NULL)
			ant->next = p->next;
		else
			card->history = p->next;

		free(p->info);
		free(p);
	}
}

/**
 * Description: Free memory in a LSC list node.
 * Input: The node.
 * Output:
 **/
void FreeCard(TList p) {
	TCardInfo *card = p->info;
	TList h = card->history, aux;

	while(h) {
		aux = h;
		h = h->next;

		free(aux->info);
		free(aux);
	}

	free(card);
	free(p);
}

/**
 * Description: Free memory for LC.
 * Input: The address of the LC.
 * Output: N\A.
 **/
void DestroyList(AList aLC) {
	TList aux_LC;

	/** Move through LC: **/
	while(*aLC) {
		aux_LC = *aLC;
		*aLC = (*aLC)->next;

		 TList aux_LSC, LSC = (aux_LC)->info;

		/** Move through LSC: **/
		while(LSC) {
			aux_LSC = LSC;
			LSC = LSC->next;

			FreeCard(aux_LSC);
		}

		free(aux_LC);
	}
}

/**
 * Description: Add a card in the LC.
 * Input: LC, the LSC's index, the card's information.
 * Output: 0 for no errors/ -1 for memory error.
 **/
int InsertCardInLC(TList LC, int position, TCardInfo card_info) {
	TList aux = AssignNode();
	if(!aux)
		return -1;

	aux->info = malloc(sizeof(TCardInfo));
	if(!aux->info) {
		free(aux);
		return -1;
	}

	memcpy(aux->info, &card_info, sizeof(TCardInfo));

	int i;
	for(i = 0; i < position; i++)
		LC = LC->next;

	/** Insert at the beginning of list: **/
	aux->next = LC->info;
	LC->info = aux;

	return 0;
}

/**
 * Descriere: Functia corespunde operatiei de redimensionare a bazei de date.
 * Input: Adresa listei vechi, adresa numarului maxim de carduri vechi.
 * Output: -1 daca alocarea noii liste nu are succes/ numar pozitiv ce
 reprezinta cel mai mare indice al unei subliste in care este adaugat un card.
 **/
/**
 * Description: The operation for resizing of the database.
 * Input: The address of the old LC, the maximum number of cards.
 * Output: -1 if the resizing was not possible/ a positive number that
 represents the highest index of a non-empty LSC in the new database.
 **/
int ExtendDatabase(AList aLC, int *max_cards) {
	(*max_cards) *= 2;

	/** Create new LC with the extended dimension: */
	TList new_LC = CreateLC(*max_cards);

	int max_position = -1;

	/** Move though LC and then through each LSC and add each card to the LC: **/
	TList LC, p;

	for(LC = *aLC; LC != NULL; LC = LC->next) {
		for(p = (TList)LC->info; p != NULL; p = p->next) {
			TCardInfo *card_info = (TCardInfo *)p->info;

			int position = PositionInLC(card_info->card_number, *max_cards);
			if(position > max_position)
				max_position = position;

			/** If there is not enough memory, free all memory assinged for
			the new LC: **/
			if(InsertCardInLC(new_LC, position, *card_info)) {
				DestroyList(&new_LC);
				return -1;
			}

			/** The history list for the card in the old LC and in the new LC
			have the same address, so it must only be freed once (at the final
			freeing of all LC).**/
			card_info->history = NULL;
		}
	}

	/** Free all memory for old LC and move new LC in place: **/
	DestroyList(aLC);
	*aLC = new_LC;

	return max_position;
}