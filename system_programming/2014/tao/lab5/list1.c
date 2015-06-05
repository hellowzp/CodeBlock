#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
#include<assert.h>
#include"list.h"

//define List 

int list_erro;
typedef long int data_t;
typedef data_t* data_ptr_t;

typedef struct klist
{
	struct  klist *next;
	struct  klist *previous;
            data_t data;
}list_t,

typedef list_t* list_ptr_t;

typedef struct node
{
	int size;
	struct node *head;
	struct node *tail;	
	//compareFunc *compare;
}Tnode, *Tlist;

// Returns a pointer to a newly-allocated list.
list_ptr_t list_alloc ( void )
{ 
    list_ptr_t newpointer; 
    head=(list_ptr_t)malloc(sizeof(list_t));
    if(head==NULL)
    list_erro=1;                                                                //ERROR_MEM
    newpointer->next=NULL;
    return newpointer;
	
}


// Every element of 'list' needs to be deleted (free memory) and finally the list itself needs to be deleted (free all memory)
void list_free( list_ptr_t list )
{
	list_t* temp,totemp;
	                                                                            //make temp point to list
	temp=list;
	while(list->next!=NULL)
	{
         totemp=temp;
         temp=temp->next;
		 free(totemp);
    }
    
    free(list);
  	
}


// Function:Returns the number of elements in 'list'.
int list_size( list_ptr_t list )
{
	int i=0;
	list_t* temp;
	temp=(list_ptr_t) malloc(sizeof(list_t));
	temp=list;
	while(list->next!=NULL)
	{
		 temp=temp->next;
		 i++;   
    }
    i=i+1;
    return i;
}

/* Inserts a new element containing 'data' in 'list' at position 'index'  and returns a pointer to the new list. 
If 'index' is 0 or negative, the element is inserted at the start of 'list'. 
If 'index' is bigger than the number of elements in 'list', the element is inserted at the end of 'list'.*/
list_ptr_t list_insert_at_index( list_ptr_t list, data_ptr_t data, int index)
{	 
	 list_ptr_t temp;
	 list_ptr_t sectemp,thrtemp;
	 list_erro=0;
	 temp=(list_ptr_t)malloc(sizeof(list_t));
     if(temp==NULL)
          list_erro=1;                                                          //  ERROR_MEMORY
	 temp->data=*data;
	 if(list->data==0)                                                          //  if the list has been initialized as the first list
     {                                                                          //  if not
        temp->next=NULL;
        temp->previous=NULL;
        return temp;	
     } 	  
	
	 else
	 {                                                                          //if so then 
	 
	   if(index<=0)                                                             //index<=0 then insert at first place
	   {		 
	 	 list->previous=temp;
	 	 temp->next=list;
	 	 temp->previous=NULL;  
	 	 return temp;
	   }
	    
       if(index>list_size(list))                                                //index>whole size of list
 	   {
	     sectemp=list;		
		 while(sectemp!=NULL)                                                   //find the end position of list
		 	{
		       thrtemp=sectemp;
		       sectemp=sectemp->next;		   
            }
	   	thrtemp->next=temp;
	   	temp->previous=thrtemp; 	 		 
	   	temp->next=NULL;
	   	return  list;
       }
	  
       else                                                                     //insert at middle position 
       {
	
		  int i=1;
		  sectemp=list;
		  thrtemp=list->next;  	
          while((i+1)<index)
		  {			
		     sectemp=sectemp->next;
		     thrtemp=thrtemp->next;  	
			 i=i+1;
		  }        			
		  temp=sectemp->next;
		  temp->previous=sectemp;
          temp->next=thrtemp;
          thrtemp->previous=temp;           	 
	      
		  return list;
      }         
}



// Inserts a new element containing 'data' in the 'list' at position 'reference' 
// and returns a pointer to the new list. If 'reference' is NULL, the element is inserted at the end of 'list'.
list_ptr_t list_insert_at_reference( list_ptr_t list, data_ptr_t data, list_ptr_t reference )
{	
	list_ptr_t temp;
	list_ptr_t ptr_0;
	temp=(list_ptr_t)malloc(sizeof(list_t));
    if(temp==NULL)
       list_erro=1;                                                             //EEROR_MEM 
	temp->data=*data;
	ptr_0=list;
	
	
	if(reference!=NULL)
	{
		while(ptr_0->data!=reference->data)                                     //insert by ptr_0 connection
	    {      
	        ptr_0=ptr_0->next;	
   	    }    	    
      temp->next=ptr_0->next;
      ptr_0->next->previous=temp;                                               // connect them like triangle
      ptr_0->next=temp;
      temp->previous=ptr_0;
      
         return list;
	}
	else                                           
	{    
	    while(ptr_0->next!=NULL)
	   	{
		    ptr_0=ptr_0->next;				
	    }
		ptr_0->next=temp;
		temp->previous=ptr_0;
		temp->next=NULL;
			   
	    return list;
	}

}
	
// Inserts a new element containing 'data' in the sorted 'list' and returns a pointer to the new list.
// The 'list' must be sorted before calling this function.
list_ptr_t list_insert_sorted( list_ptr_t list, data_ptr_t data )
{
	list_ptr_t temp,ptr_0,ptr_1;                                                //ptr_0 is a pointer for point next
	data_t Tdata;                                                               //ptr_1 is a pointer for point the inserted element
	ptr_1=(list_ptr_t)malloc(sizeof(list_t));
    if(ptr_1==NULL)
          list_erro=1;                                                          //ERROR_MEM
   	ptr_1->data=*data;
	
	for(temp=list;temp->next!=NULL;temp=temp->next)                             //sort the list
	{     
	                    
	   for(ptr_0=temp->next;ptr_0!=NULL;ptr_0=ptr_0->next)
	   	{
		   
		   if((temp->data)>(ptr_0->data))
		   	{
			   Tdata=temp->data;
			   temp->data=ptr_0->data;
			   ptr_0->data=Tdata;		   
		    }		 
	    }	   
    }

    temp=list;
    ptr_0=temp->next;
    
    if(list_size(list)==0)
    {
	    ptr_1->next=NULL;
	    ptr_1->prev=NULL;
	    return tempInsert;	
	}
    else if(list_size(list)==1)
	{
		
		if(ptr_1->data>ptr_0->data)
		{
	    	list->next=ptr_1;
	    	ptr_1->prev=list;
		    ptr_1->next=NULL;				
		    return list;
		}
		else
		{
		ptr_1=list->previous;
		list=ptr_1->next;
		ptr_1->previous=NULL;
		return ptr_1;				
	    }
	}
	else
	{
	   while(ptr_1->data>temp->next->data)
	   {
   		   	 temp=temp->next;
	   }
	   ptr_1->next=temp->next;
	   temp->next->previous=ptr_1;
	   ptr_1->previous=temp;
	   temp->next=ptr_1;
       return list;
    }
}




/* Deletes the element at index 'index' in 'list'. 
A free() is called on the data pointer of the element to free any dynamic memory allocated to the data pointer. 
If 'index' is 0 or negative, the first element is deleted. 
If 'index' is bigger than the number of elements in 'list', the data of the last element is deleted.*/
list_ptr_t list_free_at_index( list_ptr_t list, int index)
{
	list_ptr_t temp,ptr_1;	
	int i;
	if(index<=0)
	{
	  temp=list->next;
	  temp->previous=NULL;
	  free(list);
	 // list_erro=3;
	  return temp; 
    }
    else if(index>list_size(list))                                              //delete the last element
	{
       temp=list;
       while(temp->next->next!=NULL)
       {
       	 temp=temp->next;
       }
        ptr_1=temp->next;
        temp->next=NULL;
        free(ptr_1);
       // list_erro=4;
       
       return list;
    }
    else
	{
	  if(list_size(list)==1)
  	  {
	   //  list_erro=8;
	     free(list);
	     return NULL;	
	  }
	  else
  	  {
		temp=list;
		if(index==1)
		{
		  free(list);
		  return list;	
		}
		else
		{
	      for(i=2;i<index;i++)
             temp=temp->next;
         
          ptr_1=temp->next;     
	      free(ptr_1);
	      return list;
	    }
      }
    }
}

/*Deletes the element with position 'reference' in 'list'. 
A free() is called on the data pointer of the element to free any dynamic memory allocated to the data pointer. 
If 'reference' is NULL, the data of the last element is deleted.*/
list_ptr_t list_free_at_reference( list_ptr_t list, list_ptr_t reference )
{	
	list_ptr_t temp;
	temp=list;
    int i=0;
    
   	if(reference!=NULL)
	{
		while(temp->data!=reference->data)
	  {
          temp=temp->next;
          i++;
      }
      i=i+1;
      list=list_free_at_index(list,i);
	  return list;
	 
    }
	else
	{
      list=list_free_at_index(list,list_size(list));
	  //list_erro=5;
	  return list;
    }
}

/*Finds the first element in 'list' that contains 'data' and deletes the element from 'list'.
 A free() is called on the data pointer of the element to free any dynamic memory allocated to the data pointer.*/
list_ptr_t list_free_data( list_ptr_t list, data_ptr_t data )
{	
	list_ptr_t temp;
	temp=list;

    while(temp->data!=*data&&temp!=NULL)
    {
   	    temp=temp->next;
    }
	
	   
    if(temp==NULL)
   	{
	  //  list_erro=6;
        return NULL;
    }
  	else
    {
	   ptr_1=temp;
       temp=NULL;
       free(ptr_1); 
       return list;
   	}
	
}

/* Removes the element at index 'index' from 'list'. NO free() is called on the data pointer of the element. 
If 'index' is 0 or negative, the first element is removed. 
If 'index' is bigger than the number of elements in 'list', the data of the last element is removed.*/
list_ptr_t list_remove_at_index( list_ptr_t list, int index)
{	
	list_ptr_t temp,ptr_1,ptr_2;
	int i;
	if(index<=0)
	{
	  temp=list->next;
	  list=NULL;
	  //list_erro=3;
	  return temp; 
    }
    else if(index>list_size(list))
	{
       temp=list;
       while(temp->next->next!=NULL)
       {
       	 temp=temp->next;
       }   
       temp->next=NULL;
       //list_erro=4;
       return list;
    }
    else
	{
	  if(list_size(list)==1)
	  {	     
	     return NULL;	
	  }
	  else
  	  {
		  if(index==1)
	  	  {
	  	  	ptr_2=list->next;
	  	  	free(list);
	  	  	return ptr_2;
		  }
		  else
	  	  {
		      temp=list;
	          for(i=2;i<index;i++)
                 temp=temp->next;
              
              ptr_1=temp->next;
	          temp->next=ptr_1->next;	 
	          ptr_1->next->prev=temp;        
	          free(ptr_1);
	          return list;
	      }
      }
    }
}

/* Removes the element with reference 'reference' in 'list'. 
NO free() is called on the data pointer of the element. If 'reference' is NULL, the data of the last element is removed.*/
list_ptr_t list_remove_at_reference( list_ptr_t list, list_ptr_t reference )
{	
	list_ptr_t temp;
	temp=list;
    int i=0;
   	if(reference==NULL)
	{
	  list=list_remove_at_index(list,list_size(list));
	 // list_erro=5;
	  return list;
    }
	else
	{
      while((temp->data!=reference->data)&&(temp!=NULL))
	  {
          temp=temp->next;
          i++;
      }
      i=i+1;
      if(temp==NULL)
      {	
	    //list_erro=7;
	    return list;  		  
	  }
      else
	    {
        list=list_remove_at_index(list,i);
	    return list;
        }
    }
}

// Finds the first element in 'list' that contains 'data' and removes the element from 'list'. NO free() is called on the data pointer of the element.

list_ptr_t list_remove_data( list_ptr_t list, data_ptr_t data )
{
	list_ptr_t temp;
	temp = list->head;
	int i;
	for(i=1;i<=list->size;i++)
	{
		if(data == temp->data)
		{
			temp->prev->next = temp->next;
			temp->next->prev = temp->prev;
			i=list->size+1;
		}
		temp = temp->next;
	}
		
	list->size--;
	return list;
}


// Returns a reference to the first element of 'list'. If the list is empty, NULL is returned.
list_ptr_t list_get_first_reference( list_ptr_t list )
{
	Tlist reference;
	reference = (Tlist)malloc(sizeof(Tnode));
	reference->size=0;
	reference->head=NULL;
	reference->tail=NULL;
	
	if(list->size == 0)
	{
		return NULL;
	}
	else
	{
		reference->size = 1;
		reference->head = list->head;
		reference->tail = list->head;
	}
	return reference;
}
// Returns a reference to the last element of 'list'. If the list is empty, NULL is returned.
list_ptr_t list_get_last_reference( list_ptr_t list )
{
    Tlist reference;
	reference = (Tlist)malloc(sizeof(Tnode));
	reference->size=0;
	reference->head=NULL;
	reference->tail=NULL;
	
	if(list->size == 0)
	{
		return NULL;
	}
	else
	{
		reference->size = 1;
		reference->head = list->tail;
		reference->tail = list->tail;
	}
	return reference;
}

/* Returns a reference to the next element of the element with reference 'reference' in 'list'. 
If the next element doesn't exists, NULL is returned.*/
list_ptr_t list_get_next_reference( list_ptr_t list, list_ptr_t reference )
{	
    list_ptr_t temp;
	temp=list;
	list_erro=0;
	while((temp->data!=reference->data)&&(temp!=NULL))
	{
		temp=temp->next;
	}
	   	
	if(temp!=NULL) 
	{
	  return temp->next;
	}
	else
	{	  
    // list_erro=7;   
	   return NULL;
    }
}


// Returns a reference to the previous element of the element with reference 'reference' in 'list'. If the previous element doesn't exists, NULL is returned.
list_ptr_t list_get_previous_reference( list_ptr_t list, list_ptr_t reference ){	
    list_ptr_t temp;
	temp=list;
	list_erro=0;
	while((temp->next->data!=reference->data)&&(temp->next!=NULL))
	   temp=temp->next;	
	if(temp->next==NULL) {
	   list_erro=7;
	   return NULL;
	}
	else
	   return temp;
}

// Returns the index of the element in the 'list' with reference 'reference'. If 'reference' is NULL, the index of the last element is returned.
int list_get_index_of_reference( list_ptr_t list, list_ptr_t reference ){
    list_ptr_t temp;
    int i=1;
    temp=list;
    list_erro=0;
    if(list==NULL){
        list_erro=8;                       //empty list	
        return 0;
    }
    else{
	   if(reference==NULL){		
		  while(temp->next!=NULL){
		    temp=temp->next;
		    i++;
	      }
	      list_erro=5;
	      return i;
	    }
	   else{
	      while((temp->data!=reference->data)&&(temp!=NULL)){
	        temp=temp->next;		
            i++;
          }
          if(temp==NULL){
             list_erro=7;
             return -1;
		  }
          else
             return i;
    }
 }
}

// Returns a reference to the element with index 'index' in 'list'. If 'index' is 0 or negative, a reference to the first element is returned. If 'index' is bigger than the number of elements in 'list', a reference to the last element is returned. If the list is empty, NULL is returned.
list_ptr_t list_get_reference_at_index( list_ptr_t list, int index ){	
	list_ptr_t temp;
	temp=list;
	int i;
	list_erro=0;
	if(index<=0){
	    list_erro=3;
	    return list;	
	}
	else if(index>list_size(list)){
		while(temp->next!=NULL)
		    temp=temp->next;
		list_erro=4;
		return temp;	
	}
	else{
	    for(i=1;i<index;i++)
             temp=temp->next;
	    return temp;
	}	
}

// Returns the data pointer contained in the element with reference 'reference' in 'list'. If 'reference' is NULL, the data of the last element is returned.
data_ptr_t list_get_data_at_reference( list_ptr_t list, list_ptr_t reference ){	
    list_ptr_t temp;
	temp=list;  	
	list_erro=0;
	if(reference==NULL){
	   	while(temp->next!=NULL)
		    temp=temp->next;
		list_erro=5;
		return &(temp->data);
	}
	else{
		while((temp->data!=reference->data)&&(temp!=NULL))
		     temp=temp->next;
		if(temp==NULL){
		     list_erro=7;
		     return NULL;
		}
		else
		     return &(temp->data);
	}	
}


// Returns the data contained in the element with index 'index' in 'list'. If 'index' is 0 or negative, the data of the first element is returned. If 'index' is bigger than the number of elements in 'list', the data of the last element is returned.
data_ptr_t list_get_data_at_index( list_ptr_t list, int index ){	
	list_ptr_t temp;
	temp=list;
	int i;    	
	list_erro=0;
	if(index<=0){
	   list_erro=3;	
	   return &(list->data);
	}
	else if(index>list_size(list)){
	   while(temp->next!=NULL)
		    temp=temp->next;
	   list_erro=4;
	   return &(temp->data);	
    }
    else{
	   for(i=1;i<index;i++)
		    temp=temp->next;
		return &(temp->data);	
	}
}


// Returns an index to the first element in 'list' containing 'data'.  If 'data' is not found in 'list', -1 is returned.
int list_get_index_of_data( list_ptr_t list, data_ptr_t data ){
    list_ptr_t temp;
	temp=list;
	int i=0; 
	list_erro=0;
	while((temp->data!=*data)&(temp!=NULL)){
	   temp=temp->next;
       i++;    
    }
	if(temp==NULL){
	   list_erro=6;
	   return -1;
    }
	else
	   return i;   	   
}


// Returns a reference to the first element in 'list' containing 'data'. If 'data' is not found in 'list', NULL is returned.
list_ptr_t list_get_reference_of_data( list_ptr_t list, data_ptr_t data ){
	list_ptr_t temp;
	temp=list;
	int i=0; 
	list_erro=0;
	while((temp->data!=*data)&(temp!=NULL)){
	   temp=temp->next;
       i++;    
    }
	if(temp==NULL){
	   list_erro=6;
	   return NULL;
    }
	else
	   return temp;  	
	   	
}