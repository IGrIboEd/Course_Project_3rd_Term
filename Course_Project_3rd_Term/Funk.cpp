#define NOMINMAX
#include <windows.h>
#define _CRT_SECURE_NO_WARNINGS
#include <ctime>
#include <cctype>
#include "Prot.h"


//надо бы потом мб ещё разбить это всё на файлы по отдельности


using namespace std;


namespace telecom {

    int Entity::aliveCount = 0;

    //здесь будут доп функции, просто чтоб жизнь была проще -----------------------------------

    //я хочу вкусно и точка
    string inputLine() {
        string s;
        if (!getline(cin, s))
            throw inputException("Ввод данных прерван (конец потока ввода)");
        return s;
    }

    void waitEnter() {
        cout << "\nНажмите Enter для продолжения";
        inputLine();
    }

    void printLine(char symbol, int length) {
        cout << string(length, symbol) << endl;
    }

    string money(double value) {
        ostringstream ss;
        ss << fixed << setprecision(2) << value;
        return ss.str();
    }

    //нужен для того, чтобы при вводе допустим после того же логина удаляло пробелы после него, чтобы оно правильно прочитало
    string trim(const string& s) {
        string result = s;

        while (!result.empty() && result.front() == ' ')
            result.erase(0, 1);

        while (!result.empty() && result.back() == ' ')
            result.pop_back();

        return result;
    }


    string inputText(const string& prompt) {
        while (true) {
            cout << prompt << ": ";
            string s = trim(inputLine());
            if (s.empty())
                cout << "Поле не может быть пустым" << endl;
            else if (s.find(';') != string::npos)
                cout << "Символ ';' нельзя использовать" << endl;
            else if (s.size() > 25)
                cout << "Не более 25 символов" << endl;
            else
                return s;
        }
    }


    //функции для файлов -----------------------------------


    string readField(istream& in) {
        string field;

        if (!getline(in, field, ';'))
            throw myException("Недостаточно полей в записи");
        return field;

    }

    int toInt(const string& s) {
        istringstream ss(s);
        int value;
        char extra;

        if (!(ss >> value) || (ss >> extra))
            throw myException("Неверное целое число:'" + s + "'");
        return value;

    }


    double toDouble(const string& s) {
        istringstream ss(s);
        double value;
        char extra;

        if (!(ss >> value) || (ss >> extra))
            throw myException("Неверное число:'" + s + "'");
        return value;

    }

    //функции для дат -----------------------------------


    Date::Date() : day(1), month(1), year(2000) {}
    Date::Date(int d, int m, int y) : day(d), month(m), year(y) {}

	bool Date::isValid(int day, int month, int year) {
		if (year < 1950 || year > 2100 || month < 1 || month > 12)
			return false;

		int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
		bool leap = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
        int maxDay = daysInMonth[month - 1];

        if (month == 2 && leap)
            maxDay = 29;

        return day >= 1 && day <= maxDay;
	}

    string Date::toString() const {
        ostringstream ss;
        ss << setfill('0') << setw(2) << day << '.' << setw(2) << month << '.' << setw(4) << year;
        return ss.str();
    }

    bool Date::operator<(const Date& other) const {
        if (year != other.year)
            return year < other.year;
        if (month != other.month)
            return month < other.month;
        return day < other.day;
    }

    ostream& operator<<(ostream& out, const Date& date) {
        return out << date.toString();
    }

    istream& operator>>(istream& in, Date& date) {
        int d, m, y;
        char dot1, dot2;
        if (!(in >> d >> dot1 >> m >> dot2 >> y) || dot1 != '.' || dot2 != '.' || !Date::isValid(d, m, y)) {
            in.setstate(ios::failbit);
            return in;
        }
        date = Date(d, m, y);
        return in;
    }

    //переписываю
    Date Date::today() {
        time_t now = time(nullptr);
        tm* local = localtime(&now);
        return Date(local->tm_mday, local->tm_mon + 1, local->tm_year + 1900);
    }

    int Date::yearsBetween(const Date& from, const Date& to) {
        int years = to.year - from.year;
        if (to.month < from.month || (to.month == from.month && to.day < from.day))
            years--;
        return years < 0 ? 0 : years;
    }


    Date readDate(const string& prompt) {
        while (true) {
            cout << prompt << " (дд.мм.гггг): ";
            istringstream ss(inputLine());
            Date date;
            char extra;
            if (!(ss >> date) || (ss >> extra))
                cout << "Неверная дата" << endl;
            else if (Date::today() < date)
                cout << "Дата не может быть в будущем" << endl;
            else
                return date;
        }
    }

    //функции для тарифов -----------------------------------


    void Tariff::write(ostream& out) const {
        out << setprecision(10) << getType() << ';' << id << ';' << name << ';' << fee << ';'
            << minutes << ';' << sms << ';' << gb << ';' << priceMinute << ';' << priceSms << ';' << priceGb;
    }

    void Tariff::read(istream& in) {

        id = toInt(readField(in));
        name = readField(in);

        fee = toDouble(readField(in));
        minutes = toInt(readField(in));
        sms = toInt(readField(in));
        gb = toInt(readField(in));

        priceMinute = toDouble(readField(in));
        priceSms = toDouble(readField(in));
        priceGb = toDouble(readField(in));

    }

    double PackageTariff::calculateCost(int usedMinutes, int usedSms, double usedGb) const {
        return fee + over(usedMinutes, minutes) * priceMinute
            + over(usedSms, sms) * priceSms
            + over(usedGb, gb) * priceGb;
    }

    double UnlimitedTariff::calculateCost(int usedMinutes, int usedSms, double /*usedGb*/) const {
        // интернет безлимитный, поэтому гигабайты не учитываются
        return fee + over(usedMinutes, minutes) * priceMinute
            + over(usedSms, sms) * priceSms;
    }

    //Создание тарифа нужного типа
    shared_ptr<Tariff> createTariff(int type) {
        if (type == 1)
            return make_shared<PackageTariff>();
        
        if (type == 2)
            return make_shared<UnlimitedTariff>();

        throw myException("Неизвестный тип тарифа:" + to_string(type));
    }

    shared_ptr<Tariff> readTariff(istream& in) {
        int type = toInt(readField(in));
        shared_ptr<Tariff> tariff = createTariff(type);
        tariff->read(in);
        return tariff;
    }


    //функции для клиентов -----------------------------------

    Client& Client::operator+=(double amount) {
        if (amount <= 0)
            throw myException("Сумма пополнения должна быть больше нуля");
        balance += amount;
        return *this;
    }

    bool Client::charge(double amount) {
        if (balance < amount)
            return false;
        balance -= amount;
        return true;
    }

    int Client::getDiscount() const {
        int years = Date::yearsBetween(connectDate, Date::today());
        if (years >= 5)
            return 10;
        if (years >= 2)
            return 5;
        return 0;
    }

    //проверка на то что номер правильный, т.е. должно начинаться с +375 и 9 цифр потом ещё должно быть
    bool Client::isValidPhone(const string& value) {
        if (value.size() != 13 || value.substr(0, 4) != "+375")
            return false;
        for (int i = 4; i < 13; i++)
            if (value[i] < '0' || value[i] > '9')
                return false;
        return true;
    }

    //для записи строки в поток
    void Client::write(ostream& out) const {
        out << setprecision(10) << id << ';' << fullName << ';' << phone << ';' << tariffId << ';'<< balance
        << ';' << connectDate << ';' << usedMinutes << ';' << usedSms << ';' << usedGb << ';' << (blocked ? 1 : 0);
    }

    void Client::read(istream& in) {

        id = toInt(readField(in));
        fullName = readField(in);
        phone = readField(in);

        tariffId = toInt(readField(in));
        balance = toDouble(readField(in));

        istringstream dateText(readField(in));

        if (!(dateText >> connectDate))
            throw myException("Неверная дата в файле");

        usedMinutes = toInt(readField(in));
        usedSms = toInt(readField(in));
        usedGb = toDouble(readField(in));
        blocked = toInt(readField(in)) == 1;

    }

    shared_ptr<Client> readClient(istream& in) {
        shared_ptr<Client> client = make_shared<Client>();
        client->read(in);
        return client;
    }
    
  
    //функции менюшек -----------------------------------

    void mainMenu(Database& db) {
        bool isAdmin = db.currentUser->getRole() == 1;

        int choice;
        do {
            system("cls");
            cout << "---- Главное меню (" << db.currentUser->getLogin() << ") ----\n"
                << "1 - Тарифы\n"
                << "2 - Клиенты\n"
                << "3 - Выставить счёт клиенту\n"
                << "4 - Отчёт по тарифам\n"
                << "5 - Сменить свой пароль\n";
            if (isAdmin)
                cout << "6 - Пользователи системы\n"
                << "7 - Журнал действий\n";
            cout << "0 - Выйти из аккаунта" << endl;
            choice = readNumber<int>("Ваш выбор", 0, isAdmin ? 7 : 5);
            switch (choice) {
            case 1:
                tariffsMenu(db);
                break;
            case 2:
                clientsMenu(db);
                break;
            case 3:
                //пусто
                break;
            case 4:
                //пусто
                break;
            case 5:
                //пусто
                break;
            case 6:
                //пусто
                break;
            case 7:
                //пусто
                break;
            }
        } while (choice != 0);

    }

    void usersMenu(Database& db) {
        int choice;
        do {
            system("cls");
            cout << "---- Пользователи ----\n"
                << "1 - Показать всех\n"
                << "2 - Добавить\n"
                << "3 - Удалить\n"
                << "0 - Назад" << endl;
            choice = readNumber<int>("Ваш выбор", 0, 3);
            switch (choice) {
            case 1:
                //пусто
                break;
            case 2:
                //пусто
                break;
            case 3:
                //пусто
                break;
            }
        } while (choice != 0);
    }

    void tariffsMenu(Database& db) {
        bool isAdmin = db.currentUser->getRole() == 1;
        int choice;
        do {
            system("cls");
            cout << "---- Тарифы ----\n"
                << "1 - Показать все\n"
                << "2 - Поиск по названию\n"
                << "3 - Фильтрация\n"
                << "4 - Сортировка\n";
            if (isAdmin)
                cout << "5 - Добавить\n"
                << "6 - Редактировать\n"
                << "7 - Удалить\n";
            cout << "0 - Назад" << endl;
            choice = readNumber<int>("Ваш выбор", 0, isAdmin ? 7 : 4);
            switch (choice) {
            case 1:
                //пусто
                break;
            case 2:
                //пусто
                break;
            case 3:
                //пусто
                break;
            case 4:
                //пусто
                break;
            case 5:
                //пусто
                break;
            case 6:
                //пусто
                break;
            case 7:
                //пусто
                break;
            }
        } while (choice != 0);
    }

    void clientsMenu(Database& db) {
        bool isAdmin = db.currentUser->getRole() == 1;
        int choice;
        do {
            system("cls");
            cout << "---- Клиенты ----\n"
                << "1 - Показать всех\n"
                << "2 - Поиск (ФИО или телефон)\n"
                << "3 - Фильтрация\n"
                << "4 - Сортировка\n"
                << "5 - Добавить\n"
                << "6 - Редактировать\n"
                << "7 - Пополнить баланс\n"
                << "8 - Записать расход за месяц\n";
            if (isAdmin)
                cout << "9 - Удалить\n";
            cout << "0 - Назад" << endl;
            choice = readNumber<int>("Ваш выбор", 0, isAdmin ? 9 : 8);
            switch (choice) {
            case 1:
                //пусто
                break;
            case 2:
                //пусто
                break;
            case 3:
                //пусто
                break;
            case 4:
                //пусто
                break;
            case 5:
                //пусто
                break;
            case 6:
                //пусто
                break;
            case 7:
                //пусто
                break;
            case 8:
                //пусто
                break;
            case 9:
                //пусто
                break;
            }
        } while (choice != 0);
    }


    //функции для аккаунтов -----------------------------------


    //хэширование по типу djb2, ну не гоже же паролю просто так лежать в файле
    unsigned long long Account::hashPassword(const string& password) {
        const string data = password;
        unsigned long long hash = 6767;
        for (int round = 0; round < 1000; round++)
            for (unsigned char c : data)
                hash = ((hash << 5) + hash) ^ c;
        return hash;
    }

    bool Account::isValidLogin(const string& value) {

        if (value.size() < 3 || value.size() > 20)
            return false;

        for (char c : value) {

            bool isLetter = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
            bool isDigit = c >= '0' && c <= '9';

            if (!isLetter && !isDigit && c != '_')
                return false;
        }

        return true;

    }

    bool Account::isValidPassword(const string& value) {
        return value.size() >= 4 && value.size() <= 30 && value.find(';') == string::npos;
    }



    void Account::write(ostream& out) const {
        out << id << ';' << login << ';' << hash << ';' << role;
    }

    void Account::read(istream& in) {
        id = toInt(readField(in));
        login = readField(in);
        istringstream hashText(readField(in));
        if (!(hashText >> hash))
            throw myException("Неверный хеш пароля в файле");
        role = toInt(readField(in));
    }

    shared_ptr<Account> readAccount(istream& in) {
        shared_ptr<Account> account = make_shared<Account>();
        account->read(in);
        return account;
    }

    //функции для файлов --------------------------------------


    void saveTariffs(const Database& db) {
        saveToFile(db.tariffs, file_tarrifs);
    }

    void saveClients(const Database& db) {
        saveToFile(db.clients, file_clients);
    }

    void saveAccounts(const Database& db) {
        saveToFile(db.accounts, file_users);
    }

    void loadAll(Database& db) {
        loadFromFile(db.tariffs, file_tarrifs, readTariff);
        loadFromFile(db.clients, file_clients , readClient);
        loadFromFile(db.accounts, file_users, readAccount);

        if (db.accounts.size() == 0) {
            shared_ptr<Account> admin = make_shared<Account>("admin", Account::hashPassword("admin"), 1);
            admin->setId(1);
            db.accounts.push_back(admin);

            saveAccounts(db);
        }
    }


    //регистрация

    shared_ptr<Account> loginUser(Database& db) {
        system("cls");
        cout << "---- Вход в систему ----" << endl;
        for (int attempt = 1; attempt <= 5; attempt++) {
            cout << "Логин: ";
            string login = trim(inputLine());
            cout << "Пароль: ";
            string password = inputLine();
            for (const auto& account : db.accounts)
                if (account->getLogin() == login && account->getHash() == Account::hashPassword(password))
                    return account;
            cout << "Неверный логин или пароль (попытка " << attempt << " из 5)" << endl;
        }
        cout << "Превышено число попыток входа" << endl;
        waitEnter();
        return nullptr;
    }


    //создание аккаунта
    shared_ptr<Account> createAccount(Database& db, int role) {
        string login = inputText("Логин (3-20 символов):");

        if (!Account::isValidLogin(login))
            throw myException("Логин должен содержать 3-20 символов");

        for (const auto& account : db.accounts)
            if (account->getLogin() == login)
                throw myException("Пользователь с таким логином уже существует");

        cout << "Пароль (не менее 4 символов):";
        string password = inputLine();

        if (!Account::isValidPassword(password))
            throw myException("Пароль должен содержать от 4 до 30 символов без символа ';'");

        cout << "Повторите пароль:";

        if (inputLine() != password)
            throw myException("Пароли не совпадают");

        shared_ptr<Account> account = make_shared<Account>(login, Account::hashPassword(password), role);

        account->setId(nextId(db.accounts));
        db.accounts.push_back(account);

        saveAccounts(db);

        return account;
    }


    void registerUser(Database& db) {
        system("cls");
        cout << "---- Регистрация ----" << endl;
        createAccount(db, 2);
        cout << "\nУспешная регистрация" << endl;
    }



    //нужна для попытки выполнения переданнго действия, но при этом не завершает программу при ошибке
    void runAction(void (*action)(Database&), Database& db) {

        try {
            action(db);
        }
        catch (const inputException&) {
            throw;
        }
        catch (const myException& e) {
            cout << "\nОшибка:" << e.what() << endl;
        }

        waitEnter();
    }


}
