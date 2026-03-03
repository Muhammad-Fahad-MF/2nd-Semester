#include <iostream>
#include<string>
#include<vector>

using namespace std;

enum class Category { SEDAN, SUV, VAN };

class Car{
private:
    const int id;
    static int cars_count;
public:
    string make;
    string model;
    string city;
    Category category;
    int mileage = 0;
    int price = 0;
    string description;

    string getCategory() {
        switch (category) {
            case Category::SEDAN:
                return "SEDAN";
            case Category::SUV:
                return "SUV";
            case Category::VAN:
                return "VAN";
            default:
                return "UNKNOWN";
        }
    }

    Car(): id(++cars_count){
        string cat;
        cout << "Enter Category: ";
        cin >> cat;
        while(true){
            if (cat == "SEDAN") {
                this->category = Category::SEDAN;
                break;
            } else if (cat == "SUV") {
                this->category = Category::SUV;
                break;
            } else if (cat == "VAN") {
                this->category = Category::VAN;
                break;
            } else {
                cout << "Invalid Category! Select( SEDAN / SUV / VAN)" << endl;
                continue;
            }
        }
        cout << "Enter make: ";
        cin >> this->make;
        cout << "Enter model: ";
        cin >> this->model;
        cout << "Enter city: ";
        cin >> city;
        cout << "Enter Mileage: ";
        cin >> this->mileage;
        cout << "Enter Price: ";
        cin >> this->price;
        cout << "Enter Description: ";
        cin >> this->description;
    }
};

int Car::cars_count = 0;

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
        ads.push_back(c);
        allCars.push_back(c); // Static member is common for all classes which will behave as all cars pool.
        return true;
    }
    static void printAllCars() {
        for (auto & allCar : allCars) {
            cout << allCar.make << " " << allCar.model << " " << allCar.city << " " << allCar.getCategory() << " " << allCar.mileage << " Rs." << allCar.price << endl;
        }
    }
    void buyCar() {
        cout << "\nChoose One of the Cars: " << endl;
        printAllCars();
        int cId;
        cout << "\nEnter a car id: ";
        cin >> cId;
        
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
                    if(user.addAds()){
                        cout << "Car added successfully!" << endl;
                    } else {
                        cout << "Failed to add car. Please try again." << endl;
                    }
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