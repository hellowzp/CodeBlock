#ifndef BOOK_H
#define BOOK_H
#include "order.h"

class Book : public Order
  {
  public:
    Book(std::string orderTitle, float orderPrice, std::shared_ptr<const Order> req = nullptr);
  };

#endif // BOOK_H
