#ifndef _CRT_SECURE_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "bank_account.h"
#include "util.h"
#include "passwords.h"

static int g_accounts = 0;

extern ACCOUNT ADMIN = {
	DBL_MAX,
	"",
	"",
	0,
	0
};

void init() {
	FILE* fp = NULL;
	fp = fopen("passwords.bin", "wbx");
	if (fp) {
		fclose(fp);
		registerPassword("\0", &ADMIN);
	}

	fp = fopen("accounts.bin", "wbx");
	if (fp) {
		fclose(fp);
		registerAccount(&ADMIN);
	}

	fp = fopen("transactions.bin", "wbx");
	if (fp) {
		fclose(fp);
	}

	g_accounts = countAccounts();
}

ACCOUNT** findByFullName(const char* const name, const char* const surname, int* const matches) {
	ACCOUNT** temp = NULL;
	temp = (ACCOUNT**)calloc((size_t)g_accounts + 1, sizeof(ACCOUNT*));
	if (temp == NULL) {
		return NULL;
	}
	for (int j = 0; j < g_accounts + 1; j++) {
		temp[j] = (ACCOUNT*)calloc(1, sizeof(ACCOUNT));
		if (!temp[j]) {
			safeFreeArr(&temp, j + 1);
			return NULL;
		}
	}

	FILE* fp = NULL;
	fp = fopen("accounts.bin", "rb+");
	if (!fp) {
		perror("Failed to open file");
		exit(EXIT_FAILURE);
	}

	int i = 0;
	size_t read = 1;
	while (1) {
		read = fread(temp[i], sizeof(ACCOUNT), 1, fp);
		if (!read) {
			break;
		}
		else if (!strcmp(temp[i]->name, name) && !strcmp(temp[i]->surname, surname)) {
			i++;
		}
	}

	*matches = i;
	if (*matches == 0) {
		fclose(fp);
		safeFreeArr(&temp, g_accounts + 1);
		system("cls");
		printf("No accounts with that name and surname found\n\n");
		return NULL;
	}

	ACCOUNT** accs = NULL;
	accs = (ACCOUNT**)calloc(*matches, sizeof(ACCOUNT*));
	for (int j = 0; j < *matches; j++) {
		accs[j] = (ACCOUNT*)calloc(1, sizeof(ACCOUNT));
		if (!accs[j]) {
			safeFreeArr(&accs, j + 1);
			break;
		}
	}

	for (int j = 0; j < *matches; j++) {
		strcpy(accs[j]->name, temp[j]->name);
		strcpy(accs[j]->surname, temp[j]->surname);
		accs[j]->ID = temp[j]->ID;
		accs[j]->index = temp[j]->index;
	}

	safeFreeArr(&temp, g_accounts + 1);
	fclose(fp);

	return accs;
}

ACCOUNT* findByID(const int ID) {
	system("cls");
	ACCOUNT* acc = NULL;
	acc = (ACCOUNT*)calloc(1, sizeof(ACCOUNT));
	if (!acc) {
		perror("Failed to allocate memory for account");
		return NULL;
	}
	
	FILE* fp = NULL;
	fp = fopen("accounts.bin", "rb+");
	if (!fp) {
		perror("Failed to open file");
		exit(EXIT_FAILURE);
	}

	size_t read = 0;
	while (acc) {
		read = fread(acc, sizeof(ACCOUNT), 1, fp);
		if (acc->ID == ID) {
			break;
		}
		if (!read) {
			break;
		}
	}

	fclose(fp);

	if (!read) {
		printf("Account with that ID does NOT exist\n\n");
		return NULL;
	}
	return acc;
}

ACCOUNT* createAccount(){
	system("cls");
	ACCOUNT* acc = NULL;
	acc = (ACCOUNT*)calloc(1, sizeof(ACCOUNT));
	if (!acc) {
		perror("Failed to allocate memory for account");
		return NULL;
	}

	printf("Enter \"end\" to cancel input\n\n");

	if (setName(acc) || setSurname(acc)) {
		system("cls");
		return NULL;
	}

	setID(acc);
	int check = setPassword(acc);
	while (check == RETURN_TRY_AGAIN || check == RETURN_FAILURE) {
		check = setPassword(acc);
	}
	
	switch (check) {
	case RETURN_ABORT:
		safeFree(&acc);
		system("cls");
		return NULL;
		break;
	}

	acc->balance = 0;

	g_accounts++;
	system("cls");
	return acc;
}

void registerAccount(const ACCOUNT* const acc) {
	if (!acc) {
		return;
	}

	FILE* fp = NULL;
	fp = fopen("accounts.bin", "rb+");
	if (!fp) {
		perror("Failed to open file");
		exit(EXIT_FAILURE);
	}

	ACCOUNT temp = { 0 };
	size_t nmRead = 0;

	while (1) {
		nmRead = fread(&temp, sizeof(ACCOUNT), 1, fp);
		if (temp.ID == -1) {
			fseek(fp, (-1) * sizeof(ACCOUNT), SEEK_CUR);
			break;
		}
		if (!nmRead) {
			break;
		}
	}

	fwrite(acc, sizeof(ACCOUNT), 1, fp);

	fclose(fp);
}

void updateAccount(const ACCOUNT* const acc) {
	if (!acc) {
		return;
	}

	FILE* fp = NULL;
	fp = fopen("accounts.bin", "rb+");
	if (!fp) {
		perror("Failed to open file");
		exit(EXIT_FAILURE);
	}

	ACCOUNT temp = { 0 };

	while (1) {
		if (!fread(&temp, sizeof(ACCOUNT), 1, fp)) {
			break;
		}
		if (temp.ID == acc->ID) {
			fseek(fp, (-1) * sizeof(ACCOUNT), SEEK_CUR);
			fwrite(acc, sizeof(ACCOUNT), 1, fp);
			break;
		}
	}
	fclose(fp);
}

void deleteAccount(ACCOUNT* const acc) {
	if (!acc) {
		return;
	}

	FILE* fp = NULL;
	fp = fopen("accounts.bin", "rb+");
	if (!fp) {
		perror("Failed to open file");
		exit(EXIT_FAILURE);
	}

	deletePassword(acc);

	ACCOUNT empty = { 0, "EMPTY", "EMPTY", -1, 0};
	ACCOUNT temp = { 0 };
	size_t nmRead = 0;

	while (1) {
		nmRead = fread(&temp, sizeof(ACCOUNT), 1, fp);
		if (!nmRead) {
			break;
		}
		else if (temp.ID == acc->ID) {
			fseek(fp, (-1) * sizeof(ACCOUNT), SEEK_CUR);
			fwrite(&empty, sizeof(ACCOUNT), 1, fp);
			break;
		}
	}

	g_accounts--;
	fclose(fp);
}

int getLastID() {
	FILE* fp = NULL;
	fp = fopen("accounts.bin", "rb+");
	if (!fp) {
		perror("Failed to open file");
		exit(EXIT_FAILURE);
	}

	int id = 0;
	ACCOUNT temp = { 0 };
	size_t read = 0;
	
	do {
		read = fread(&temp, sizeof(ACCOUNT), 1, fp);
		if (read && temp.ID > id) {
			id = temp.ID;
		}
	} while (read);

	fclose(fp);
	return id;
}

int countAccounts() {
	FILE* fp = NULL;
	fp = fopen("accounts.bin", "rb+");
	if (!fp) {
		perror("Failed to open file");
		exit(EXIT_FAILURE);
	}

	int count = 0;
	ACCOUNT acc = { 0 };

	size_t read = 0;
	do {
		read = fread(&acc, sizeof(ACCOUNT), 1, fp);
		if ((acc.ID > 0)) {
			count++;
		}
	}while (read);

	fclose(fp);
	return count;
}

int setName(ACCOUNT* const acc) {
	char name[31] = { '\0' };
	printf("Enter name: ");
	if (!inputString(name, nameCondition)) {
		return 1;
	}

	strcpy(acc->name, name);
	return 0;
}

int setSurname(ACCOUNT* const acc) {
	char surname[31] = { '\0' };
	printf("Enter surname: ");
	if (!inputString(surname, surnameCondition)) {
		return 1;
	}

	strcpy(acc->surname, surname);
	return 0;
}

void setID(ACCOUNT* const acc) {
	static int lastID = 0;

	lastID = getLastID();

	acc->ID = ++lastID;
}

int nameCondition(const char* const str) {
	int alpha = 0;

	for (int i = 0; str[i] != '\0'; i++) {
		alpha = isalpha(str[i]);
		if (!alpha) {
			printf("\nName can contain only alphabetic characters.\n\nRe-enter name: ");
			break;
		}
	}
	
	return alpha;
}

int surnameCondition(const char* const str) {
	int alpha = 0;

	for (int i = 0; str[i] != '\0'; i++) {
		alpha = isalpha(str[i]);
		if (!alpha) {
			printf("\nSurname can contain only alphabetic characters.\n\nRe-enter surname: ");
			break;
		}
	}

	return alpha;
}

void printAccounts(const ACCOUNT* const* const accs, const int n) {
	if (accs) {
		for (int i = 0; i < n; i++) {
			if (accs[i]) {
				printf("Name:\t %s\nSurname: %s\nBalance: %0.2lf\nID:\t %d\n\n", accs[i]->name, accs[i]->surname, accs[i]->balance, accs[i]->ID);
			}
		}
	}
}

void printAccount(const ACCOUNT* const acc) {
	if (acc) {
		printf("Name:\t %s\nSurname: %s\nBalance: %0.2lf\nID:\t %d\n\n", acc->name, acc->surname, acc->balance, acc->ID);
	}
}