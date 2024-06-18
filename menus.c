#ifndef _CRT_SECURE_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "menus.h"
#include "util.h"
#include "bank_account.h"
#include "passwords.h"
#include "transactions.h"

extern const ACCOUNT* currAcc = NULL;
extern const LLTRANS* currTrans = NULL;
extern const ACCOUNT ADMIN;

int mainMenu() {
	char choice = 0;

	enum choices {
		END_PROGRAM = '0',
		TRANSACTION_HISTORY = '1',
		REQUEST_LOAN = '2',
		TRANSFER_BALANCE = '3',
		ACCOUNT_SETTINGS = '4',
		LOG_OUT = '5'
	};

	do {
		printMainMenu();

		do {
			choice = _getch();
		} while (choice < '0' && choice > '5');

		switch (choice) {
		case TRANSACTION_HISTORY:
			system("cls");
			printTransactionsMenu();
			transactionsMenu();
			break;
		case REQUEST_LOAN:
			requestLoan();
			break;
		case TRANSFER_BALANCE:
			transferBalance();
			break;
		case ACCOUNT_SETTINGS:
			system("cls");
			choice = accountSettingsMenu();
			switch (choice) {
			case RETURN_ABORT:
				return RETURN_ABORT;
				break;
			}
			break;
		case LOG_OUT:
			if (!confirmAction()) {
				system("cls");
				updateAccount(currAcc);
				logOut();
				return RETURN_ABORT;
			}
			system("cls");
			break;
		case END_PROGRAM:
			updateAccount(currAcc);
			return RETURN_END;
			break;
		default:
			system("cls");
		}
	} while (choice);

	return RETURN_SUCCESS;
}

int loginMenu() {
	volatile char choice = 0;

	enum choices {
		END_PROGRAM = '0',
		SIGN_UP = '1',
		LOG_IN = '2'
	};

	while (currAcc == NULL) {
		printLoginMenu();

		do {
			choice = _getch();
		} while (choice < '0' && choice > '2');

		switch (choice) {
		case SIGN_UP:
			signUp();
			return RETURN_SUCCESS;
			break;
		case LOG_IN:
			login(NULL);
			return RETURN_SUCCESS;
			break;
		case END_PROGRAM:
			return RETURN_END;
			break;
		default:
			system("cls");
		}
	};

	return RETURN_ABORT;
}

int transactionsMenu() {
	char choice = -1;
	enum choices {
		GO_BACK = '0',
		SEARCH_BY_ID = '1',
		SEARCH_BY_NAME = '2',
		SORT = '3',
		SHOW_ALL = '4'
	};
	currTrans = readTransByID(currAcc->ID);
	printTransactions(currTrans);
	static LLTRANS* current = NULL;

	do {

		do {
			choice = _getch();
		} while (choice < '0' && choice > '4');

		switch (choice) {
		case SEARCH_BY_ID:
			system("cls");
			safeFreeLL(&current);
			printTransactionsMenu();
			printf("Enter ID: ");
			current = (LLTRANS*)printTransactions(findTransByID(readTransByID((const int)currAcc->ID)));
			if (!current) {
				system("cls");
				printTransactionsMenu();
				printf("No transactions found\n");
			}
			break;
		case SEARCH_BY_NAME:
			system("cls");
			safeFreeLL(&current);
			printTransactionsMenu();
			current = (LLTRANS*)printTransactions(findTransByName(readTransByID((const int)currAcc->ID)));
			if (!current) {
				system("cls");
				printTransactionsMenu();
				printf("No transactions found\n");
			}
			break;
		case SORT:
			system("cls");
			if (current) {
				sortMenu(current);
			}
			else {
				sortMenu((LLTRANS* const)currTrans);
				safeFreeLL(&current);
			}

			system("cls");
			printTransactionsMenu();
			printTransactions(current ? current : currTrans);
			break;
		case SHOW_ALL:
			system("cls");
			if (current) {
				safeFreeLL(&current);
			}
			printTransactionsMenu();
			printTransactions(currTrans);
			break;
		case GO_BACK:
			system("cls");
			safeFreeLL((LLTRANS** const)(&currTrans));
			if (current) {
				safeFreeLL(&current);
			}
			return RETURN_END;
		default:
			system("cls");
			printTransactionsMenu();
		}
	} while (choice);

	return RETURN_END;
}

int accountSettingsMenu() {
	char choice = 0;
	int temp = 0;

	enum choices {
		GO_BACK = '0',
		CHANGE_PASSWORD = '1',
		DELETE_ACCOUNT = '2'
	};

	do {
		printAccountSettingsMenu();

		do {
			choice = _getch();
		} while (choice < '0' && choice > '2');

		switch (choice) {
		case CHANGE_PASSWORD:
			while (1) {
				temp = changePassword(currAcc);
				switch (temp) {
				case RETURN_TRY_AGAIN:
					temp = changePassword(currAcc);
					break;
				case RETURN_ABORT:
					system("cls");
					printf("Password change canceled\n\n");
					return RETURN_ABORT;
					break;
				case RETURN_FAILURE:
					system("cls");
					printf("The passwords don't match\n\n");
					return RETURN_FAILURE;
					break;
				case RETURN_SUCCESS:
					system("cls");
					printf("Password succesfully changed\n\n");
					return RETURN_SUCCESS;
					break;
				}
			}
			break;

		case DELETE_ACCOUNT:
			if (confirmAction()) {
				system("cls");
				return RETURN_END;
			}
			deleteAccount((ACCOUNT* const)currAcc);
			logOut();
			system("cls");
			return RETURN_ABORT;
			break;

		case GO_BACK:
			system("cls");
			return RETURN_END;
			break;

		default:
			system("cls");
		}
	} while (choice);

	return RETURN_END;
}

void sortMenu(LLTRANS* const trans) {
	char choice = -1;
	enum choices {
		GO_BACK = '0',
		SORT_BY_SENDER = '1',
		SORT_BY_RECIEVER = '2',
		TOGGLE_ORDER = '3'
	};

	printSortMenu();

	do {
		do {
			choice = _getch();
		} while (choice < '0' && choice > '3');

		switch (choice) {
		case SORT_BY_SENDER:
			if (!trans) {
				return;
			}
			LLMergeSort(trans->head, mergeBySender);
			return;
			break;
		case SORT_BY_RECIEVER:
			if (!trans) {
				return;
			}
			LLMergeSort(trans->head, mergeByReciever);
			return;
			break;
		case TOGGLE_ORDER:
			if (!trans) {
				return;
			}
			LLReverseList(trans);
			return;
			break;
		case GO_BACK:
			system("cls");
			printTransactionsMenu();
			return;
		default:
			system("cls");
			printSortMenu();
		}
	} while (choice);

	return;
}



int login(const ACCOUNT* const acc) {
	if (acc != NULL) {
		currAcc = acc;
		return RETURN_SUCCESS;
	}

	char name[31] = { '\0' };
	char surname[31] = { '\0' };
	char password[31] = { '\0' };

	system("cls");
	printf("Enter \"end\" to cancel input\n\n");

	printf("Enter name: ");
	if (inputString(name, nameCondition)) {
		printf("Enter surname: ");
		if (!inputString(surname, surnameCondition)) {
			system("cls");
			return RETURN_ABORT;
		}
	}
	else {
		system("cls");
		return RETURN_ABORT;
	}


	int matches = 0;
	ACCOUNT** accs = findByFullName(name, surname, &matches);
	if (!accs) {
		return RETURN_NOT_FOUND;
	}

	int match = 0;
	int id = -1;
	const int LOGIN_ATTEMPTS = 3;

	for (int i = 0; i < LOGIN_ATTEMPTS; i++) {
		printf("Enter passsword: ");
		switch (enterPassword(password)) {
		case RETURN_ABORT:
			system("cls");
			return RETURN_ABORT;
		case RETURN_TRY_AGAIN:
			if (i < LOGIN_ATTEMPTS - 1) {
				printf("Attempts left: %d\n\n", LOGIN_ATTEMPTS - 1 - i);
				continue;
			}
			else {
				system("cls");
				return RETURN_FAILURE;
			}
			break;
		}

		for (int j = 0; j < matches; j++) {
			match = !isWrongPassword(accs[j], password);
			if (match) {
				id = accs[j]->ID;
				safeFreeArr(&accs, matches);
				break;
			}
		}

		if (match) {
			currAcc = findByID(id);
			system("cls");
			return RETURN_SUCCESS;
			break;
		}
		else if (i < LOGIN_ATTEMPTS - 1) {
			printf("\nPassword and name don't match!\nAttempts left: %d\n\n", LOGIN_ATTEMPTS - 1 - i);
		}

	}

	if (!currAcc) {
		system("cls");
		return RETURN_FAILURE;
	}

	system("cls");
	return RETURN_END;
}

ACCOUNT* signUp() {
	ACCOUNT* acc = NULL;
	acc = createAccount();

	if (acc) {
		registerAccount(acc);

		if (login(acc)) {
			system("cls");
			return NULL;
		}
	}
	return acc;
}

void transferBalance() {
	ACCOUNT* reciever = NULL;
	int amount = 0;

	printf("Enter ID: ");
	const int ID = inputNumber();
	if (ID == -1) {
		system("cls");
		printf("Transfer canceled\n\n");
		return;
	}
	reciever = findByID(ID);
	if (!reciever) {
		return;
	}
	printf("Enter amount to transfer: ");
	amount = inputNumber();
	if (confirmAction()) {
		system("cls");
		printf("Transfer canceled\n\n");
		safeFree(&reciever);
		return;
	}

	registerTransaction(createTrans((ACCOUNT* const)currAcc, (ACCOUNT* const)reciever, (const double)amount));
	safeFree(&reciever);
}

inline void requestLoan(){
	system("cls");
	printf("Enter \"end\" to cancel input\n\nHow much do you want to loan?: ");
	registerTransaction(createTrans((ACCOUNT* const)(&ADMIN), (ACCOUNT* const)currAcc, inputNumber()));
}

inline void printMainMenu() {
	printf("Logged in as:\n");
	printAccount(currAcc);
	printf("Please input a number coresponding to an option (0 - 5)\n");
	printf("1 - Transaction history\n2 - Request a loan\n3 - Transfer balance\n4 - Account settings\n5 - Log out\n0 - Exit program\n\n");
	updateAccount(currAcc);
}

inline void printLoginMenu() {
	printf("Please input a number coresponding to an option (0 - 2)\n");
	printf("1 - Sign up\n2 - Log in\n0 - Exit program\n\n");
}

inline void printAccountSettingsMenu() {
	printf("Logged in as:\n");
	printAccount(currAcc);
	printf("Please input a number coresponding to an option (0 - 2)\n");
	printf("1 - Change password\n2 - Delete account\n0 - Go back\n\n");
}

inline void printSortMenu() {
	printf("Please input a number coresponding to an option (0 - 4)\n");
	printf("1 - By sender\n2 - By Reciever\n3 - Ascending/Descending\n0 - Go back\n\n");
}


inline void printTransactionsMenu() {
	printf("Please input a number coresponding to an option (0 - 4)\n");
	printf("1 - Search by ID\n2 - Search by name\n3 - Sort\n4 - Show all\n0 - Go back\n\n");
}

inline void transactionHistory() {
	system("cls");
	currTrans = showTransactions(currAcc);
}

inline void logOut() {
	safeFree((void** const)(&currAcc));
}