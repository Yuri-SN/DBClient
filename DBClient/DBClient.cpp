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
    Wt::Dbo::collection< Wt::Dbo::ptr<Book> > books;

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

    template<class Action>
    void persist(Action& a) {
        Wt::Dbo::field(a, title, "title");
        Wt::Dbo::belongsTo(a, publisher, "publisher");
    }
};

class Shop {
public:
    std::string name = "";
    Wt::Dbo::collection< Wt::Dbo::ptr<Stock> > stocks;

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
        session.mapClass <Shop>("shop");
        session.mapClass<Stock>("stock");
        session.mapClass <Sale>("sale");

        session.createTables();
    }
    catch (const Wt::Dbo::Exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }

    return 0;
}
