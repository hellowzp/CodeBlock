#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iterator>
#include <queue>

#include <vector>       // std::vector

using namespace std;
 
template<class T, class Compare>
class PQV : public vector<T> {
Compare comp;
bool sorted;

void assureHeap() {
    if(sorted) {
        // Turn it back into a heap:
        make_heap(this->begin(),this->end(), comp);
        sorted = false;
    }
}

public:
PQV(Compare cmp = Compare()) : comp(cmp) {
    make_heap(this->begin(),this->end(), comp);
    sorted = false;
}
const T& top() {
    assureHeap();
    return this->front();
}
void push(const T& x) {
    assureHeap();
    this->push_back(x); // Put it at the end
    // Re-adjust the heap:
    push_heap(this->begin(),this->end(), comp);
}
void pop() {
    assureHeap();
    // Move the top element to the last position:
    pop_heap(this->begin(),this->end(), comp);
    cout << this->size();
    this->pop_back();// Remove that element
}
void sort() {
    if(!sorted) {
        sort_heap(this->begin(),this->end(), comp);
        reverse(this->begin(),this->end());
        sorted = true;
    }
}
};
 
int main() {
PQV< int, less<int> > pqi;
srand(time(0));
for(int i = 0; i < 10; i++) {
pqi.push(rand() % 25);
copy(pqi.begin(), pqi.end(),
ostream_iterator<int>(cout, " "));
cout << "\n-----" << endl;
}

//pqi.sort();
copy(pqi.begin(), pqi.end(),
ostream_iterator<int>(cout, " "));
cout << "\n-----" << endl;
while(!pqi.empty()) {
cout << pqi.top() << ' ';
pqi.pop();
}

  int myints[] = {10,20,30,5,15,25,28,35};
  std::vector<int> v(myints,myints+8);

  std::cout << "\n\ninitial order :";
  for (unsigned i=0; i<v.size(); i++)
    std::cout << ' ' << v[i];

  std::cout << '\n';
  
  std::make_heap (v.begin(),v.end());
  std::cout << "initial max heap   : " << v.front() << '\n';

  std::cout << "make_heap :";
  for (unsigned i=0; i<v.size(); i++)
    std::cout << ' ' << v[i];

  std::cout << '\n';

  std::pop_heap (v.begin(),v.end()); 
  std::cout << "pop_heap  :";
  for (unsigned i=0; i<v.size(); i++)
    std::cout << ' ' << v[i];

  std::cout << '\n';
  v.pop_back();
  std::cout << "max heap after pop : " << v.front() << '\n';

  v.push_back(99); std::push_heap (v.begin(),v.end());
  std::cout << "max heap after push: " << v.front() << '\n';

  std::sort_heap (v.begin(),v.end());

  std::cout << "final sorted range :";
  for (unsigned i=0; i<v.size(); i++)
    std::cout << ' ' << v[i];

  std::cout << '\n';

} ///:~