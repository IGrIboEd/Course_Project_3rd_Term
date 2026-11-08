#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <exception>
#include <algorithm>
#include <sstream>
#include <memory>
#include <cstdlib>
#include <iomanip>


namespace telecom{

	const std::string file_tarrifs = "tarrifs.txt";
	const std::string file_clients = "clients.txt";
	const std::string file_users = "users.txt";


	//свой класс для обработки своих ошибок
	class myException : public std::exception {
		std::string msg;
	public:
		myException(const std::string message) : msg(message) {}
		const char* what() const noexcept override {
			return msg.c_str();
		}
	};

	//ошибки при работе с файлами
	class fileException : public myException {
	public:
		using myException::myException; // по сути нужно для того, чтобы мог использоваться конструктор мат класса
	};

	//при ошибке при вводе
	class inputException : public myException {
	public:
		using myException::myException;
	};



	//класс для записи даты
	class Date {
	private:
		int day;
		int month;
		int year;
	public:
		Date();
		Date(int d, int m, int y);

        //чёт ещё дописать возможно

        static bool isValid(int d, int m, int y);
        static Date today();
        static int yearsBetween(const Date& from, const Date& to);

        std::string toString() const;

        bool operator<(const Date& other) const;

		friend std::ostream& operator<<(std::ostream& out, const Date& date);

		friend std::istream& operator>>(std::istream& in, Date& date);


	};


    //базовый абстрактный класс для почти всех других классов
    class Entity {
        static int aliveCount;
    protected:
        int id;        
    public:

        Entity() : id(0) {
            aliveCount++;
        }

        Entity(const Entity& other) : id(other.id) {
            aliveCount++;
        }

        virtual ~Entity() {
            aliveCount--;
        }

        Entity& operator=(const Entity& other) = default;

        

        int getId() const {
            return id;
        }
        void setId(int newId) {
            id = newId;
        }
        static int getAliveCount() {
            return aliveCount;
        }


        //чисто виртуальные функции для перегрузок
        virtual void write(std::ostream& out) const = 0;
        virtual void read(std::istream& in) = 0;


        friend std::ostream& operator<<(std::ostream& out, const Entity& e) {
            e.write(out);
            return out;
        }

        friend std::istream& operator>>(std::istream& in, Entity& e) {
            e.read(in);
            return in;
        }

    };

    //абстрактный класс для тарифов
    class Tariff : public Entity {
    protected:

        std::string name;
        double fee = 0; //плата в месяц
        int minutes = 0; //кол-во минут в пакете
        int sms = 0; //кол-во смсок в пакете
        int gb = 0; //кол-во гигов в пакете


        double priceMinute = 0; //цена минуты если использованно больше чем в пакете
        double priceSms = 0; //цена смсок если использованно больше чем в пакете
        double priceGb = 0; //цена гига если использованно больше чем в пакете

        //сколько израсходовано больше чем есть в тарифе
        static double over(double used, double included) {
            return used > included ? used - included : 0;
        }

    public:
        const std::string& getName() const {
            return name;
        }
        double getFee() const {
            return fee;
        }
        int getMinutes() const {
            return minutes;
        }
        int getSms() const {
            return sms;
        }
        int getGb() const {
            return gb;
        }
        double getPriceMinute() const {
            return priceMinute;
        }
        double getPriceSms() const {
            return priceSms;
        }
        double getPriceGb() const {
            return priceGb;
        }

        void set(const std::string& newName, double newFee, int newMinutes, int newSms, int newGb,
            double newPriceMinute, double newPriceSms, double newPriceGb) {
            name = newName;
            fee = newFee;
            minutes = newMinutes;
            sms = newSms;
            gb = newGb;
            priceMinute = newPriceMinute;
            priceSms = newPriceSms;
            priceGb = newPriceGb;
        }

        virtual int getType() const = 0; //1-ограниченный, 2-безлимитный

        virtual double calculateCost(int usedMinutes, int usedSms, double usedGb) const = 0;

        virtual std::string internetText() const {
            return std::to_string(gb) + " ГБ";
        }

        void write(std::ostream& out) const override;
        void read(std::istream& in) override;

    };



    //класс для тарифа с ограничениями
    class PackageTariff : public Tariff {
    public:

        int getType() const override {
            return 1;
        }

        double calculateCost(int usedMinutes, int usedSms, double usedGb) const override;

    };

    //класс для тарифа без ограничений
    class UnlimitedTariff : public Tariff {
    public:

        int getType() const override {
            return 2;
        }

        double calculateCost(int usedMinutes, int usedSms, double usedGb) const override;

        std::string internetText() const override {
            return "безлимит";
        }

    };


    //класс для клиента
    class Client : public Entity {
        std::string fullName;
        std::string phone;

        int tariffId = 0;
        double balance = 0;

        Date connectDate;

        int usedMinutes = 0;
        int usedSms = 0;
        double usedGb = 0;

        bool blocked = false;

    public:
        const std::string& getFullName() const {
            return fullName;
        }
        const std::string& getPhone() const {
            return phone;
        }

        int getTariffId() const {
            return tariffId;
        }
        double getBalance() const {
            return balance;
        }
        const Date& getConnectDate() const {
            return connectDate;
        }
        int getUsedMinutes() const {
            return usedMinutes;
        }
        int getUsedSms() const {
            return usedSms;
        }
        double getUsedGb() const {
            return usedGb;
        }
        bool isBlocked() const {
            return blocked;
        }


        void setFullName(const std::string& value) {
            fullName = value;
        }
        void setPhone(const std::string& value) {
            phone = value;
        }
        void setTariffId(int value) {
            tariffId = value;
        }
        void setConnectDate(const Date& value) {
            connectDate = value;
        }
        void setBlocked(bool value) {
            blocked = value;
        }
        void setUsage(int minutes, int sms, double gb) {
            usedMinutes = minutes;
            usedSms = sms;
            usedGb = gb;
        }


        Client& operator+=(double amount);
        bool charge(double amount); //списание; false, если денег не хватает
        int getDiscount() const; //скидка в процентах за кол-во лет у оператора
        static bool isValidPhone(const std::string& value);

        void write(std::ostream& out) const override;
        void read(std::istream& in) override;

    };


    //класс для аккаунта
    class Account : public Entity {
        std::string login;
        unsigned long long hash = 0;
        int role = 2;// 1-админ, 2-обычный пользователь
    public:

        Account() {}
        Account(const std::string& newLogin, unsigned long long newHash, int newRole) : login(newLogin), hash(newHash), role(newRole) {}

        const std::string& getLogin() const {
            return login;
        }
        unsigned long long getHash() const {
            return hash;
        }
        int getRole() const {
            return role;
        }
        void setHash(unsigned long long value) {
            hash = value;
        }
        std::string getRoleName() const {
            return role == 1 ? "администратор" : "пользователь";
        }

        static unsigned long long hashPassword(const std::string& password);
        static bool isValidLogin(const std::string& value);
        static bool isValidPassword(const std::string& value);

        void write(std::ostream& out) const override;
        void read(std::istream& in) override;
    };

    //структурка для хранения данных из файлов

    struct Database {
        std::vector<std::shared_ptr<Tariff>> tariffs;
        std::vector<std::shared_ptr<Client>> clients;
        std::vector<std::shared_ptr<Account>> accounts;

        std::shared_ptr<Account> currentUser;
    };
}