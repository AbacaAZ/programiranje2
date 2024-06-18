#ifndef _CRT_SECURE_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include "bank_account.h"
#include "transactions.h"
#include "util.h"
#include "passwords.h"

char* inputString(char* const str, int (*condition)(const char* const)) {
	int check = 1;

	do {
		char temp[31] = { '\0' };
		for (int i = 0; i < 30;) {
			temp[i] = _getch();

			switch (temp[i]) {
			case 8:
				if (i > 0) {
					i--;
					printf("\b \b");
				}
				break;
			case '\n':
			case 13:
				str[i] = '\0';
				printf("\n");
				i = 30;
				break;
			default:
				printf("%c", temp[i]);
				str[i] = temp[i];
				i++;
			}
		}

		check = strcmp(str, "end");
	} while (check && !condition(str));

	if (!check) {
		return NULL;
	}

	return str;
}

int inputNumber() {
	int check = 0;
	char str[31] = { '\0' };

	do {
		for (int i = 0; i < 30;) {
			str[i] = _getch();

			switch (str[i]) {
			case 8:
				if (i > 0) {
					i--;
					printf("\b \b");
				}
				break;
			case '\n':
			case 13:
				str[i] = '\0';
				printf("\n");
				i = 30;
				break;
			default:
				printf("%c", str[i]);
				i++;
			}
		}

		if (!strcmp(str, "end")) {
			return -1;
		}

		for (int i = 0; str[i] != '\0'; i++) {
			check = isdigit(str[i]);
			if (!check) {
				printf("Please enter a number or \"end\" to cancel input: ");
				break;
			}
		}
	} while (!check);

	return stringToDigit(str);
}

int stringToDigit(const char* const str) {
	int num = 0;
	size_t len = strlen(str);
	for (int i = 0; i < len; i++) {
		num *= 10;
		num += str[i] - '0';
	}

	return num;
}

void LLMergeSort(TRANS* headRef, TRANS* (*merge)(TRANS* const, TRANS* const)) {
	if (!headRef) {
		return;
	}
	TRANS* head = headRef;
	TRANS* left = NULL;
	TRANS* right = NULL;

	if (!head || !head->next) {
		return;
	}

	LLMergeSortSplit(head, &left, &right);

	LLMergeSort(left, merge);
	LLMergeSort(right, merge);

	headRef = merge(left, right);
}

void LLMergeSortSplit(TRANS* source, TRANS** front, TRANS** back) {
	if (!source || !source->next) {
		return;
	}
	TRANS* fast = NULL;
	TRANS* slow = NULL;
	slow = source;
	fast = source->next;

	while (fast != NULL) {
		fast = fast->next;
		if (fast != NULL) {
			slow = slow->next;
			fast = fast->next;
		}
	}

	*front = source;
	*back = slow->next;
	slow->next = NULL;
}

TRANS* mergeBySender(TRANS* const a, TRANS* const b) {
	TRANS* newHead = NULL;

	if (!a) {
		return b;
	}
	else if (!b) {
		return a;
	}

	int check = 0;
	int check2 = 0;
	if (a->sender && b->sender) {
		check = strcmp(a->sender, b->sender);
	}


	if (check < 0) {
		newHead = a;
		newHead->next = mergeBySender(a->next, b);
	}
	else if(check > 0){
		newHead = b;
		newHead->next = mergeBySender(a, b->next);
	}
	else {
		check2 = a->senderID >= b->senderID;
		if (check2 > 0) {
			newHead = a;
			newHead->next = mergeBySender(a->next, b);
		}
		else if (check2 < 0) {
			newHead = b;
			newHead->next = mergeBySender(a, b->next);
		}
		else {
			if (a->value >= b->value) {
				newHead = a;
				newHead->next = mergeBySender(a->next, b);
			}
			else {
				newHead = b;
				newHead->next = mergeBySender(a, b->next);
			}
		}
	}

	return newHead;
}

TRANS* mergeByReciever(TRANS* const a, TRANS* const b) {
	TRANS* newHead = NULL;

	if (!a) {
		return b;
	}
	else if (!b) {
		return a;
	}

	int check = 0;
	if (a->reciever && b->reciever) {
		check = strcmp(a->reciever, b->reciever);
	}


	if (check < 0) {
		newHead = a;
		newHead->next = mergeByReciever(a->next, b);
	}
	else if (check > 0) {
		newHead = b;
		newHead->next = mergeByReciever(a, b->next);
	}
	else {
		if (a->recieverID >= b->recieverID) {
			newHead = a;
			newHead->next = mergeByReciever(a->next, b);
		}
		else {
			newHead = b;
			newHead->next = mergeByReciever(a, b->next);
		}
	}

	return newHead;
}

void LLReverseList(LLTRANS* const list) {
	if (!list || !list->head) {
		return;
	}

	TRANS* curr = NULL;
	TRANS* next = NULL;
	TRANS* prev = NULL;

	curr = list->head;
	list->tail = list->head;

	while (curr != NULL) {
		next = curr->next;
		curr->next = prev;
		prev = curr;
		curr = next;
	}

	list->head = prev;
}

static void copyAccountsFile(const char* const newName) {
	FILE* newFile = fopen(newName, "wb+x");
	if (!newFile) {
		return;
	}

	FILE* fp = fopen("accounts.bin", "rb+");
	if (!fp) {
		fclose(newFile);
		return;
	}

	ACCOUNT temp = { 0 };
	size_t read = fread(&temp, sizeof(ACCOUNT), 1, fp);

	while (read) {
		if (temp.ID != -1) {
			fwrite(&temp, sizeof(ACCOUNT), 1, newFile);
		}
		read = fread(&temp, sizeof(ACCOUNT), 1, fp);
	}

	fclose(fp);
	fclose(newFile);
}

static void copyPasswordsFile(const char* const newName) {
	FILE* newFile = fopen(newName, "wb+x");
	if (!newFile) {
		return;
	}

	FILE* fp = fopen("passwords.bin", "rb+");
	if (!fp) {
		fclose(newFile);
		return;
	}

	PASSWORD temp = { 0 };
	size_t read = fread(&temp, sizeof(PASSWORD), 1, fp);

	while (read) {
		if (temp.hash != -1) {
			fwrite(&temp, sizeof(PASSWORD), 1, newFile);
		}
		read = fread(&temp, sizeof(PASSWORD), 1, fp);
	}

	fclose(fp);
	fclose(newFile);
}

void rewriteFiles() {
	copyAccountsFile("temp.bin");

	if (rename("accounts.bin", "delete.bin") || rename("temp.bin", "accounts.bin")) {
		return;
	}

	remove("delete.bin");
	
	copyPasswordsFile("temp.bin");
	if (rename("passwords.bin", "delete.bin") || rename("temp.bin", "passwords.bin")) {
		return;
	}

	remove("delete.bin");
}

int confirmAction() {
	printf("\nAre you sure? (Y/N): ");
	char choice = 0;
	do {
		choice = _getch();
	} while (choice != 'Y' && choice != 'N' && choice != 'y' && choice != 'n');

	return choice == 'N' || choice == 'n' ? 1 : 0;
}

void safeFree(void** const p) {
	if (*p) {
		free(*p);
	}
	*p = NULL;
}

void safeFreeLL(LLTRANS** const ll) {
	if (!ll || !(*ll)) {
		return;
	}
	TRANS* current = NULL;
	TRANS* next = NULL;
	if ((*ll)->head) {
		current = (*ll)->head;
		while (current) {
			next = current->next;
			safeFree(&current);
			current = next;
		}
	}

	safeFree(ll);
}

void safeFreeArr(ACCOUNT*** const pArr, const int n) {
	if (*pArr != NULL) {
		for (int i = 0; i < n; i++) {
			if (pArr[0][i]) {
				safeFree(&pArr[0][i]);
			}
		}
		safeFree((void** const)pArr);
	}
}