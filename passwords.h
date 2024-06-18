#pragma once
#include "bank_account.h"
#include <stdio.h>

typedef struct password {
	int hash;
	int index;
	char password[31];
}PASSWORD;

char* encrypt(char* const pass);
char* decrypt(char* const pass);
void deletePassword(const ACCOUNT* const acc);
int isWrongPassword(const ACCOUNT* const acc, const char* const password);
int checkIfUniquePassword(PASSWORD* const newPass, FILE* const fp);
int registerPassword(char* const password, ACCOUNT* const acc);
int adjustDuplicateIndex(PASSWORD* const newPass);
int hashNameSurname(const ACCOUNT* const acc);
int changePassword(const ACCOUNT* const acc);
int passwordCondition(const char* const str);
int inputPassword(char* const password);
int enterPassword(char* const password);
int setPassword(ACCOUNT* const acc);
int sumStr(const char* const str);