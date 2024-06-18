#pragma once

#include "transactions.h"

enum returnIndicators {
	RETURN_SUCCESS,
	RETURN_ABORT,
	RETURN_FAILURE,
	RETURN_TRY_AGAIN,
	RETURN_END,
	RETURN_NOT_FOUND
};

static void copyPasswordsFile(const char* const newName);
static void copyAccountsFile(const char* const newName);
TRANS* mergeBySender(TRANS* const a, TRANS* const b);
TRANS* mergeByReciever(TRANS* const a, TRANS* const b);
char* inputString(char* const str, int (*condition)(const char* const));
void LLMergeSortSplit(TRANS* source, TRANS** front, TRANS** back);
void LLMergeSort(TRANS* headRef, TRANS* (*merge)(TRANS* const, TRANS* const));
void LLReverseList(LLTRANS* const list);
void safeFree(void** const p);
void safeFreeLL(LLTRANS** const ll);
void safeFreeArr(ACCOUNT*** const pArr, const int n);
void rewriteFiles();
int stringToDigit(const char* const str);
int confirmAction();
int inputNumber();