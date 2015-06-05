#ifndef STUDENT_H
#define STUDENT_H
#include <string>
#include <vector>
#include "order.h"
#include "printer.h"

enum Stage {starter, inbetween, graduating};

class Student
  {
  public:
    Student(std::string studName, Stage studStage = starter);
    ~Student();
    std::string getName() const;

    Stage getStage() const;
    void setStage(const Stage &value);

    int addOrder(std::shared_ptr<const Order> newOrder);
    std::string listOrders();
    std::string listOrders(const Printer & aPrinter );

//    Student &operator ++();
    std::vector<std::shared_ptr<const Order> > getOrders() const;

  private:
    const std::string name;
    Stage stage;
    std::vector<std::shared_ptr<const Order>> orders;
  };
std::shared_ptr<Student> operator++(std::shared_ptr<Student> rhs);
std::ostream & operator<<(std::ostream & os, const Stage & stage);
#endif // STUDENT_H
