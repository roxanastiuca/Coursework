Homework - ATM Database Management

The purpose of this homework was creating a database for an ATM (cash machine)
using only generic singly linked lists. The main features of this ATM are:
managing clients' requests, administrating their accounts and keeping a history
of all transactions made.
The database is structured like this:
- There is a main list of cards (called LC) that contains all cards; the LC
is devided in sublists (LSC), where the cards are placed based on their number.
- Each element of an LSC contains a card's information, along with its history
(another list).
- Each new card is added to the beginning of the coresponding LSC and each new
transaction is added as a string to the beginning of the card's history list.

The variable max_cards determines the maximum number of cards allowed in the
database and, also, the cards' coresponding sublist in LC. A card is added
in the LSC at the position given by:
[sum of digits of card number] % max_cards.
If the number of cards added surpasses max_cards, then max_cards is doubled
and the cards are rearranged in the new coresponding LSCs.