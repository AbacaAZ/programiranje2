#ifndef _CRT_SECURE_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "menus.h"
#include "transactions.h"
#include "util.h"

int main() {
	init();
	extern const ACCOUNT* currAcc;
	int option = 0;
	while (option != RETURN_END) {
		if (!currAcc) {
			option = loginMenu();
		}
		else {
			option = mainMenu();
		}
	}

	rewriteFiles();
}