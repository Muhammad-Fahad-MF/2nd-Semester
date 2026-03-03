#include <iostream>
#include<string>
#include<vector>

using namespace std;

enum class Category { SEDAN, SUV, VAN };

class Car{
public:
    string make;
    string model;
    string city;
    Category category;
    int mileage = 0;
    int price = 0;
    string description;

    string getCategory() {
        string c = (string) category;
        return c;
    }
};

class User {
private:
    string name;
    long int phoneNum = 0;
    string email;
    string city;
    vector<Car> ads;

public:
    static vector<Car> allCars;
    bool addAds() {
        Car c;
        cout << "Enter make: ";
        cin >> c.make;
        cout << "Enter model: ";
        cin >> c.model;
        cout << "Enter city: ";
        cin >> city;
        string cat;
        cout << "Enter Category: ";
        cin >> cat;
        if (cat == "SEDAN") {
            c.category = Category::SEDAN;
        } else if (cat == "SUV") {
            c.category = Category::SUV;
        } else if (cat == "VAN") {
            c.category = Category::VAN;
        } else {
            cout << "Invalid Category" << endl;
            return false;
        }
        cout << "Enter Mileage: ";
        cin >> c.mileage;
        cout << "Enter Price: ";
        cin >> c.price;
        cout << "Enter Description: ";
        cin >> c.description;
        ads.push_back(c);
        allCars.push_back(c); // Static member is common for all classes which will behave as all cars pool.
        return true;
    }
    static void printAllCars() {
        for (auto & allCar : allCars) {
            cout << allCar.make << " " << allCar.model << " " << allCar.city << " " << allCar.printCategory() << " " << allCar.mileage << " Rs." << allCar.price << endl;
        }
    }
    void buyCar() {

    }
};

class MarketPlace {
public:
    User user;
    void startLoop() {
        while (true) {
            int choice ;
            cout << "Welcome to MarketPlace " << endl;
            cout << "1. Sell a Car" << endl;
            cout << "2. Buy a Car" << endl;
            cout << "3. Exit" << endl;
            cout << "Enter your choice: ";
            cin >> choice;
            switch (choice) {
                case 1:
                    user.addAds();
                case 2:
                    user.printAllCars();

            }
        }
    }
    MarketPlace() {
        startLoop();
    }
};

int main(){



    return 0;
}