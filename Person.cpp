#include "Person.h"
#include <iostream>

Person::Person() : id(0), name(""), age(0), phone("")
{
    std::cout << "Person created (default)" << std::endl;
}

Person::Person(int id, std::string name, int age, std::string phone)
    : id(id), name(name), age(age), phone(phone)
{
    std::cout << "Person created: " << name << " (ID: " << id << ")" << std::endl;
}

int Person::getId() const
{
    return id;
}

std::string Person::getName() const
{
    return name;
}

int Person::getAge() const
{
    return age;
}

std::string Person::getPhone() const
{
    return phone;
}

void Person::setId(int id)
{
    this->id = id;
}

void Person::setName(std::string name)
{
    this->name = name;
}

void Person::setAge(int age)
{
    this->age = age;
}

void Person::setPhone(std::string phone)
{
    this->phone = phone;
}

Person::~Person()
{
    std::cout << "Person destroyed: " << name << std::endl;
}