#include <iostream>
#include <sstream>
#include <algorithm>
#include "student.h"

Student::Student(std::string studName, Stage studStage):
  name{studName}, stage{studStage}
  {
  orders.reserve(3);
  std::cout << "Constructing student with name " << name << "(" << stage << ")" << std::endl;
  }

Student::~Student()
  {
  std::cout << "Deleting student with name " << name << std::endl;
  }

std::string Student::getName() const
  {
  return name;
  }

Stage Student::getStage() const
  {
  return stage;
  }

void Student::setStage(const Stage &value)
  {
  stage = value;
  }

int Student::addOrder(std::shared_ptr<const Order> newOrder)
  {
  if (std::find(std::begin(orders), std::end(orders), newOrder) == std::end(orders))
    {
    orders.push_back(newOrder);
    if (newOrder->getRequiredOrder() != nullptr)
      {
      addOrder(newOrder->getRequiredOrder());
      }
    }
  return orders.size();
  }

std::string Student::listOrders()
  {
  std::stringstream result;
  float totalPrice{0.0f};
  for (auto b : orders)
    {
    result << b << std::endl;
    totalPrice += b->getPrice();
    }
  result << "For a total price of " << totalPrice << std::endl;
  return result.str();
  }

std::string Student::listOrders(const Printer & aPrinter)
  {
  aPrinter.schedule_job("test.pdf", *this);
  }

std::vector<std::shared_ptr<const Order> > Student::getOrders() const
  {
  return orders;
  }


//Student &Student::operator ++()
//  {
//  int i = static_cast<int>(stage);
//  if (i < static_cast<int>(graduating))
//    setStage(static_cast<Stage>(++i));
//  return *this;

//  }
std::shared_ptr<Student> operator ++(std::shared_ptr<Student> rhs)
  {
  int i = static_cast<int>(rhs->getStage());
  if (i < static_cast<int>(graduating))
    rhs->setStage(static_cast<Stage>(++i));
  return rhs;

  }




std::ostream &operator<<(std::ostream &os, const Stage &stage)
  {
  switch(stage)
    {
    case starter: os << "starter";
                  return os;
    case inbetween: os << "inbetween";
                  return os;
    case graduating: os << "graduating";
                  return os;
    default: os << "invalid value for stage";
                   return os;
    }
  }
