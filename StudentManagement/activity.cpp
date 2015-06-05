#include "activity.h"
#include <sstream>

Activity::Activity(std::string orderTitle, float orderPrice, std::shared_ptr<Order> req,
                   int orderCapacity, QDateTime start, QDateTime end):
  Order(orderTitle, orderPrice, req), capacity{orderCapacity}, startMoment{start}, endMoment{end}
  {
  }

std::string Activity::getDescription() const
  {
  std::stringstream result;
  result << Order::getDescription();
  result << "starting on : " << startMoment.toString().toStdString() << std::endl;
  result << "ending on : " << endMoment.toString().toStdString() << std::endl;
  result << "with a maximum number of " << capacity << " participants" << std::endl;
  return result.str();
  }
