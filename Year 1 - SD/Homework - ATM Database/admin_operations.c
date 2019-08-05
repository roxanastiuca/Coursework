#include "functions_lib.h"

/**
 * Description: The operation for adding a card in the LC.
 * Input: The input file, the address for the LC, the maximum number of cards,
 the index of the card's LSC and the current number of cards in LC.
 * Output: 0 for no errors/ -1 for memory error/ 1 for "The card already
 exists".
 **/
int AddCard(
	FILE *fin,
	AList aLC,
	int *max_cards, int *position, int *count_cards
	) {

	TCardInfo card_info;
	fscanf(fin, "%lld %s %s %d\n", &card_info.card_number, card_info.pin,
						card_info.exp_date, &card_info.cvv);
	card_info.balance = card_info.pin_tries = 0;
	card_info.status = 'n';
	card_info.history = NULL;

	int aux_position = PositionInLC(card_info.card_number, *max_cards);
	*position = aux_position;

	if(CheckCardAlreadyExists(card_info.card_number, *aLC, *position) == 1)
		return 1;

	if(*count_cards == *max_cards) {
		int aux = ExtendDatabase(aLC, max_cards);
		if(aux == -1)
			return -1;
		if(aux > *position)
			*position = aux;

		aux_position = PositionInLC(card_info.card_number, *max_cards);
		if(aux_position > *position)
			*position = aux_position;
	}
	
	return InsertCardInLC(*aLC, aux_position, card_info);
}

/**
 * Description: The operation for deleting a card from the database.
 * Input: The input file, the main list (LC), the maximum number of cards. 
 * Output: N\A.
 **/
void DeleteCard(FILE *fin, TList LC, int max_cards) {
	long long int card_number;
	fscanf(fin, "%lld", &card_number);

	int position = PositionInLC(card_number, max_cards), i;

	for(i = 0; i < position; i++)
		LC = LC->next;

	TList p, ant;
	for(p = (TList)LC->info, ant = NULL; p != NULL; ant = p, p = p->next)
		if(card_number == ((TCardInfo *)p->info)->card_number)
			break;


	if(p != NULL) {
		if(ant == NULL) {
			if(p->next != NULL)
				LC->info = p->next;
			else {
				LC->info = NULL;
			}
		}
		else
			ant->next = p->next;

		FreeCard(p);
	}
}

/**
 * Description: The operation for reversing a transaction.
 * Input: The input and output files, the main list (LC), the maximum number
 of cards.
 * Output: 0 for no errors/ -1 for memory error.
 **/
int ReverseTransaction(FILE *fin, FILE *fout, TList LC, int max_cards) {
	long long int card_number_source, card_number_dest;
	int sum;

	fscanf(fin, "%lld %lld %d", &card_number_source, &card_number_dest, &sum);

	TCardInfo *source = FindCard(LC, card_number_source, max_cards);
	TCardInfo *dest = FindCard(LC, card_number_dest, max_cards);

	char info_source[100], info_dest[100];

	if(dest->balance < sum) {
		fprintf(fout, "The transaction cannot be reversed\n");
	} else {
		source->balance += sum;
		dest->balance -= sum;

		sprintf(info_source, "(SUCCESS, reverse_transaction %lld %lld %d)",
				card_number_source, card_number_dest, sum);
		if(AddToHistory(source, info_source))
			return -1;

		sprintf(info_dest, "(SUCCESS, transfer_funds %lld %lld %d)",
				card_number_source, card_number_dest, sum);

		RemoveFromHistory(dest, info_dest);
	}

	return 0;
}

/**
 * Description: The operation for unblocking a card.
 * Input: The input file, the main list (LC), the maximum number of cards.
 * Output: N\A.
 **/
void UnblockCard(FILE *fin, TList LC, int max_cards) {
	long long int card_number;

	fscanf(fin, "%lld", &card_number);

	TCardInfo *card = FindCard(LC, card_number, max_cards);

	card->status = 'a';
	card->pin_tries = 0;
}

/**
 * Description: The operation for showing all information about one card
 or all cards.
 * Input: The input and output files, the main list (LC), the maximum number
 of cards, the index of the last non-empty LSC.
 * Output: N\A.
 **/
void ShowCards(
	FILE *fin, FILE *fout,
	TList LC,
	int max_cards, int max_position
	) {
	
	long long int card_number;

	if(fscanf(fin, "%lld", &card_number) == 1) {
		TCardInfo *card = FindCard(LC, card_number, max_cards);
		PrintCardInfo(fout, card);
	} else {
		int i;

		for(i = 0; LC != NULL && i <= max_position; LC = LC->next, i++) {
			fprintf(fout, "pos%d: [", i);
			if((TList)LC->info != NULL) {
				fprintf(fout, "\n");
				TList p;
				
				for(p = (TList)LC->info; p != NULL; p = p->next) {
					PrintCardInfo(fout, (TCardInfo *)p->info);
				}
			}
			fprintf(fout, "]\n");
		}
	}
}