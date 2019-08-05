#include "functions_lib.h"

/**
 * Description: The operation for inserting a card in the ATM.
 * Input: The input and output files, the LC, the maximum number of cards,
 the eventual error (-1 for memory error).
 * Output: The address of the newly inserted card's information.
 **/
TCardInfo *InsertCard(
	FILE *fin, FILE *fout,
	TList LC, int max_cards,
	int *error
	) {

	long long int card_number;
	char pin[5];
	fscanf(fin, "%lld %s", &card_number, pin);

	TCardInfo *card = FindCard(LC, card_number, max_cards);

	/** + 3 spaces + 11 characters in "insert_card" **/
	int info_dim = OPERATIONS_LENGTH + C_NUMBER_LENGTH + PIN_LENGTH + 3 + 11;
	char info[info_dim];

	if(strcmp(card->pin, pin) || card->status == 'l') {
		if(card->status != 'l') {
			fprintf(fout, "Invalid PIN\n");
			card->pin_tries++;
		}

		if(card->pin_tries >= MAX_PIN_TRIES) {
			card->status = 'l';
			fprintf(fout, "The card is blocked. ");
			fprintf(fout, "Please contact the administrator.\n");
		}

		sprintf(info, "(FAIL, insert_card %016lld %s)", card_number, pin);
	} else {
		if(card->status == 'n') {
			fprintf(fout, "You must change your PIN.\n");
		}
	
		card->pin_tries = 0;
		sprintf(info, "(SUCCESS, insert_card %016lld %s)", card_number, pin);
	}

	if(AddToHistory(card, info))
		*error = -1;
	
	return card;
}

/**
 * Description: The operation for recharing a card with money.
 * Input: The input and output files, the address for the card's information.
 * Output: 0 for no errors/ -1 for memory error.
 **/
int Recharge(FILE *fin, FILE *fout, TCardInfo *card) {
	long long int card_number;
	int sum;

	fscanf(fin, "%lld %d", &card_number, &sum);

	/** + 3 spaces + 8 characters in "recharge" **/
	int info_dim = OPERATIONS_LENGTH + C_NUMBER_LENGTH + DigitsOf(sum) + 3 + 8;
	char info[info_dim];

	if((sum % 10) || (card->status == 'l')) {
		if(card->status == 'l') {
			fprintf(fout, "The card is blocked. ");
			fprintf(fout, "Please contact the administrator.\n");
		} else {
			fprintf(fout, "The added amount must be multiple of 10\n");
		}
		/** In both cases, the operation failed: **/
		sprintf(info, "(FAIL, recharge %016lld %d)", card_number, sum);
	} else {
		card->balance += sum;
		fprintf(fout, "%lld\n", card->balance);
		sprintf(info, "(SUCCESS, recharge %016lld %d)", card_number, sum);
	}

	return AddToHistory(card, info);
}

/**
 * Description: The operation for withdrawing cash from a card.
 * Input: The input and output files, the address for the card's information.
 * Output: 0 for no errors/ -1 for memory error.
 **/
int CashWithdrawal(FILE *fin, FILE *fout, TCardInfo *card) {
	long long int card_number;
	int sum;

	fscanf(fin, "%lld %d", &card_number, &sum);

	int info_dim;
	/** + 3 spaces + 15 characters in "cash_withdrawal" **/
	info_dim = OPERATIONS_LENGTH + C_NUMBER_LENGTH + DigitsOf(sum) + 3 + 15;
	char info[info_dim];

	if((sum % 10) || (sum > card->balance) || (card->status == 'l')) {
		sprintf(info, "(FAIL, cash_withdrawal %016lld %d)", card_number, sum);
		if(card->status == 'l') {
			fprintf(fout, "The card is blocked. ");
			fprintf(fout, "Please contact the administrator.\n");
		} else if(sum % 10)
			fprintf(fout, "The requested amount must be multiple of 10\n");
		else if(sum > card->balance)
			fprintf(fout, "Insufficient funds\n");
	} else {
		card->balance -= sum;
		fprintf(fout, "%lld\n", card->balance);
		sprintf(info, "(SUCCESS, cash_withdrawal %016lld %d)", card_number, sum);
	}

	return AddToHistory(card, info);
}

/**
 * Description: The operation for inquiring about a card's current balance.
 * Input: The input and output files, the address for the card's information.
 * Output: 0 for no errors/ -1 for memory error.
 **/
int BalanceInquiry(FILE *fin, FILE *fout, TCardInfo *card) {
	long long int card_number;

	fscanf(fin, "%lld", &card_number);

	/** + 2 spaces + 15 characters in "balance_inquiry" **/
	int info_dim = OPERATIONS_LENGTH + C_NUMBER_LENGTH + 2 + 15;
	char info[info_dim];

	if(card->status == 'l') {
		fprintf(fout, "The card is blocked. ");
		fprintf(fout, "Please contact the administrator.\n");
		sprintf(info, "(FAIL, balance_inquiry %016lld)", card_number);
	} else { 
		fprintf(fout, "%lld\n", card->balance);
		sprintf(info, "(SUCCESS, balance_inquiry %016lld)", card_number);
	}

	return AddToHistory(card, info);
}

/**
 * Description: The operation for transferring funds from one card to another.
 * Input: The input and output files, the address for the source's card, the
 LC, the maximum number of cards.
 * Output: 0 for no errors/ -1 for memory error.
 **/
int TransferFunds(
	FILE *fin, FILE *fout,
	TCardInfo *source,
	TList LC, int max_cards
	) {

	long long int card_number_source, card_number_dest;
	int sum;

	fscanf(fin, "%lld %lld %d", &card_number_source, &card_number_dest, &sum);

	int info_dim;
	/** + 4 spaces + 14 characters in "transfer_funds" **/
	info_dim = OPERATIONS_LENGTH + 4 + 14 + 2*C_NUMBER_LENGTH + DigitsOf(sum);
	char info_source[info_dim], info_dest[info_dim];

	if((sum % 10) || (source->status == 'l') || (source->balance < sum)) {
		if(source->status == 'l') {
			fprintf(fout, "The card is blocked. ");
			fprintf(fout, "Please contact the administrator.\n");
		} else if(sum % 10) {
			fprintf(fout, "The transferred amount must be multiple of 10\n");
		} else if(source->balance < sum) {
			fprintf(fout, "Insufficient funds\n");
		}

		/** In all 3 cases, the operation fails and it will only appear in
		the source's card history: **/
		sprintf(info_source, "(FAIL, transfer_funds %016lld %016lld %d)",
				card_number_source, card_number_dest, sum);
	} else {
		TCardInfo *dest = FindCard(LC, card_number_dest, max_cards);

		source->balance -= sum;
		dest->balance += sum;

		fprintf(fout, "%lld\n", source->balance);

		/** The operation is a success and it will also appear on the
		receiver's card history.: **/
		sprintf(info_source, "(SUCCESS, transfer_funds %016lld %016lld %d)",
				card_number_source, card_number_dest, sum);
		strcpy(info_dest, info_source);

		if(AddToHistory(dest, info_dest))
			return -1;
	}

	return AddToHistory(source, info_source);
}

/**
 * Description: The operation for changing a card's PIN.
 * Input: The input and output files, the address for the card's information.
 * Output: 0 for no errors/ -1 for memory error.
 **/
int PinChange(FILE *fin, FILE *fout, TCardInfo *card) {
	long long int card_number;
	char new_pin[5];

	fscanf(fin, "%lld %s", &card_number, new_pin);

	/** + 3 spaces + 10 characters in "pin_change" **/
	int info_dim = OPERATIONS_LENGTH + 3 + 10 + C_NUMBER_LENGTH + PIN_LENGTH;
	char info[info_dim];

	if(InvalidPin(new_pin) || card->status == 'l') {
		if(card->status == 'l') {
			fprintf(fout, "The card is blocked. ");
			fprintf(fout, "Please contact the administrator.\n");
		} else {
			fprintf(fout, "Invalid PIN\n");
		}

		sprintf(info, "(FAIL, pin_change %016lld %s)", card_number, new_pin);
	} else {
		strcpy(card->pin, new_pin);
		card->status = 'a';

		sprintf(info, "(SUCCESS, pin_change %016lld %s)", card_number, new_pin);
	}

	return AddToHistory(card, info);
}

/**
 * Description: The operation for canceling the insert of a card.
 * Input: The input file, the address for the card's information.
 * Output: 0 for no error/ -1 for memory error.
 **/
int Cancel(FILE *fin, TCardInfo *card) {
	long long int card_number;

	fscanf(fin, "%lld", &card_number);

	/** + 2 spaces + 6 characters in "cancel" **/
	int info_dim = OPERATIONS_LENGTH + 2 + 6 + C_NUMBER_LENGTH;
	char info[info_dim];

	sprintf(info, "(SUCCESS, cancel %016lld)", card_number);

	return AddToHistory(card, info);
}