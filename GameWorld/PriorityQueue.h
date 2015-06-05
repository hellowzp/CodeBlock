#ifndef PriorityQUEUE_H
#define PriorityQUEUE_H

#include <QVector>
#include <algorithm>

//priority queue based on heap
template <class T, typename Comparator>
class PriorityQueue : public QVector<T>
{
    Comparator comparator;

public:
    PriorityQueue(const Comparator& comp): comparator(comp) {}

    const T& top() { return this->front(); }
    void push(const T& x) {
        this->push_back(x);
        std::push_heap(this->begin(),this->end(), comparator);
    }
    void pop() {
        std::pop_heap(this->begin(),this->end(), comparator);
        this->pop_back();
    }
    void updatePriority() {
        std::make_heap(this->begin(),this->end(), comparator);
    }
};

#endif // PriorityQUEUE_H

