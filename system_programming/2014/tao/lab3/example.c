 // Abstract data type queue
   // queue is a queue of integers

   // queue invariant : new integers are added to the back of the queue
   //                   integers are only removed from the front of the queue

   // operations for queue are defined as follows

   // queue empty()
   //    returns an empty queue

   // int empty_huh(queue q)
   //    returns 1 (true) if q is empty
   //    returns 0 (false) if q is not empty

   // queue enqueue(int in, queue q)
   //    returns a queue with in added to the back of q

   // queue dequeue(queue q)
   //    returns a queue with the element at the front of q removed
   //    returns empty if q is empty
   //    this operation is *destructive*

   // int next(queue q)
   //    returns the element at the front of q
#include<stdio.h>
#include<stdlib.h>
// Abstract data type queue
// queue is a queue of integers
struct node{
 int value;
 struct node * next;  // 队列一般是访问受限制的链表
};
typedef struct queue{
 struct node * head, * tail; // 每个队列维护头/尾指针
}queue;

// queue invariant : new integers are added to the back of the queue
//                   integers are only removed from the front of the queue

// operations for queue are defined as follows

// queue empty()
//    returns an empty queue
queue empty()
{
 queue q;
 q.head = q.tail = NULL;
 return q;
}
   
// int empty_huh(queue q)
//    returns 1 (true) if q is empty
//    returns 0 (false) if q is not empty
int empty_huh(queue q)
{
 if(q.head == NULL)
  return 1;
 else
  return 0;
}

// queue enqueue(int in, queue q)
//    returns a queue with in added to the back of q
queue enqueue(int in, queue q)
{
 struct node * item = (struct node *)malloc(sizeof(struct node));
 item->value = in;
 item->next = NULL;

 if(empty_huh(q))
  q.head = q.tail = item;
 else
 {
  q.tail->next = item;
  q.tail = item;
 }
 return q;
};

// queue dequeue(queue q)
//    returns a queue with the element at the front of q removed
//    returns empty if q is empty
//    this operation is *destructive*
queue dequeue(queue q)
{
 struct node * temp;
 if(q.head)
 {
  temp = q.head;
  q.head = q.head->next;
  free(temp);
 }
 if(empty_huh(q))
  return empty();
 return q;
}

// int next(queue q)
//    returns the element at the front of q 
int next(queue q)
{
 return q.head->value;
}

// main function for testing
void main()
{
 int i;
 queue q = empty();
 
 for(i=1; i<=10; i++)
  q = enqueue(i, q);
 while(! empty_huh(q))
 {
  printf("%d ", next(q));
  q = dequeue(q);
 }
}
