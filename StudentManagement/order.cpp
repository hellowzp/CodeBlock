#include <iostream>
#include <sstream>
#include "order.h"

Order::Order(std::string orderTitle, float orderPrice, std::shared_ptr<const Order> req):
  title{orderTitle}, price{orderPrice}, requiredOrder{req}
  {
  }

Order::Order(const Order &rhs):
  title{rhs.getTitle()}, price{rhs.getPrice()}, requiredOrder{rhs.getRequiredOrder()}
  {
  std::cout << "using copy ctor of order" << std::endl;
  }

std::string Order::getTitle() const
  {
  return title;
  }

float Order::getPrice() const
  {
  return price;
  }

void Order::setPrice(float value)
  {
  price = value;
  }

std::shared_ptr<const Order> Order::getRequiredOrder() const
  {
  return requiredOrder;
  }

void Order::setRequiredOrder(const std::shared_ptr<Order> &value)
  {
  requiredOrder = value;
  }

bool Order::operator==(const Order &rhs)
  {
  return title == rhs.getTitle();
  }

std::string Order::getDescription() const
  {
  std::stringstream result;
  result << getTitle() << " for the price of " << getPrice() << std::endl;
  if (getRequiredOrder() != nullptr)
    {
    result << "with a required order" << std::endl;
    }
  return result.str();
  }


std::stringstream &operator<<(std::stringstream &ss, std::shared_ptr<const Order> order)
  {
  ss << order->getDescription();
  return ss;
  }
