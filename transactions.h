#pragma once

typedef struct transaction {
	double value;
	char sender[62];
	char reciever[62];
	struct transaction* next;
	int senderID;
	int recieverID;
}TRANS;

typedef struct linkedListTransactions {
	TRANS* head;
	TRANS* tail;
	int count;
}LLTRANS;

const LLTRANS* printTransactions(const LLTRANS* const trans);
LLTRANS* showTransactions(const ACCOUNT* const acc);
LLTRANS* findTransByName(LLTRANS* trans);
LLTRANS* findTransByID(LLTRANS* trans);
LLTRANS* readTransByID(const int id);
TRANS* createTrans(ACCOUNT* const sender, ACCOUNT* const reciever, const double value);
void registerTransaction(TRANS* trans);