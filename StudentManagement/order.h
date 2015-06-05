#ifndef ORDER_H
#define ORDER_H
#include <string>
#include <memory>
#include <iostream>

class Order
  {
  public:
    explicit Order(std::string orderTitle, float orderPrice = 0.0f, std::shared_ptr<const Order> req = nullptr);
    Order(const Order & rhs);
    virtual ~Order() = default;
    std::string getTitle() const;

    float getPrice() const;
    void setPrice(float value);

    std::shared_ptr<const Order> getRequiredOrder() const;
    void setRequiredOrder(const std::shared_ptr<Order> &value);

    bool operator==(const Order & rhs);
    virtual std::string getDescription() const;

  private:
    const std::string title;
    float price;
    std::shared_ptr<const Order> requiredOrder;
  };

std::stringstream & operator<<(std::stringstream & ss, std::shared_ptr<const Order> order);

#endif // ORDER_H
