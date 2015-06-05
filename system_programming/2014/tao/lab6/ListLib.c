#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
int list_erro;

typedef long int data_t;
typedef data_t* data_ptr_t;

typedef struct myelement{
	data_t data;
	struct myelement *next;
	struct myelement *prev;	
	}myElement;

typedef myElement* list_ptr_t;

// Returns a pointer to a newly-allocated list.
list_ptr_t list_alloc( void ){
	list_erro=0;
	list_ptr_t myList;
	myList=(list_ptr_t)malloc(sizeof(myElement));
	if(myList==NULL)
	     list_erro=1;	//memory problem;  
	return myList;	
 }
  
// Every element of 'list' needs to be deleted (free memory) and finally the list itself needs to be deleted (free all memory)  
void list_free( list_ptr_t list ){	
	list_ptr_t temp,temp1;
	temp=list;
	int i;
	list_erro=0;
	for(i=0;i<list_size(list);i++){
     temp1=temp;	
	 temp=temp->next;	  
	 free(temp1);	
    }
 }

// Returns the number of elements in 'list'.
int list_size( list_ptr_t list ){
    list_ptr_t temp;
    int i=0;
    list_erro=0;
    if(list->data==0){
       list_erro=8;     //empty list
       return 0;
    }
    temp=list;
    while(temp!=NULL){
		temp=temp->next;
		i++;
	}
    return i;
}
 
 // Inserts a new element containing 'data' in 'list' at position 'index'  and returns a pointer to the new list. If 'index' is 0 or negative, the element is inserted at the start of 'list'. If 'index' is bigger than the number of elements in 'list', the element is inserted at the end of 'list'.
 list_ptr_t list_insert_at_index( list_ptr_t list, data_ptr_t data, int index){	 
	 list_ptr_t temp,temp1,temp2;
	 list_erro=0;
	 temp=(myElement*)malloc(sizeof(myElement));
     if(temp==NULL)
          list_erro=1;     //memory problem;  
	 temp->data=*data;
	 if(list->data==0){    //not initialized,insert first element
        temp->next=NULL;
        temp->prev=NULL;
        return temp;	
     } 	  
	
	 else{                  //have initialized
	 
	   if((index)<=0){		 
	 	 list->prev=temp;
	 	 temp->next=list;
	 	 temp->prev=NULL;  
	 	 list_erro=3;
	 	 return temp;
	   }
	 
       if(index>list_size(list)){
	     temp1=list;		
		 while(temp1!=NULL){
		   temp2=temp1;
		   temp1=temp1->next;		   
	     }
	   	temp2->next=temp;
	   	temp->prev=temp2; 	 		 
	   	temp->next=NULL;
	   	list_erro=4;
		return list;
	  }
	  
       else
       {
		if(list_size(list)==1){        	   
		    list->next=temp;  
		    temp->prev=list;
		    temp->next=NULL;
		}
		else{
		  int i=1;
		  temp1=list;
		  temp2=list->next;  	
          while((i+1)<index){			
		     temp1=temp1->next;
		     temp2=temp2->next;  	
			 i++;
		  }        			
		  temp=temp1->next;
		  temp->prev=temp1;
          temp->next=temp2;
          temp2->prev=temp;           	 
	    }
	    return list;
      }         
	}
}


// Inserts a new element containing 'data' in the 'list' at position 'reference'  and returns a pointer to the new list. If 'reference' is NULL, the element is inserted at the end of 'list'.
list_ptr_t list_insert_at_reference( list_ptr_t list, data_ptr_t data, list_ptr_t reference ){	
	list_ptr_t temp,temp0,temp1;
	temp=(myElement*)malloc(sizeof(myElement));
	list_erro=0;
    if(temp==NULL)
          list_erro=1;     //memory problem;  
	temp->data=*data;
	temp0=list;
	temp1=list;
	
    if(reference==NULL){
	   	while(temp1->next!=NULL){
		    temp1=temp1->next;				
		}
		temp1->next=temp;
		temp->prev=temp1;
		temp->next=NULL;
		list_erro=5;
		return list;		
	}
	else{    
	   while(temp0->data!=reference->data){       //suppose the data originally in the list are different
	        temp0=temp0->next;	
    	}    	    
      temp->next=temp0->next;
      temp0->next->prev=temp;
      temp0->next=temp;
      temp->prev=temp0;
      
	  return list;
	}

}
	
// Inserts a new element containing 'data' in the sorted 'list' and returns a pointer to the new list. The 'list' must be sorted before calling this function.
list_ptr_t list_insert_sorted( list_ptr_t list, data_ptr_t data ){
	list_ptr_t tempCurrent,tempNext,tempInsert;
	data_t dataTemp;
	tempInsert=(myElement*)malloc(sizeof(myElement));
	list_erro=0;
    if(tempInsert==NULL)
          list_erro=1;     //memory problem;  
	tempInsert->data=*data;
	
	for(tempCurrent=list;tempCurrent->next!=NULL;tempCurrent=tempCurrent->next){     //sorted from small till big
	                    
	   for(tempNext=tempCurrent->next;tempNext!=NULL;tempNext=tempNext->next){
		   
		   if((tempCurrent->data)>(tempNext->data)){
			   dataTemp=tempCurrent->data;
			   tempCurrent->data=tempNext->data;
			   tempNext->data=dataTemp;		   
		   }		 
	   }	   
    }

    tempCurrent=list;
    tempNext=tempCurrent->next;
    
    if(list_size(list)==0){
	    tempInsert->next=NULL;
	    tempInsert->prev=NULL;
	    return tempInsert;	
	}
    else if(list_size(list)==1){
		
		if(tempInsert->data>tempCurrent->data){
	    	list->next=tempInsert;
	    	tempInsert->prev=list;
		    tempInsert->next=NULL;				
		    return list;
		}
		else{
		tempInsert=list->prev;
		list=tempInsert->next;
		tempInsert->prev=NULL;
		return tempInsert;				
	    }
	}
	else{
	   while(tempInsert->data>tempCurrent->next->data)
	       tempCurrent=tempCurrent->next;   
	   tempInsert->next=tempCurrent->next;
	   tempCurrent->next->prev=tempInsert;
	   tempInsert->prev=tempCurrent;
	   tempCurrent->next=tempInsert;
       return list;
    }
}


// Deletes the element at index 'index' in 'list'. A free() is called on the data pointer of the element to free any dynamic memory allocated to the data pointer. If 'index' is 0 or negative, the first element is deleted. If 'index' is bigger than the number of elements in 'list', the data of the last element is deleted.
list_ptr_t list_free_at_index( list_ptr_t list, int index){
	list_ptr_t temp,temp1;	
	int i;
	list_erro=0;
	if(index<=0){
	  temp=list->next;
	  temp->prev=NULL;
	  free(list);
	  list_erro=3;
	  return temp; 
    }
    else if(index>list_size(list)){
       temp=list;
       while(temp->next->next!=NULL)
         temp=temp->next;
        temp1=temp->next;
        temp->next=NULL;
        free(temp1);
        list_erro=4;
       return list;
    }
    else{
	  if(list_size(list)==1){
	     list_erro=8;
	     free(list);
	     return NULL;	
	  }
	  else{
		temp=list;
		if(index==1)
		{
		  free(list);
		  return list;	
		}
		else{
	      for(i=2;i<index;i++)
             temp=temp->next;
         
          temp1=temp->next;     
	      free(temp1);
	      return list;
	    }
      }
    }
}

// Deletes the element with position 'reference' in 'list'. A free() is called on the data pointer of the element to free any dynamic memory allocated to the data pointer. If 'reference' is NULL, the data of the last element is deleted.
list_ptr_t list_free_at_reference( list_ptr_t list, list_ptr_t reference ){	
	list_ptr_t temp;
	temp=list;
    int i=1;
    list_erro=0;
	if(reference==NULL){
	  list=list_free_at_index(list,list_size(list));
	  list_erro=5;
	  return list;
   }
	else{
      while(temp->data!=reference->data){
          temp=temp->next;
          i++;
      }
      list=list_free_at_index(list,i);
	  return list;
   }
}

// Finds the first element in 'list' that contains 'data' and deletes the element from 'list'. A free() is called on the data pointer of the element to free any dynamic memory allocated to the data pointer.
list_ptr_t list_free_data( list_ptr_t list, data_ptr_t data ){	
	list_ptr_t temp,temp1;
	temp=list;
	list_erro=0;
	while((temp->data!=*data)&&(temp!=NULL))
	   temp=temp->next;
	if(temp==NULL){
	    list_erro=6;
	    return NULL;
	}
	else{
	   temp1=temp;
	   temp=NULL;
	   free(temp1); 
	   return list;
	}
}

// Removes the element at index 'index' from 'list'. NO free() is called on the data pointer of the element. If 'index' is 0 or negative, the first element is removed. If 'index' is bigger than the number of elements in 'list', the data of the last element is removed.
list_ptr_t list_remove_at_index( list_ptr_t list, int index){	
	list_ptr_t temp,temp1;
	int i;
	list_erro=0;
	if(index<=0){
	  temp=list->next;
	  list=NULL;
	  list_erro=3;
	  return temp; 
    }
    else if(index>list_size(list)){
       temp=list;
       while(temp->next->next!=NULL)
          temp=temp->next;
       temp->next=NULL;
       list_erro=4;
       return list;
    }
    else{
	  if(list_size(list)==1){	     
	     return NULL;	
	  }
	  else{
		  if(index==1){
		      free(list);
		      return list->next; 
		  }
		  else{
		      temp=list;
	          for(i=2;i<index;i++)
                 temp=temp->next;
              
              temp1=temp->next;
	          temp->next=temp1->next;	 
	          temp1->next->prev=temp;        
	          free(temp1);
	          return list;
	      }
      }
    }
}

// Removes the element with reference 'reference' in 'list'. NO free() is called on the data pointer of the element. If 'reference' is NULL, the data of the last element is removed.
list_ptr_t list_remove_at_reference( list_ptr_t list, list_ptr_t reference ){	
	list_ptr_t temp;
	temp=list;
    int i=1;
    list_erro=0;
	if(reference==NULL){
	  list=list_remove_at_index(list,list_size(list));
	  list_erro=5;
	  return list;
   }
	else{
      while((temp->data!=reference->data)&&(temp!=NULL)){
          temp=temp->next;
          i++;
      }
      if(temp==NULL){	
	    list_erro=7;
	    return list;  		  
	  }
      else{
        list=list_remove_at_index(list,i);
	    return list;
      }
   }
}

// Finds the first element in 'list' that contains 'data' and removes the element from 'list'. NO free() is called on the data pointer of the element.
list_ptr_t list_remove_data( list_ptr_t list, data_ptr_t data ){	
 	list_ptr_t temp;
	temp=list;
	int i=1;
	list_erro=0;
	while((temp->data!=*data)&&(temp!=NULL)){
	   temp=temp->next;
       i++;
   }
	if(temp==NULL){
	    list_erro=6;
	    return list;
	}
	else{
	   list=list_remove_at_index(list,i);
	   return list;
	}
}


// Returns a reference to the first element of 'list'. If the list is empty, NULL is returned.
list_ptr_t list_get_first_reference( list_ptr_t list ){
	list_erro=0;
    if(list==NULL){
      list_erro=8;
      return NULL;
    }
    else
      return list;	
}


// Returns a reference to the last element of 'list'. If the list is empty, NULL is returned.
list_ptr_t list_get_last_reference( list_ptr_t list ){
	list_erro=0;
	list_ptr_t temp;
	temp=list;
    if(list==NULL){
       list_erro=8;
       return NULL;
    }
    else{
	   while(temp->next!=NULL)
	      temp=temp->next;
	   return temp;
	}       	
}

// Returns a reference to the next element of the element with reference 'reference' in 'list'. If the next element doesn't exists, NULL is returned.
list_ptr_t list_get_next_reference( list_ptr_t list, list_ptr_t reference ){	
    list_ptr_t temp;
	temp=list;
	list_erro=0;
	while((temp->data!=reference->data)&&(temp!=NULL))
	   temp=temp->next;	
	if(temp==NULL) {
	   list_erro=7;   
	   return NULL;
	}
	else{	  
	   return temp->next;
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
