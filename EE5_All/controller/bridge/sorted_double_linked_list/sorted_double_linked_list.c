#include "sorted_double_linked_list.h"


#define size_list_t sizeof(list_t)
#define size_data_element sizeof(data_element)


list_ptr_t list_init () {
   err_num = NORMAL;
   list_ptr_t temp = (list_ptr_t)malloc(size_list_t);
        if(temp == NULL){
          err_num = MALLOC_ERR;
          ErrHandler();}
   temp->previousElement = NULL;
   temp->nextElement = NULL;
   temp->data = NULL;
   return temp;
}

void list_free_all (list_ptr_t* list) {

   err_num = NORMAL;

   list_ptr_t temp = *list;
   list_ptr_t temp1;

   if(*list == NULL){
        err_num = LIST_NOT_EXIST;
        ErrHandler();
        return;}

   if(temp->data == NULL){
     free(temp);
     *list = NULL;
     return;}

   while(temp->nextElement != NULL){
        temp1 = temp;
        temp = temp->nextElement;
        if( (temp1->data->message) != NULL){
            free(temp1->data->message);
        }
        free(temp1->data);
        free(temp1);}
   if( (temp->data->message) != NULL){
       free(temp->data->message);
   }
   free(temp->data);
   free(temp);
   *list = NULL;
}


int list_size(list_ptr_t list){

    err_num = NORMAL;
    list_ptr_t temp = list;

    int size = 1;
    if(temp == NULL){
        err_num = LIST_NOT_EXIST;
        ErrHandler();
        return -1;}
    if(list->data == NULL){return 0;}

    while((temp->nextElement) != NULL){
          temp = temp->nextElement;
          size++;}

    return size;
}

int list_get_by_address(list_ptr_t list, DADDRESS address, list_ptr_t *return_value){

    err_num = NORMAL;
    list_ptr_t temp = list;

    if(temp == NULL){
        err_num = LIST_NOT_EXIST;
        ErrHandler();
        return 0;}
    if(list->data == NULL){
        *return_value = NULL;
        return 1;}

    do{
        if(temp->data->address==address){
            *return_value = temp;
            return 0;}
        temp = temp->nextElement;
      }while(temp!=NULL);

    *return_value = NULL;
    return 0;
}

list_ptr_t list_put(list_ptr_t *list, DADDRESS address, time_t timeout, TIME_PROTOCOL duration, MESSAGE *message){

    err_num = NORMAL;

    if(message == NULL){
    list_ptr_t element_exist;
    list_get_by_address(*list, address,&element_exist);
    if(element_exist != NULL){
         element_exist->data->duration = duration;
         list_update(list,address,time(NULL)+duration);
         return *list;
       }
    }

    data_ptr_t element = (data_ptr_t)malloc(size_data_element);
    if(element == NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return NULL;
    }

    element->address = address;
    element->duration = duration;
    element->timeout = timeout;
    element->message = message;

    list_ptr_t newElement = (list_ptr_t)malloc(size_list_t);
    if(newElement == NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return NULL;
    }
    newElement->data = element;

    list_ptr_t element_bigger_or_equal = list_get_element_with_bigger_value(*list,timeout);

    if(element_bigger_or_equal == NULL){
        if((*list)->data == NULL){
            newElement->previousElement = NULL;
            newElement->nextElement = NULL;
            free(*list);
            *list = newElement;
            return newElement;
        }
    list_ptr_t lastElement = list_get_last_element(*list);

    newElement->previousElement = lastElement;
    newElement->nextElement = NULL;
    lastElement->nextElement = newElement;
    }

    else if(element_bigger_or_equal->previousElement == NULL){
        newElement->previousElement = NULL;
        newElement->nextElement = element_bigger_or_equal;
        *list = newElement;
        element_bigger_or_equal->previousElement = newElement;
    }

    else{
        list_ptr_t previousE = element_bigger_or_equal->previousElement;
        previousE->nextElement = newElement;
        element_bigger_or_equal->previousElement = newElement;
        newElement->previousElement = previousE;
        newElement->nextElement = element_bigger_or_equal;
    }
    return *list;
}



list_ptr_t list_get_last_element(list_ptr_t list){
    err_num = NORMAL;
    list_ptr_t temp = list;

    if(temp == NULL){
        err_num = LIST_NOT_EXIST;
        ErrHandler();
        return NULL;}
    if(list->data == NULL){
        err_num = LIST_EMPTY;
         ErrHandler();
        return NULL;}
    while(temp->nextElement != NULL){
        temp = temp->nextElement;
    }
    return temp;
}


list_ptr_t list_get_element_with_bigger_value(list_ptr_t list, time_t value){


    err_num = NORMAL;

    list_ptr_t temp = list;

    if(temp == NULL){
        err_num = LIST_NOT_EXIST;
        ErrHandler();
        return NULL;}
    if(list->data == NULL){return NULL;}

    do{
        if((temp->data->timeout)>=value){return temp;}
        temp = temp->nextElement;
      }while(temp!=NULL);

    return NULL;
}

list_ptr_t list_update(list_ptr_t *list, DADDRESS address, time_t value){


    list_ptr_t element_equal_or_bigger = list_get_element_with_bigger_value(*list, value);
            list_ptr_t element_with_address;

            list_get_by_address(*list, address,&element_with_address);
            element_with_address->data->timeout = value;

            if(element_with_address == NULL){
               err_num = DEVICE_NOTCONNECT;
               ErrHandler();
               return NULL;
            }



    if(element_equal_or_bigger == NULL){

        if(element_with_address->nextElement==NULL){return *list;}
        else if(element_with_address->previousElement==NULL){
            list_ptr_t newFirst = element_with_address->nextElement;
            newFirst->previousElement = NULL;
            element_with_address->nextElement = NULL;
            list_ptr_t oldLast = list_get_last_element(*list);
            oldLast->nextElement = element_with_address;
            element_with_address->previousElement = oldLast;
            *list= newFirst;

        }

        else{
            list_ptr_t oldPrevious = element_with_address->previousElement;
            list_ptr_t oldNext = element_with_address->nextElement;
            list_ptr_t oldLast = list_get_last_element(*list);
            oldPrevious->nextElement = oldNext;
            oldNext->previousElement = oldPrevious;
            oldLast->nextElement = element_with_address;
            element_with_address->previousElement = oldLast;
            element_with_address->nextElement = NULL;

        }
    }
    else{

         if((element_equal_or_bigger->previousElement->data->address) == address){return *list;}
        if(element_with_address->previousElement == NULL){

           list_ptr_t newFirst = element_with_address->nextElement;
           newFirst->previousElement = NULL;
           *list = newFirst;
           list_ptr_t newPrevious = element_equal_or_bigger->previousElement;
           newPrevious->nextElement = element_with_address;
           element_equal_or_bigger->previousElement = element_with_address;
           element_with_address->previousElement = newPrevious;
           element_with_address->nextElement = element_equal_or_bigger;

        }
        else{

            if((element_equal_or_bigger->data->address) == (element_with_address->data->address)){}

            else{
            list_ptr_t oldPrevious = (element_with_address->previousElement);
            list_ptr_t oldNext = element_with_address->nextElement;
            list_ptr_t newPrevious = element_equal_or_bigger->previousElement;
            oldPrevious->nextElement = oldNext;
            oldNext->previousElement = oldPrevious;
            newPrevious->nextElement = element_with_address;
            element_equal_or_bigger->previousElement = element_with_address;
            element_with_address->previousElement = newPrevious;
            element_with_address->nextElement = element_equal_or_bigger;
            }
        }
    }

    return *list;
}

list_ptr_t list_delete_by_address(list_ptr_t *list, DADDRESS address){
    list_ptr_t item;
    list_get_by_address(*list,address,&item);
    if(item == NULL){
        err_num = DEVICE_NOTCONNECT;
        ErrHandler();
        return NULL;
    }
    if((item->previousElement == NULL)&&(item->nextElement == NULL)){
        if((item->data->message) != NULL){
            free(item->data->message);
        }
        free(item->data);
    }
    else if(item->previousElement == NULL){
        *list = item->nextElement;
        (*list)->previousElement = NULL;
        if((item->data->message) != NULL){
            free(item->data->message);
        }
        free(item->data);
        free(item);
    }
    else if(item->nextElement == NULL){
        list_ptr_t temp = item->previousElement;
        temp->nextElement = NULL;
        if((item->data->message) != NULL){
            free(item->data->message);
        }
        free(item->data);
        free(item);
    }
    else{
        list_ptr_t oldPrevious = item->previousElement;
        list_ptr_t oldNext = item->nextElement;
        oldPrevious->nextElement = oldNext;
        oldNext->previousElement = oldPrevious;
        if((item->data->message) != NULL){
            free(item->data->message);
        }
        free(item->data);
        free(item);
    }
    return *list;
}

list_ptr_t list_find_first_output_item(list_ptr_t list){
    if(list == NULL){
        err_num = LIST_NOT_EXIST;
        ErrHandler();
        return NULL;
    }
    if((list->data) == NULL){
        err_num = LIST_EMPTY;
        ErrHandler();
        return NULL;
    }
    list_ptr_t temp = list;
    while((temp->nextElement) != NULL){
        if(temp->data->message != NULL){
            return temp;
        }
        temp = temp->nextElement;
    }
    return NULL;

}

void print_list(list_ptr_t list){
    if(list == NULL){
        err_num = LIST_NOT_EXIST;
        ErrHandler();
        return;
    }
    if((list->data) == NULL){
        err_num = LIST_EMPTY;
        ErrHandler();
        return;
    }
    list_ptr_t temp = list;
    data_ptr_t dataE;
    printf("the size of the list: %d \n",list_size(list));
    do{
        dataE = temp->data;
        printf("device address: %d ; durantion: %d ; the absolute timeout value: %lu .\n",(int)(dataE->address),(int)(dataE->duration),dataE->timeout);
        temp = temp->nextElement;
    }while(temp!=NULL);
}
