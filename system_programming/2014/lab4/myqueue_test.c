#include "myqueue.h"
#include <check.h>
#include <stdlib.h>

START_TEST(test_QueueCreate)
{
  Queue* queue = NULL;
  queue = QueueCreate();
  ck_assert_msg(queue != NULL, "Unable to create a new queue");
  ck_assert_msg(QueueSize(queue) == 0, "Failure: expected a newly created queue with size:<0> but was %d\n",QueueSize(queue));
  ck_assert_msg(QueueTop(queue) == NULL, "Failure: expected a newly created queue with a pointer to the first element:<NULL>\n");
}
END_TEST

START_TEST(test_QueueDestroy)
{
  Queue* queue = NULL;
  queue = QueueCreate();
  QueueDestroy(&queue);
  ck_assert_msg(queue == NULL, "Failure: expected queue:<NULL>\n");
}
END_TEST

START_TEST(test_Enqueue)
{
  Queue* queue[] = {NULL, NULL};

  queue[0] = QueueCreate();
  Enqueue(queue[0],12);
  ck_assert_msg(QueueSize(queue[0]) == 1, "Failure: expected queue size:<1> but was %d\n",QueueSize(queue[0]));
  ck_assert_msg(*QueueTop(queue[0]) == 12, "Failure: expected first element:<12> but was %d\n", *QueueTop(queue[0]));
  QueueDestroy(&queue[0]);

  queue[1] = QueueCreate();
  Enqueue(queue[1],12);
  Enqueue(queue[1],15);
  Enqueue(queue[1],17);
  ck_assert_msg(QueueSize(queue[1]) == 3, "Failure: expected queue size:<3> but was %d\n",QueueSize(queue[1]));
  ck_assert_msg(*QueueTop(queue[1]) == 12, "Failure: expected first element:<12> but was %d\n", *QueueTop(queue[1]));		
  QueueDestroy(&queue[1]);
}
END_TEST

START_TEST(test_Dequeue)
{
  Queue* queue[] = {NULL, NULL, NULL, NULL};

  queue[0] = QueueCreate();
  Dequeue(queue[0]);
  ck_assert_msg(QueueSize(queue[0]) == 0, "Failure: expected queue size:<0> but was %d\n",QueueSize(queue[0]));
  QueueDestroy(&queue[0]);

  queue[1] = QueueCreate();
  Dequeue(queue[1]);
  ck_assert_msg(QueueSize(queue[1]) == 0, "Failure: expected queue size:<0> but was %d\n",QueueSize(queue[1]));
  Enqueue(queue[1],12);
  ck_assert_msg(QueueSize(queue[1]) == 1, "Failure: expected queue size:<1> but was %d\n",QueueSize(queue[1]));
  ck_assert_msg(*QueueTop(queue[1]) == 12, "Failure: expected first element:<12> but was %d\n", *QueueTop(queue[1])); 
  QueueDestroy(&queue[1]);

  queue[2] = QueueCreate();
  Enqueue(queue[2],12);
  Enqueue(queue[2],15);
  Dequeue(queue[2]);
  ck_assert_msg(*QueueTop(queue[2]) == 15, "Failure: after dequeue, expected QueueTop() to return:<15> but was %d\n", *QueueTop(queue[2]));
  ck_assert_msg(QueueSize(queue[2]) == 1, "Failure: expected queue size:<1> but was %d\n",QueueSize(queue[2]));
  Dequeue(queue[2]);
  ck_assert_msg(QueueSize(queue[2]) == 0, "Failure: expected queue size:<0> but was %d\n",QueueSize(queue[2]));
  Dequeue(queue[2]);
  ck_assert_msg(QueueSize(queue[2]) == 0, "Failure: expected queue size:<0> but was %d\n",QueueSize(queue[2]));
  QueueDestroy(&queue[2]);  

  queue[3] = QueueCreate();
  Enqueue(queue[3],12);
  Dequeue(queue[3]);
  ck_assert_msg(QueueSize(queue[3]) == 0, "Failure: expected queue size:<0> but was %d\n",QueueSize(queue[3]));
  Enqueue(queue[3],15);
  Enqueue(queue[3],17);
  ck_assert_msg(*QueueTop(queue[3]) == 15, "Failure: expected QueueTop() to return:<15> but was %d\n", *QueueTop(queue[3]));
  ck_assert_msg(QueueSize(queue[3]) == 2, "Failure: expected queue size:<2> but was %d\n",QueueSize(queue[3]));
  QueueDestroy(&queue[3]);
}
END_TEST

int main(void)
{
    Suite *s1 = suite_create("Queue");
    TCase *tc1_1 = tcase_create("Core");
    SRunner *sr = srunner_create(s1);
    int nf;

    suite_add_tcase(s1, tc1_1);
    tcase_add_test(tc1_1, test_QueueCreate);
    tcase_add_test(tc1_1, test_QueueDestroy);
    tcase_add_test(tc1_1, test_Enqueue);
    tcase_add_test(tc1_1, test_Dequeue);

    srunner_run_all(sr, CK_NORMAL);
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return nf == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
