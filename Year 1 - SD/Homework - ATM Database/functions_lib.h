#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define INPUT_FILE_NAME "input.in"
#define OUTPUT_FILE_NAME "output.out"

#define OPERATIONS_LENGTH 21
#define C_NUMBER_LENGTH 16
#define PIN_LENGTH 4
#define MAX_PIN_TRIES 3

typedef struct node{
	void *info;
	struct node *next;
} TNode, *TList, **AList;

typedef struct card_info{
	long long int card_number;
	char pin[5];
	char exp_date[6];
	int cvv;
	long long int balance;
	char status;
	int pin_tries;
	TList history;
} TCardInfo;

/** utils.c **/
TList AssignNode();
TList CreateLC(int max_cards);
int PositionInLC(long long int number, int divider);
int DigitsOf(long long int x);
int CheckCardAlreadyExists(long long int card_number, TList LC, int position);
void PrintCardInfo(FILE *fout, TCardInfo *card);
TCardInfo *FindCard(TList LC, long long int card_number, int max_cards);
int InvalidPin(char *pin);
int AddToHistory(TCardInfo *card, char *info);
void RemoveFromHistory(TCardInfo *card, char *info);
void FreeCard(TList p);
void DestroyList(AList aLC);
int InsertCardInLC(TList LC, int position, TCardInfo card_info);
int ExtendDatabase(AList aLC, int *max_cards);

/** admin_operations.c **/
int AddCard(
	FILE *fin,
	AList aLC,
	int *max_cards, int *position, int *count_card
	);
void DeleteCard(FILE *fin, TList LC, int max_cards);
int ReverseTransaction(FILE *fin, FILE *fout, TList LC, int max_cards);
void UnblockCard(FILE *fin, TList LC, int max_cards);
void ShowCards(
	FILE *fin, FILE *fout,
	TList LC,
	int max_cards, int max_position
	);

/** client_operations.c **/
TCardInfo *InsertCard(
	FILE *fin, FILE *fout,
	TList LC, int max_cards,
	int *error
	);
int Recharge(FILE *fin, FILE *fout, TCardInfo *card);
int CashWithdrawal(FILE *fin, FILE *fout, TCardInfo *card);
int BalanceInquiry(FILE *fin, FILE *fout, TCardInfo *card);
int TransferFunds(
	FILE *fin, FILE *fout,
	TCardInfo *source,
	TList LC, int max_cards
	);
int PinChange(FILE *fin, FILE *fout, TCardInfo *card);
int Cancel(FILE *fin, TCardInfo *card);