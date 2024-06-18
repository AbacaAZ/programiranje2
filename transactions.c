#ifndef _CRT_SECURE_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "bank_account.h"
#include "transactions.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern const ACCOUNT* currAcc;
extern const LLTRANS* currTrans;

TRANS* createTrans(ACCOUNT* const sender, ACCOUNT* const reciever, const double value) {
	if (!sender || !reciever) {
		system("cls");
		return NULL;
	}

	if (sender->ID == reciever->ID) {
		system("cls");
		printf("Can't transfer balance to yourself\n\n");
		return NULL;
	}

	if (value <= 0) {
		system("cls");
		printf("Transfer value has to be higher than 0\n\n");
		return NULL;
	}

	TRANS* trans = NULL;
	trans = (TRANS*)calloc(1, sizeof(TRANS));
	if (!trans) {
		perror("Failed to allocate memory for transaction");
		return NULL;
	}

	if (sender->balance < value) {
		safeFree(&trans);
		system("cls");
		printf("Not enough balance on sender account\n\n");
		return NULL;
	}

	strcat(trans->sender, sender->name);
	strcat(trans->sender, " ");
	strcat(trans->sender, sender->surname);
	trans->senderID = sender->ID;
	
	strcat(trans->reciever, reciever->name);
	strcat(trans->reciever, " ");
	strcat(trans->reciever, reciever->surname);
	trans->recieverID = reciever->ID;

	trans->value = value;

	if (sender->ID == 0) {
		reciever->balance += value;
		updateAccount(reciever);
	}
	else if(reciever->ID == 0){
		system("cls");
		printf("Failed to complete the transaction\n\n");
		safeFree(&trans);
		return NULL;
	}
	else {
		sender->balance -= value;
		reciever->balance += value;
		updateAccount(sender);
		updateAccount(reciever);
	}
	

	return trans;
}

void registerTransaction(TRANS* trans) {
	if (!trans) {
		return;
	}

	FILE* fp = NULL;
	fp = fopen("transactions.bin", "rb+");
	if (!fp) {
		perror("Failed to open file");
		exit(EXIT_FAILURE);
	}

	TRANS temp = { 0 };
	size_t nmRead = 0;

	do {
		nmRead = fread(&temp, sizeof(TRANS), 1, fp);
	} while (nmRead);

	fwrite(trans, sizeof(TRANS), 1, fp);
	safeFree(&trans);

	fclose(fp);
}

LLTRANS* readTransByID (const int id) {
	TRANS* current = (TRANS*)calloc(1, sizeof(TRANS));
	if (!current) {
		return NULL;
	}

	LLTRANS* foundByID = (LLTRANS*)calloc(1, sizeof(LLTRANS));
	if (!foundByID) {
		return NULL;
	}
	foundByID->head = current;
	foundByID->count = 0;
	size_t nmRead = 0;

	FILE* fp = NULL;
	fp = fopen("transactions.bin", "rb+");
	if (!fp) {
		perror("Failed to open file");
		exit(EXIT_FAILURE);
	}

	while (current) {
		nmRead = fread(current, sizeof(TRANS), 1, fp);
		if (nmRead) {
			if (current->recieverID == id || current->senderID == id) {
				foundByID->count++;
				current->next = (TRANS*)calloc(1, sizeof(TRANS));
				foundByID->tail = current;
				current = current->next;
			}
		}
		else {
			safeFree(&current);
			if (!foundByID->count) {
				safeFree(&foundByID);
				break;
			}

			if (foundByID->tail) {
				foundByID->tail->next = NULL;
			}
			
			break;
		}
	};

	fclose(fp);
	return foundByID;
}

LLTRANS* findTransByID(LLTRANS* trans) {
	if (!trans || !trans->head) {
		return NULL;
	}

	const int id = inputNumber();
	
	if (id == -1) {
		system("cls");
		return NULL;
	}

	TRANS* current = trans->head;
	TRANS* prev = trans->head;

	while (current && !(current->recieverID == id || current->senderID == id)) {
		prev = current->next;
		safeFree(&current);
		current = prev;
		
		trans->head = current;
	}
	
	while (current) {
		if (current->recieverID == id || current->senderID == id) {
			prev = current;
			current = current->next;
		}
		else {
			prev->next = current->next;
			safeFree(&current);
			current = prev->next;
		}
	}

	trans->tail = prev;

	if (!trans->head) {
		safeFreeLL(&trans);
		system("cls");
		printf("No transactions found associated with that ID\n\n");
		return NULL;
	}

	return trans;
}

LLTRANS* findTransByName(LLTRANS* trans) {
	if (!trans || !trans->head) {
		return NULL;
	}

	TRANS* current = trans->head;
	TRANS* prev = trans->head;

	char name[31] = { '\0' };
	char surname[31] = { '\0' };

	printf("Enter name: ");
	if (!inputString(name, nameCondition)) {
		safeFreeLL(&trans);
		system("cls");
		return NULL;
	}

	printf("Enter surname: ");
	if (!inputString(surname, nameCondition)) {
		safeFreeLL(&trans);
		system("cls");
		return NULL;
	}

	while (current
		&& !((strstr(current->reciever, name) && strstr(current->reciever, surname))
			|| (strstr(current->sender, name) && strstr(current->sender, surname)))) {
		prev = current->next;
		safeFree(&current);
		current = prev;

		trans->head = current;
	}

	while (current) {
		if (((strstr(current->reciever, name) && strstr(current->reciever, surname)) || (strstr(current->sender, name) && strstr(current->sender, surname)))) {
			prev = current;
			current = current->next;
		}
		else {
			prev->next = current->next;
			safeFree(&current);
			current = prev->next;
		}
	}

	trans->tail = prev;

	if (!trans->head) {
		safeFreeLL(&trans);
		system("cls");
		printf("No transactions found asociated with %s %s\n\n", name, surname);
		return NULL;
	}

	return trans;
}

LLTRANS* showTransactions(const ACCOUNT* const acc) {
	LLTRANS* trans = readTransByID(acc->ID);
	return (LLTRANS*)printTransactions((const LLTRANS* const)trans);
}

const LLTRANS* printTransactions(const LLTRANS* const trans) {
	if (!trans || !trans->head) {
		return NULL;
	}

	TRANS* current = trans->head;

	printf("\n");
	while (current) {
		printf("%d: %s >>> %0.2lf >>> %d: %s\n", current->senderID, current->sender, current->value, current->recieverID, current->reciever);
		current = current->next;
	}

	return trans;
}