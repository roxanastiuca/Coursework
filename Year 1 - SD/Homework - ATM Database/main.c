#include "functions_lib.h"

int main() {
	FILE *fin = fopen(INPUT_FILE_NAME, "rt");
	FILE *fout = fopen(OUTPUT_FILE_NAME, "wt");

	if((fin == NULL) || (fout == NULL)) {
		fprintf(stderr, "Cannot open files.\n");
		return -1;
	}

	int max_cards, count_cards = 0, max_position = -1;
	fscanf(fin, "%d\n", &max_cards);

	TList LC = CreateLC(max_cards);
	if(LC == NULL) {
		fprintf(stderr, "%s\n", "Cannot create LC.\n");
		return -1;
	}

	char operation[OPERATIONS_LENGTH];

	/** For efficiency, I keep the address of the current inserted card so
	that all operations after insert_card have a direct pointer to it: **/
	TCardInfo *current_card = NULL;

	while(fscanf(fin, "%s", operation) == 1) {
		int error = 0;

		if(strcmp(operation, "add_card") == 0) {
			int position;
			int aux_error = AddCard(fin, &LC,
						&max_cards, &position, &count_cards);
			if(aux_error == -1) {
				// fprintf(stderr, "%s\n", MEM_ERROR);
				// break; STERGE ASTA DACA E OK//////////////////////////////////
				error = -1;
			} else if(aux_error == 1) {
				fprintf(fout, "The card already exists\n");
			} else {
				/** The new card was added successfully. **/
				count_cards++;
			}

			/** Save the index of the last LSC in which a card was added: **/
			if(max_position < position)
				max_position = position;

		} else if(strcmp(operation, "show") == 0) {
			ShowCards(fin, fout, LC, max_cards, max_position);

		} else if(strcmp(operation, "delete_card") == 0) {
			DeleteCard(fin, LC, max_cards);
			/** Number of cards in LC decreases: **/
			count_cards--;

		} else if(strcmp(operation, "reverse_transaction") == 0) {
			if(ReverseTransaction(fin, fout, LC, max_cards)) {
				error = -1;
			}

		} else if(strcmp(operation, "unblock_card") == 0) {
			UnblockCard(fin, LC, max_cards);

		} else if(strcmp(operation, "insert_card") == 0) {
			/** Function InsertCard also returns (side effect) the eventual
			memory error due to the allocation for the card's history. **/

			int aux_error = 0;
			current_card = InsertCard(fin, fout, LC, max_cards, &aux_error);
			if(aux_error) {
				error = -1;
			}

		} else if(strcmp(operation, "recharge") == 0) {
			if(Recharge(fin, fout, current_card)) {
				error = -1;
			}
			/** Card is disconnected: **/
			current_card = NULL;

		} else if(strcmp(operation, "cash_withdrawal") == 0) {
			if(CashWithdrawal(fin, fout, current_card)) {
				error = -1;
			}
			current_card = NULL;

		} else if(strcmp(operation, "balance_inquiry") == 0) {
			if(BalanceInquiry(fin, fout, current_card)) {
				error = -1;
			}
			current_card = NULL;

		} else if(strcmp(operation, "pin_change") == 0) {
			if(PinChange(fin, fout, current_card)) {
				error = -1;
			}
			current_card = NULL;

		} else if(strcmp(operation, "cancel") == 0) {
			if(Cancel(fin, current_card)) {
				error = -1;
			}
			current_card = NULL;

		} else if(strcmp(operation, "transfer_funds") == 0) {
			if(TransferFunds(fin, fout, current_card, LC, max_cards)) {
				error = -1;
			}
			current_card = NULL;
		}

		if(error) {
			fprintf(stderr, "Insufficient memory.\n");
			break;
		}
	}

	/** Close files and free all memory used: **/
	fclose(fin);
	fclose(fout);
	DestroyList(&LC);

	return 0;
}