#include<cstdio>
#include <iostream>
#include<string>
#include<vector>

using namespace std;

enum class Category { SEDAN, SUV, VAN };

class Car {
public:
    string make;
    string model;
    string city;
    string category;
    int mileage = 0;
    int price = 0;
    string description;

};

class User {
private:
    string name;
    long int phoneNum = 0;
    string email;
    string city;
    vector<Car> ads;
public:
    void addAds() {
        Car c;
        cout<<"Enter make: " ;
        cin >> c.make;
        cout<<"Enter model: " ;
        cin >> c.model;
        ads.push_back(car);
    }
};

int main(){



    return 0;
}