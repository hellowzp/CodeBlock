#include <stdlib.h>
#include <stdio.h>
#include "list.h"

int list_errno=0;
//int i;


list_ptr_t list_alloc ( void )
{
  list_ptr_t loc;
  loc = (list_t *) malloc ( sizeof(list_t) );
	if (loc == NULL )
    {
		list_errno=0;
        return loc;
    }
    loc->front =NULL;
	loc->next =NULL;
	return loc;
}

void list_free_all( list_ptr_t* list )
{
  list_ptr_t target;
  while(*list != NULL)
  {
     target = *list;
     *list = (*list)->next;
     free(target);
     target = NULL;
  }
}

int list_size( list_ptr_t list )
{
  int size=0;
  while(list != NULL)
  {
    size++;
    list = list->next;
  }
  return size;
}


list_ptr_t list_insert_at_index( list_ptr_t list, data_ptr_t data, int index)
{  
  list_ptr_t new_element;
  if(index <= 0)
  {
     new_element= list_alloc();
     new_element->data = data;
     new_element->next = list;
     list->front = new_element;
  }
  else
  {
     if(index >= list_size(list))
     {
       new_element = list_alloc();
       new_element->data = data;
       list_ptr_t last = list;
       while(last->next != NULL)
       {
       last = last->next;
       }
       last->next = new_element;
       new_element->front = last;
     }
     else 
     {
       new_element = list_alloc();
       new_element->data = data;
       list_ptr_t before = list;
       list_ptr_t after;
       int i=0;
       while(i<index-1)
       {
         before = list->next;
         i++;
       }
       after = before->next;
       new_element->front = before;
       new_element->next = after;
       before->next = new_element;
       after->front = new_element;
     }
     new_element = list;
  }
  return new_element;
}


list_ptr_t list_insert_sorted( list_ptr_t list, data_ptr_t data )
{ 
  list_ptr_t new_element;
  new_element = list_alloc();
  new_element->data = data;
  if(*(int*)data <= *(int*)(list->data))
  {
    new_element->next = list;
    list->front = new_element;
    return new_element;
  }
  list_ptr_t before = list;
  list_ptr_t after  = list->next;
  while(after != NULL)
  {
    if(*(int*)data > *(int*)(before->data) && *(int*)data <= *(int*)(after->data))
    {
      break;
    }
    else
    {
      before = after;
      after = after->next;
    }
  }
  if (after != NULL)
  {
    new_element->next = after;
    new_element->front = before;
    before->next = new_element;
    after->front = new_element;
  }
  else
  {
    before->next = new_element;
    new_element->front = before;
  }
  return list;
}


list_ptr_t list_free_at_index( list_ptr_t list, int index)
{
  list_ptr_t new_head;
  if(index <= 0){
    new_head = list->next;
    free(list);
    list = new_head;
    }
  else{
    list_ptr_t target=list;
    int i=0;
    while((i < index) && (i< list_size(list)-1))
    {
      target = target->next;
      i++;
    }
   if(target->front!=NULL) ((list_ptr_t)target->front)->next = target->next;
   if(target->next!=NULL) ((list_ptr_t)target->next)->front = target->front;
    free(target);
   }
   return list;
}


list_ptr_t list_free_data( list_ptr_t list, data_ptr_t data ){
  list_ptr_t target= list;
  char found = 0;
  while((found ==0)&&(target->next !=NULL)){
    if(target->data == data) found = 1;
    else{
      target= target->next;
    }
  }
  if(found ==1){
    if(target == list){
      ((list_ptr_t)list->next)->front = NULL;
      list=list->next;
    }
    else{
   if(target->front!=NULL) ((list_ptr_t)target->front)->next = target->next;
    if(target->next!=NULL) ((list_ptr_t)target->next)->front = target->front;
    }
    free(target);
  }
  return list;
}

list_ptr_t list_remove_at_index( list_ptr_t list, int index){
  list_ptr_t new_head;
  if(index <= 0){
    new_head = list->next;
    list = new_head;}
  else{
    list_ptr_t target=list;
    int i=0;
    while((i < index) && (i< list_size(list)-1))
    {
      target = target->next;
      i++;
    }
    if(target->front!=NULL) ((list_ptr_t)target->front)->next = target->next;
    if(target->next!=NULL) ((list_ptr_t)target->next)->front = target->front;
   }
   return list;
}



list_ptr_t list_remove_data( list_ptr_t list, data_ptr_t data ){
  list_ptr_t target= list;
  char found = 0;
  while((found ==0)&&(target->next !=NULL)){
    if(target->data == data) found = 1;
    else{
      target= target->next;
    }
  }
  if(found ==1){
    if(target == list){
      if(list->next!=NULL) ((list_ptr_t)list->next)->front = NULL;
      list=list->next;
    }
    else{
    if(target->front!=NULL) ((list_ptr_t)target->front)->next = target->next;
    if(target->next!=NULL) ((list_ptr_t)target->next)->front = target->front;
    }
  }
  return list;
}


list_ptr_t list_get_reference_at_index( list_ptr_t list, int index ){
  list_ptr_t target=list;
  int i=0;
  while((i<index)&&(target->next != NULL)){
    target=target->next;
    i++;
  }
  return target;
}

data_ptr_t list_get_data_at_index( list_ptr_t list, int index ){
  return list_get_reference_at_index( list, index )->data;
}

int list_get_index_of_data( list_ptr_t list, data_ptr_t data ){
  char found=0;
  int i=0;
  list_ptr_t target=list;
  if(target->data == data) found = 1;
  while((target->next != NULL)&&(found ==0)){
    target=target->next;
    i++;
    if(target->data == data) found = 1;
  }
  if(found == 0) return -1;
  else return i;
}
