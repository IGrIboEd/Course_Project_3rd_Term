#pragma once
#include "Classes.h"
#include <ctime>

//потом закинуть все прототипы функций

namespace telecom {

    void waitEnter();
    void printLine(char symbol, int length);
    std::string money(double value);

    std::string trim(const std::string& s);

    std::string inputLine();
    std::string inputText(const std::string& prompt);

    void loadAll(Database& db);


    std::shared_ptr<Account> loginUser(Database& db);
    std::shared_ptr<Account> createAccount(Database& db, int role);
    void registerUser(Database& db);


    void mainMenu(Database& db);
    void tariffsMenu(Database& db);
    void clientsMenu(Database& db);
    void usersMenu(Database& db);

    void runAction(void (*action)(Database&), Database& db);

    template <typename T>
    void saveToFile(const std::vector<std::shared_ptr<T>>& list, const std::string& fileName) {

        std::ofstream out(fileName);

        if (!out.is_open())
            throw fileException("Не удалось открыть файл для записи:" + fileName);

        for (const auto& item : list)
            out << *item << '\n';

        if (out.fail())
            throw fileException("Ошибка записи в файл:" + fileName);
    }

    // прочитать объекты из файла; reader - функция, создающая объект из одной строки
    template <typename T, typename Reader>
    void loadFromFile(std::vector<std::shared_ptr<T>>& list, const std::string& fileName, Reader reader) {
        std::ifstream in(fileName);

        if (!in.is_open())
            return;

        list.clear();

        std::string line;
        int lineNumber = 0;

        while (std::getline(in, line)) {

            lineNumber++;

            if (line.empty())
                continue;

            std::istringstream ss(line);

            try {
                list.push_back(reader(ss));
            }

            catch (const myException& e) {
                throw fileException("Файл " + fileName + ", строка " + std::to_string(lineNumber) + ":" + e.what());
            }

        }
    }

    template <typename T>
    int nextId(const std::vector<std::shared_ptr<T>>& list) {
        int maxId = 0;
        for (const auto& item : list)
            if (item->getId() > maxId)
                maxId = item->getId();
        return maxId + 1;
    }


    template <typename T>
    T readNumber(const std::string& prompt, T minValue, T maxValue) {
        while (true) {

            std::cout << prompt;
            std::istringstream ss(inputLine());

            T value;
            char extra;
            
            if ((ss >> value) && !(ss >> extra) && value >= minValue && value <= maxValue) //число прочитано, после него нет лишних символов и оно в нужном диапазоне
                return value;

            std::cout << "Ошибка: введите число от " << minValue << " до " << maxValue << std::endl;
        }
    }



}