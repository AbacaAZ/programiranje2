#pragma once

#include "bank_account.h"
#include "transactions.h"

ACCOUNT* signUp();
int mainMenu();
int loginMenu();
int transactionsMenu();
int accountSettingsMenu();
int login(const ACCOUNT* const acc);
void sortMenu(LLTRANS* const trans);
void transferBalance();
inline void printAccountSettingsMenu();
inline void printTransactionsMenu();
inline void transactionHistory();
inline void printLoginMenu();
inline void printMainMenu();
inline void printSortMenu();
inline void requestLoan();
inline void logOut();