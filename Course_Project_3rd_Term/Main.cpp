#include <iostream>
#include <windows.h>
#include "Prot.h"


using namespace std;
using namespace telecom;

int main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	
    Database db;

    try {
        loadAll(db);

        int choice;
        do {

            system("cls");

            printLine('=', 50);
            cout << " УЧЁТ ТАРИФОВ И КЛИЕНТОВ ОПЕРАТОРА СВЯЗИ" << endl;
            printLine('=', 50);

            cout << " 1 - Вход в систему" << endl
                << " 2 - Регистрация" << endl
                << " 3 - Выйти из программы" << endl;

            printLine('=', 50);
            choice = readNumber<int>("Ваш выбор:", 1, 3);

            if (choice == 1) {

                db.currentUser = loginUser(db);

                if (db.currentUser != nullptr)
                    mainMenu(db);
                db.currentUser = nullptr;
            }
            else if (choice == 2) {
                runAction(registerUser, db);
            }

        } while (choice != 3);
    }

    catch (const inputException& e) {
        cout << "\n" << e.what() << endl;
    }

    catch (const exception& e) {
        cout << "Произошла ошибка: " << e.what() << endl;
        return 1;
    }


	return 0;
}