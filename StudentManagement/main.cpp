#include <memory>
#include <iostream>
#include "student.h"
#include "order.h"
#include "book.h"
#include "activity.h"

int main()
  {
  Student s1{"Ban", inbetween};
  std::cout << s1.getStage() << std::endl;
  auto ps = std::make_shared<Student>("Jun");
  ++ps;
  std::cout << ps->getStage() << std::endl;
  auto b1 = std::make_shared<const Book>("The new C++", 43.50f);
  auto b2 = std::make_shared<const Book>("Advanced C++", 12.35f, b1);
  s1.addOrder(b1);
  s1.addOrder(b2);
  auto a1 = std::make_shared<const Activity>("Trip to Lier", 11.0f, nullptr, 60,
              QDateTime(QDate(2014, 9, 25), QTime(12, 30)),
              QDateTime(QDate(2014, 9, 26), QTime(15, 00))
              );
  s1.addOrder(a1);
  std::cout << s1.listOrders();
  auto vec = s1.getOrders();
  std::cout << vec[0]->getDescription();
  }
