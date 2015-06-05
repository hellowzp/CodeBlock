#ifndef ACTIVITY_H
#define ACTIVITY_H
#include "order.h"
#include <QDateTime>

class Activity : public Order
  {
  public:
    Activity(std::string orderTitle, float orderPrice, std::shared_ptr<Order> req,
             int orderCapacity, QDateTime start,
             QDateTime end);

    virtual std::string getDescription() const;
  private:
    int capacity;
    QDateTime startMoment;
    QDateTime endMoment;
  };

#endif // ACTIVITY_H
