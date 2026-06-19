#ifndef PERSON_H
#define PERSON_H

#include <iostream>
#include <string>

using namespace std;

class Person
{
private:

    int id;
    string name;
    int age;
    string phone;

public:

    Person();

    Person(int id, string name, int age, string phone);

    int getId() const;
    string getName() const;
    int getAge() const;
    string getPhone() const;

    void setId(int id);
    void setName(string name);
    void setAge(int age);
    void setPhone(string phone);

    virtual void displayInfo() = 0;

    virtual ~Person();
};

#endif