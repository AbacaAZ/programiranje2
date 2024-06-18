#pragma once

typedef struct bankAccount {
	double balance;
	char name[31];
	char surname[31];
	int ID;
	int index;
}ACCOUNT;

ACCOUNT** findByFullName(const char* const name, const char* const surname, int* const count);
ACCOUNT* findByID(const int ID);
ACCOUNT* createAccount();
int setName(ACCOUNT* const acc);
int setSurname(ACCOUNT* const acc);
int nameCondition(const char* const str);
int surnameCondition(const char* const str);
int countAccounts();
int getLastID();
void registerAccount(const ACCOUNT* const acc);
void setID(ACCOUNT* const acc);
void printAccounts(const ACCOUNT* const* const accs, const int n);
void printAccount(const ACCOUNT* const acc);
void updateAccount(const ACCOUNT* const acc);
void deleteAccount(ACCOUNT* const acc);
void init();