#include <iostream>
#include <vector>

#include <Windows.h>

#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/backend/Postgres.h>

class Publisher;
class Book;
class Shop;
class Stock;
class Sale;

class Publisher {
public:
	std::string name = "";
	Wt::Dbo::collection<Wt::Dbo::ptr<Book>> books;

	template<class Action>
	void persist(Action& a) {
		Wt::Dbo::field(a, name, "name");
		Wt::Dbo::hasMany(a, books, Wt::Dbo::ManyToOne, "publisher");
	}
};

class Book {
public:
	std::string title = "";
	Wt::Dbo::ptr<Publisher> publisher;
	Wt::Dbo::collection<Wt::Dbo::ptr<Stock>> stocks;

	template<class Action>
	void persist(Action& a) {
		Wt::Dbo::field(a, title, "title");
		Wt::Dbo::belongsTo(a, publisher, "publisher");
		Wt::Dbo::hasMany(a, stocks, Wt::Dbo::ManyToOne, "book");
	}
};

class Shop {
public:
	std::string name = "";
	Wt::Dbo::collection<Wt::Dbo::ptr<Stock>> stocks;

	template<class Action>
	void persist(Action& a) {
		field(a, name, "name");
		Wt::Dbo::hasMany(a, stocks, Wt::Dbo::ManyToOne, "shop");
	}
};

class Stock {
public:
	Wt::Dbo::ptr<Book> book;
	Wt::Dbo::ptr<Shop> shop;
	int count = 0;

	template<class Action>
	void persist(Action& a) {
		Wt::Dbo::belongsTo(a, book, "book");
		Wt::Dbo::belongsTo(a, shop, "shop");
		field(a, count, "count");
	}
};

class Sale {
public:
	double price = 0.0;
	std::string date_sale = "";
	Wt::Dbo::ptr<Stock> stock;
	int count = 0;

	template<class Action>
	void persist(Action& a) {
		Wt::Dbo::field(a, price, "price");
		Wt::Dbo::field(a, date_sale, "date_sale");
		Wt::Dbo::belongsTo(a, stock, "stock");
		Wt::Dbo::field(a, count, "count");
	}
};

int main() {
	SetConsoleCP(65001);
	SetConsoleOutputCP(65001);
	try {
		std::string connectionString =
			"host=localhost "
			"port=5432 "
			"dbname=book_store "
			"user=postgres "
			"password=123456";

		auto postgres = std::make_unique<Wt::Dbo::backend::Postgres>(connectionString);

		Wt::Dbo::Session session;
		session.setConnection(std::move(postgres));

		session.mapClass<Publisher>("publisher");
		session.mapClass<Book>("book");
		session.mapClass<Shop>("shop");
		session.mapClass<Stock>("stock");
		session.mapClass<Sale>("sale");

		// Запрос пользователя о создании таблиц
		char create_tables;
		std::cout << "Create tables and fill them with test data? (y/n): ";
		std::cin >> create_tables;

		if (create_tables == 'y' || create_tables == 'Y') {

			session.createTables();

			// Заполнение тестовыми данными

			Wt::Dbo::Transaction transaction(session);

			auto publisherP = session.add(std::make_unique<Publisher>());
			publisherP.modify()->name = "PenguinBooks";

			auto publisherB = session.add(std::make_unique<Publisher>());
			publisherB.modify()->name = "BearBooks";

			auto bookA = session.add(std::make_unique<Book>());
			bookA.modify()->title = "Aladdin";
			bookA.modify()->publisher = publisherP;

			auto bookP = session.add(std::make_unique<Book>());
			bookP.modify()->title = "Pinocchio";
			bookP.modify()->publisher = publisherP;

			auto bookJ = session.add(std::make_unique<Book>());
			bookJ.modify()->title = "The Jungle Book";
			bookJ.modify()->publisher = publisherB;

			auto shopA = session.add(std::make_unique<Shop>());
			shopA.modify()->name = "Bookstore A";

			auto shopB = session.add(std::make_unique<Shop>());
			shopB.modify()->name = "Bookstore B";

			auto stock1 = session.add(std::make_unique<Stock>());
			stock1.modify()->book = bookA;
			stock1.modify()->shop = shopA;
			stock1.modify()->count = 10;

			auto stock2 = session.add(std::make_unique<Stock>());
			stock2.modify()->book = bookA;
			stock2.modify()->shop = shopB;
			stock2.modify()->count = 5;

			auto stock3 = session.add(std::make_unique<Stock>());
			stock3.modify()->book = bookP;
			stock3.modify()->shop = shopA;
			stock3.modify()->count = 3;

			auto stock4 = session.add(std::make_unique<Stock>());
			stock4.modify()->book = bookJ;
			stock4.modify()->shop = shopA;
			stock4.modify()->count = 6;

			auto stock5 = session.add(std::make_unique<Stock>());
			stock5.modify()->book = bookJ;
			stock5.modify()->shop = shopB;
			stock5.modify()->count = 8;

			auto sale1 = session.add(std::make_unique<Sale>());
			sale1.modify()->price = 15.99;
			sale1.modify()->date_sale = "2025-03-01";
			sale1.modify()->stock = stock1;
			sale1.modify()->count = 1;

			auto sale2 = session.add(std::make_unique<Sale>());
			sale2.modify()->price = 5.49;
			sale2.modify()->date_sale = "2025-02-25";
			sale2.modify()->stock = stock2;
			sale2.modify()->count = 1;

			auto sale3 = session.add(std::make_unique<Sale>());
			sale3.modify()->price = 8.49;
			sale3.modify()->date_sale = "2025-02-20";
			sale3.modify()->stock = stock3;
			sale3.modify()->count = 1;

			auto sale4 = session.add(std::make_unique<Sale>());
			sale4.modify()->price = 2.24;
			sale4.modify()->date_sale = "2025-02-18";
			sale4.modify()->stock = stock4;
			sale4.modify()->count = 1;

			auto sale5 = session.add(std::make_unique<Sale>());
			sale5.modify()->price = 1.24;
			sale5.modify()->date_sale = "2025-02-11";
			sale5.modify()->stock = stock5;
			sale5.modify()->count = 3;

			transaction.commit();
		}

		// Запрос магазинов, продающих книги целевого издателя
		std::string publisher_name;
		std::cout << "Enter publisher name: ";
		std::cin >> publisher_name;

		Wt::Dbo::Transaction transaction(session);

		// Находим издателя по имени
		Wt::Dbo::ptr<Publisher> publisher = session.find<Publisher>().where("name = ?").bind(publisher_name);

		if (publisher) {
			std::cout << "Shops selling books by " << publisher_name << ":\n";
			
			// Используем std::set для хранения уникальных названий магазинов
			std::set<std::string> uniqueShops;

			// Перебираем книги издателя
			for (const Wt::Dbo::ptr<Book>& book : publisher->books) {
				// Перебираем запасы (stock) для каждой книги
				for (const Wt::Dbo::ptr<Stock>& stock : book->stocks) {
					// Получаем магазин, связанный с запасом
					Wt::Dbo::ptr<Shop> shop = stock->shop;
					// Добавляем название магазина в std::set
					uniqueShops.insert(shop->name);
				}
			}
			// Выводим уникальные названия магазинов
			for (const std::string& shop_name : uniqueShops) {
				std::cout << shop_name << std::endl;
			}
		}
		else {
			std::cout << "Publisher not found!" << std::endl;
		}

		transaction.commit();
	}
	catch (const Wt::Dbo::Exception& e) {
		std::cout << "Error: " << e.what() << std::endl;
	}

	return 0;
}
