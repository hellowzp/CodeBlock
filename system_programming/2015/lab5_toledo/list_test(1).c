#include "list.h"
#include <check.h>
#include <stdlib.h>
#include <stdio.h>

list_ptr_t list = NULL;

int list_errno;

/*
 * Copy the 'content' of src_element to dst_element.
 */
void element_copy(element_ptr_t *dest_element, element_ptr_t src_element)
{
  //...
}


/*
 * Clean up element, including freeing memory if needed
 */
void element_free(element_ptr_t *element)
{
  // ...
}

/*
 * Print 1 element to stdout. 
 */
void element_print(element_ptr_t element)
{
 // ...
}

/*
 * Compare two element elements; returns -1, 0 or 1 
 */
int element_compare(element_ptr_t x, element_ptr_t y)
{
  // ...
}


void setup(void) {
  list = list_create( &element_copy, &element_free, &element_compare, &element_print);
  ck_assert_msg(list_errno == 0,"Failure: unexpected error: %d\n",list_errno);
  ck_assert_msg(list != NULL, "Unable to create a new list");
  ck_assert_msg(list_size(list) == 0, "Failure: expected a newly created list with size:<0> but was %d\n",list_size(list));
}

void teardown(void) {
  list_free(&list);
  ck_assert_msg(list_errno == 0,"Failure: unexpected error: %d\n",list_errno);
  ck_assert_msg(list == NULL, "Failure: expected list:<NULL>\n");
}

START_TEST(test_ListInsert)
{
  int* a = malloc(sizeof(int));
  *a = 3;
  int* b = malloc(sizeof(int));
  *b = 6;
  int* c = malloc(sizeof(int));
  *c = 9;
  list_ptr_t list = NULL;
  list = list_create( &element_copy, &element_free, &element_compare, &element_print);
  list_insert_at_index(NULL,a,0);
  ck_assert_msg(list_errno != 0,"Failure: unexpected error: %d\n",list_errno);
  list = list_insert_at_index(list,a,0);
  ck_assert_msg(list_errno == 0,"Failure: unexpected error: %d\n",list_errno);
  ck_assert_msg(list_size(list) == 1, "Failure: expected size:<1> but was %d\n",list_size(list));
  ck_assert_msg(*(int*)list_get_element_at_index(list, 0) == 3, "Failure: expected first element:<3> but was %d\n",*(int*)list_get_element_at_index(list, 0));
  ck_assert_msg(*(int*)list_get_element_at_index(list, 10) == 3, "Failure: expected last element:<3> but was %d\n",*(int*)list_get_element_at_index(list, 10));
  list = list_insert_at_index(list,b,5);
  ck_assert_msg(list_errno == 0,"Failure: unexpected error: %d\n",list_errno);
  ck_assert_msg(list_size(list) == 2, "Failure: expected size:<2> but was %d\n",list_size(list));
  ck_assert_msg(*(int*)list_get_element_at_index(list, 0) == 3, "Failure: expected first element:<3> but was %d\n",*(int*)list_get_element_at_index(list, 0));
  ck_assert_msg(*(int*)list_get_element_at_index(list, 10) == 6, "Failure: expected last element:<6> but was %d\n",*(int*)list_get_element_at_index(list, 10));
  list = list_insert_at_index(list,c,-1);
  ck_assert_msg(list_errno == 0,"Failure: unexpected error: %d\n",list_errno);
  ck_assert_msg(list_size(list) == 3, "Failure: expected size:<3> but was %d\n",list_size(list));
  ck_assert_msg(*(int*)list_get_element_at_index(list, 0) == 9, "Failure: expected first element:<9> but was %d\n",*(int*)list_get_element_at_index(list, 0));
  ck_assert_msg(*(int*)list_get_element_at_index(list, 10) == 6, "Failure: expected last element:<6> but was %d\n",*(int*)list_get_element_at_index(list, 10));
  list_free(&list);
  ck_assert_msg(list_errno == 0,"Failure: unexpected error: %d\n",list_errno);
  ck_assert_msg(list == NULL, "Failure: expected list:<NULL>\n");
}
END_TEST

START_TEST(test_ListRemove)
{
  int* a = malloc(sizeof(int));
  *a = 3;
  int* b = malloc(sizeof(int));
  *b = 6;
  int* c = malloc(sizeof(int));
  *c = 9;
  list_ptr_t list = NULL;
  list = list_create( &element_copy, &element_free, &element_compare, &element_print);
  list = list_remove_at_index(list,0);
  ck_assert_msg(list_errno != 0,"Failure: unexpected error: %d\n",list_errno);
  ck_assert_msg(list_size(list) == 0, "Failure: removing from empty list expected size:<0> but was %d\n",list_size(list));
  list = list_insert_at_index(list,a,0);
  list = list_insert_at_index(list,b,5);
  list = list_insert_at_index(list,c,-1);
  list = list_remove_at_index(list,-10);
  ck_assert_msg(list_errno == 0,"Failure: unexpected error: %d\n",list_errno);
  ck_assert_msg(list_size(list) == 2, "Failure: expected size:<2> but was %d\n",list_size(list));
  ck_assert_msg(*(int*)list_get_element_at_index(list, 0) == 3, "Failure: expected first element:<3> but was %d\n",*(int*)list_get_element_at_index(list, 0));
  ck_assert_msg(*(int*)list_get_element_at_index(list, 10) == 6, "Failure: expected last element:<6> but was %d\n",*(int*)list_get_element_at_index(list, 10));
  free(c);
  list = list_remove_at_index(list,7);
  ck_assert_msg(list_errno == 0,"Failure: unexpected error: %d\n",list_errno);
  ck_assert_msg(list_size(list) == 1, "Failure: expected size:<1> but was %d\n",list_size(list));
  ck_assert_msg(*(int*)list_get_element_at_index(list, 0) == 3, "Failure: expected first element:<3> but was %d\n",*(int*)list_get_element_at_index(list, 0));
  ck_assert_msg(*(int*)list_get_element_at_index(list, 10) == 3, "Failure: expected last element:<3> but was %d\n",*(int*)list_get_element_at_index(list, 10));
  free(b);
  list = list_free_at_index(list,7);
  ck_assert_msg(list_errno == 0,"Failure: unexpected error: %d\n",list_errno);
  ck_assert_msg(list_size(list) == 0, "Failure: expected size:<0> but was %d\n",list_size(list));
  list = list_remove_at_index(list,7);
  ck_assert_msg(list_errno != 0,"Failure: unexpected error: %d\n",list_errno);
  list = list_free_at_index(list,7);
  ck_assert_msg(list_errno != 0,"Failure: unexpected error: %d\n",list_errno);
  ck_assert_msg(list_size(list) == 0, "Failure: removing from empty list expected size:<0> but was %d\n",list_size(list));
  list_free(&list);
  ck_assert_msg(list == NULL, "Failure: expected list:<NULL>\n");
}
END_TEST

START_TEST(test_ListIndexOf)
{
  int* a = malloc(sizeof(int));
  *a = 3;
  int* b = malloc(sizeof(int));
  *b = 6;
  int* c = malloc(sizeof(int));
  *c = 9;
  list_ptr_t list = NULL;
  list = list_create( &element_copy, &element_free, &element_compare, &element_print);
  list = list_insert_at_index(list,a,0);
  list = list_insert_at_index(list,b,5);
  list = list_insert_at_index(list,c,-1);
  list_print(list);
  ck_assert_msg(list_get_index_of_element(list, a) == 1, "Failure: expected index:<1> but was %d\n",list_get_index_of_element(list, a));
  ck_assert_msg(list_get_index_of_element(list, b) == 2, "Failure: expected index:<2> but was %d\n",list_get_index_of_element(list, b));
  ck_assert_msg(list_get_index_of_element(list, c) == 0, "Failure: expected index:<0> but was %d\n",list_get_index_of_element(list, c));
  list = list_remove_at_index(list,-10);
  ck_assert_msg(list_get_index_of_element(list, a) == 0, "Failure: expected index:<0> but was %d\n",list_get_index_of_element(list, a));
  ck_assert_msg(list_get_index_of_element(list, b) == 1, "Failure: expected index:<1> but was %d\n",list_get_index_of_element(list, b));
  list_free(&list);
  ck_assert_msg(list == NULL, "Failure: expected list:<NULL>\n");
}
END_TEST

START_TEST(test_ListGetReference)
{
  char* a = malloc(sizeof(char));
  *a = 'x';
  char* b = malloc(sizeof(char));
  *b = 'y';
  char* c = malloc(sizeof(char));
  *c = 'z';
  list_ptr_t list = NULL;
  list = list_create( &element_copy, &element_free, &element_compare, &element_print);;
  list = list_insert_at_index(list,a,0);
  list = list_insert_at_index(list,b,5);
  list = list_insert_at_index(list,c,-1);
  //ck_assert_msg(list_get_reference_at_index(list,0) == list, "Failure: expected address:<%p> but was %p\n", list, list_get_reference_at_index(list,0));
  ck_assert_msg(list_get_reference_at_index(list,0) != list_get_reference_at_index(list,1), "Failure: two elements with same reference");
  long int interval = (long int)list_get_reference_at_index(list,1) - (long int)list_get_reference_at_index(list,0);
  //ck_assert_msg(list_get_reference_at_index(list,10) == ((void*)list)+2*interval, "Failure: expected address:<%p> but was %p\n", ((void*)list)+2*interval, list_get_reference_at_index(list,10));
  list_free(&list);
}
END_TEST


int main(void)
{
    Suite *s1 = suite_create("LIST");
    TCase *tc1_1 = tcase_create("Core");
    SRunner *sr = srunner_create(s1);
    int nf;

    suite_add_tcase(s1, tc1_1);
    tcase_add_checked_fixture (tc1_1, setup, teardown);
    tcase_add_test(tc1_1, test_ListInsert);
    tcase_add_test(tc1_1, test_ListRemove);
    tcase_add_test(tc1_1, test_ListIndexOf);
    tcase_add_test(tc1_1, test_ListGetReference);

    srunner_run_all(sr, CK_NORMAL);
   
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return nf == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
