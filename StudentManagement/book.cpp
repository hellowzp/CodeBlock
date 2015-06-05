#include "book.h"

Book::Book(std::string orderTitle, float orderPrice, std::shared_ptr<const Order> req):
  Order(orderTitle, orderPrice, req)
  {
  }
