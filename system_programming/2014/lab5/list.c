#include <stdio.h>
#include <stdlib.h>
#include "list.h"

#define FREE(p) do{ free((mydata_ptr)(p->data)); (p)->data = NULL;\
	                free(p); p = NULL;\
	               } while(0)

#ifndef MY_DATA_TYPE      //define the real data type with compile option
#define MY_DATA_TYPE int  //-DMY_DATA_TYPE=int
#endif
typedef MY_DATA_TYPE* mydata_ptr;  

struct list_item {
	data_ptr_t data;
	list_ptr_t prev, next;
};

int list_errno;

list_ptr_t list_alloc () {
	list_ptr_t lp = NULL;
	lp = malloc(sizeof(list_t));
	if(lp) {
		lp->data = NULL;
		lp->prev = NULL;
		lp->next = NULL;
		list_errno = 0;
	} else {
		printf("Allocate list memory failed\n");
		list_errno = -1;
		exit(1);
	}
	return lp;
}

void list_free_all( list_ptr_t* list ) {
	list_ptr_t lptr = *list;
	while(lptr) {
		list_ptr_t next = lptr->next;
		FREE(lptr);
		lptr = next;
	}
	list_errno = 0;
}

int list_size( list_ptr_t list ){
	if(!list->prev && !list->next && !list->data)  return 0; //if it's just initialized, a dummy list
	int size = 0;	
	while(list) {
		size++;
		list = list->next;
	}
	list_errno = 0;
	return size;
}

list_ptr_t list_insert_at_index( list_ptr_t list, data_ptr_t data, int index) {
	list_errno = -1;
	if(!list) return NULL;
	
	list_errno = 0;
	int size = list_size(list);
	if(size == 0) {            //dummy list
		list->data = data;
		return list;
	}
	
	list_ptr_t lptrToInsert = malloc(sizeof(list_t));
	lptrToInsert->data = data;
	//lptrToInsert->data = malloc(sizeof(*(mydata_ptr)data));  //allocate memory to save data in the heap
	//*(mydata_ptr)(lptrToInsert->data) = *(mydata_ptr)data;   //don't just use lptrToInsert->data = data!!
	if(size==1) {
		if(index<=1) {
			lptrToInsert->prev = NULL;
			lptrToInsert->next = list;
			return lptrToInsert;
		} else {
			lptrToInsert->prev = list;
			list->next = lptrToInsert;
			lptrToInsert->next = NULL;
			return list;
		}
	} 
	
	if(size>=2) {  //size>=2
		if(index<=1) {   //inserted at beginning
			lptrToInsert->prev = NULL;
			lptrToInsert->next = list;
			return lptrToInsert;
		} else if(index<=size){
			int i = 1;   //inserted at middle
			list_ptr_t lptr = list;
			for(i=1;i<index;i++) {
				lptr = lptr->next;
			}
			lptr->prev->next = lptrToInsert;
			lptrToInsert->prev = lptr->prev;
			lptrToInsert->next = lptr;
			lptr->prev = lptrToInsert;
			return list;
		} else {        //inserted at the end
			int i = 1;
			list_ptr_t lptr = list;
			for(i=1;i<size;i++) {
				lptr = lptr->next;
			}
			lptr->next = lptrToInsert;
			lptrToInsert->prev = lptr;
			lptrToInsert->next = NULL;
			return list;
		}
	}
	/*
	list_ptr_t lptrToInsert = malloc(sizeof(list_t));
	lptrToInsert->data = data;
	lptrToInsert->data = malloc(sizeof(*(mydata_ptr)data));  //allocate memory to save data in the heap
	*(mydata_ptr)(lptrToInsert->data) = *(mydata_ptr)data;   //don't just use lptrToInsert->data = data!!
	
	//better to first draw different situations in draft
	int size = list_size(list);
	if(size == 0) {
		list_errno = 0;
		lptrToInsert->prev = NULL;
		lptrToInsert->next = NULL;
		return lptrToInsert;
	} else if(size==1) {
		if(index<=1) {
			lptrToInsert->prev = NULL;
			lptrToInsert->next = list;
			list_errno = 0;
			return lptrToInsert;
		} else {
			lptrToInsert->prev = list;
			list->next = lptrToInsert;
			lptrToInsert->next = NULL;
			list_errno = 0;
			return list;
		}
	} else {  //size>=2
		if(index<=1) {   //inserted at beginning
			lptrToInsert->prev = NULL;
			lptrToInsert->next = list;
			list_errno = 0;    
			return lptrToInsert;
		} else if(index<=size){
			int i = 1;   //inserted at middle
			list_ptr_t lptr = list;
			for(i=1;i<index;i++) {
				lptr = lptr->next;
			}
			lptr->prev->next = lptrToInsert;
			lptrToInsert->prev = lptr->prev;
			lptrToInsert->next = lptr;
			lptr->prev = lptrToInsert;
			list_errno = 0;
			return list;
		} else {        //inserted at the end
			int i = 1;
			list_ptr_t lptr = list;
			for(i=1;i<size;i++) {
				lptr = lptr->next;
			}
			lptr->next = lptrToInsert;
			lptrToInsert->prev = lptr;
			lptrToInsert->next = NULL;
			list_errno = 0;
			return list;
		}
	}*/
}

list_ptr_t list_remove_at_index( list_ptr_t list, int index) {
	if(!list) {
		list_errno = -1;
		return NULL;
	}
	
	int i = 0;
	list_ptr_t it = list;
	//get the list_pointer at the index
	while(it) {
		i++;
		if(i==index) break;
		it = it->next;
	}
	
	//discuss and remove 
	list_errno = 0;
	if(it->prev && it->next) {
		it->prev->next = it->next;
		it->next->prev = it->prev;
		return list;
	} else if(it->prev && !it->next) {
		it->prev->next = NULL;
		return list;
	} else if(!it->prev && it->next) {
		it->next->prev = NULL;
		return list->next;
	} else {     //only one item exists in the whole list
		//list = NULL;  //still reserve the pointer to that item in order to free it later
		return NULL;
	}
}

list_ptr_t list_free_at_index( list_ptr_t list, int index) {
	if(!list) {
		list_errno = -1;
		return NULL;
	}
	
	int i = 0;
	list_ptr_t it = list;
	//get the list_pointer at the index
	while(it) {
		i++;
		if(i==index) break;
		it = it->next;
	}
	
	//discuss and free 
	list_errno = 0;
	if(it->prev && it->next) {
		it->prev->next = it->next;
		it->next->prev = it->prev;
		FREE(it);
		return list;
	} else if(it->prev && !it->next) {
		it->prev->next = NULL;
		FREE(it);
		return list;
	} else if(!it->prev && it->next) {
		it->next->prev = NULL;
		//FREE(it);
		free((mydata_ptr)(it->data)); 
		(it)->data = ((void *)0); 
		free(it); 
		it = ((void *)0);
		return list->next;
	} else {     //only one item exists in the whole list
		FREE(it);
		return NULL;
	}
}

list_ptr_t list_get_reference_at_index( list_ptr_t list, int index ) {
	int i = 0;
	while(list) {
		i++;
		if(i==index) break;
		list = list->next;
	}
	if(list) list_errno = 0;
	else list_errno = -1;
	return list;
}

data_ptr_t list_get_data_at_index( list_ptr_t list, int index ) {
	if(!list) return NULL;
	
	list_ptr_t lptr = list;
	int min, i = 1;
	if(index<=list_size(list))
		min = index;
	else
		min = list_size(list);
	while(i < min) {
		lptr = lptr->next;
		i++;
	}
	list_errno = 0;
	return lptr->data;
}

int list_get_index_of_data( list_ptr_t list, data_ptr_t data ) {
	int index = 0;
	while(list) {
		index++;
		//first check null of pointer each time before dereferencing
		//if(!list->data || !data)  break; shouldn't do like this, continue checking remaining data instead!!		
		if( list->data && data && *(mydata_ptr)(list->data) == *(mydata_ptr)data)  return index;
		list = list->next;
	}
	return -1;
}

 int main() {
	list_ptr_t list = NULL;
	list = list_alloc();
	int *a = NULL, *b = NULL;
	a = malloc(sizeof(int));
	b = malloc(sizeof(int));
	*a = 2, *b =3;   //change to diffrent type from MY_DATA_TYPE, see how data will be converted
	list = list_insert_at_index(list,a,2);
	printf("%d %p\n",list_size(list),list);
	list = list_insert_at_index(list,b,1);
	printf("%d %p\n",list_size(list),list);
	printf("%d %d\n",*(mydata_ptr)(list_get_data_at_index(list,1)),*(mydata_ptr)(list_get_data_at_index(list,2)));
	
	printf("%d\n",list_get_index_of_data(list,a));
	list = list_free_at_index(list,1);
	printf("%d %p\n",list_size(list),list);
	printf("%d\n",list_get_index_of_data(list,a));
	printf("%d %d\n",*(mydata_ptr)(list_get_data_at_index(list,1)),*(mydata_ptr)(list_get_data_at_index(list,2)));
	list_free_all(&list);
	return 0;
}


