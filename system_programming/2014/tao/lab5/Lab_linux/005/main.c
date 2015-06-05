
/*-----------------------------------------------------------------------------
		include files
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "list.h"

/*------------------------------------------------------------------------------
		definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
		function prototypes
------------------------------------------------------------------------------*/
void ErrHandler( int erro );
void showOperation();
void printOutHeadToEnd(list_ptr_t list);
void printOutEndToHead(list_ptr_t list);


int main( )
//----------------------------------------------------------------------------//
{

    list_ptr_t s,sTemp,sRefer; 
	data_ptr_t value0;
	value0=(data_ptr_t)malloc(sizeof(data_t));
    int i=0;
    int size;
    int index;
    int referenceIndex;
    int chooseOperation=-1;
	showOperation();
	do{
		printf("\nchoose your operation number:");
		scanf("\n%d",&chooseOperation);
		if(chooseOperation!=-1){
		   if((chooseOperation<=24)&&(chooseOperation>=0)){
			   switch(chooseOperation){
				  case 0:
				     s=list_alloc();
				     ErrHandler(list_erro);
				     break;
				  case 1:
				     list_free(s);
				     ErrHandler(list_erro);
				     break;
				  case 2:
				     printf("\n%d",list_size(s));
				     ErrHandler(list_erro);
				     break;
				  case 3:
				     do{
				     printf("\nwrong data,please input inserted data:");    
                     scanf("\n%ld",value0);
				     }while(*(value0)<=0);					    				
					 printf("\nplease input index:");    	 
					 scanf("\n%d",&index);
					 s=list_insert_at_index(s,value0,index);  
					 ErrHandler(list_erro);     					 			         
				     break;
				  case 4:
				     printf("\nplease input index of your reference in the list:");    
				     scanf("\n%d",&referenceIndex);
				     printf("\nplease input inserted data:");    
                     scanf("\n%ld",value0);
                     sTemp=s;
                     for(i=0;i<referenceIndex;i++)
                         sTemp=sTemp->next;
                     s=list_insert_at_reference(s,value0,sTemp);
                     ErrHandler(list_erro);
				     break;
				  case 5:
				     do{
				     printf("\nwrong data,please input inserted data:");    
                     scanf("\n%ld",value0);
				     }while(*(value0)<=0);	
                     s=list_insert_sorted(s,value0);
                     ErrHandler(list_erro);
				     break;
				  case 6:
				     printf("\nplease input index of the list:");    
				     scanf("\n%d",&index);				  
				     s=list_free_at_index(s,index);
				     ErrHandler(list_erro);
				     break;
				  case 7:
				     printf("\nplease input index of your reference in the list:");    
				     scanf("\n%d",&referenceIndex);
                     sTemp=s;
                     for(i=0;i<referenceIndex;i++)
                         sTemp=sTemp->next;				     
		             s=list_free_at_reference(s,sTemp);
		             ErrHandler(list_erro);
				     break;
				  case 8:
				     do{
				     printf("\nwrong data,please input inserted data:");    
                     scanf("\n%ld",value0);
				     }while(*(value0)<=0);	
                     s=list_free_data(s,value0);			
                     ErrHandler(list_erro);	  
				     break;
				  case 9:
				     printf("\nplease input index of the list:");    
				     scanf("\n%d",&index);	
				     s=list_remove_at_index(s,index);	
				     ErrHandler(list_erro);			  
				     break;
				  case 10:
				     printf("\nplease input index of your reference in the list:");    
				     scanf("\n%d",&referenceIndex);
                     sTemp=s;
                     for(i=0;i<referenceIndex;i++)
                         sTemp=sTemp->next;		
                     s=list_remove_at_reference(s,sTemp);	
                     ErrHandler(list_erro);		  
				     break;
				  case 11:
				     do{
				     printf("\nwrong data,please input inserted data:");    
                     scanf("\n%ld",value0);
				     }while(*(value0)<=0);	
                     s=list_remove_data(s,value0);		
                     ErrHandler(list_erro);		  
				     break;
				  case 12:
				     printf("\nthe first element:%ld",s->data);
				     break;
				  case 13:
				     sTemp=s;
				     while(sTemp->next!=NULL)
				         sTemp=sTemp->next;
				     printf("\nthe last element:%ld",sTemp->data);
				     break;
				  case 14:
				     printf("\nplease input index of your reference in the list:");    
				     scanf("\n%d",&referenceIndex);
                     sTemp=s;
                     for(i=0;i<referenceIndex;i++)
                         sTemp=sTemp->next;
                     sTemp=list_get_next_reference(s,sTemp);
                     ErrHandler(list_erro);					  
                     printf("\n%ld",sTemp->data);
				     break;
				  case 15:
				     printf("\nplease input index of your reference in the list:");    
				     scanf("\n%d",&referenceIndex);
                     sTemp=s;
                     for(i=0;i<referenceIndex;i++)
                         sTemp=sTemp->next;
                     sTemp=list_get_previous_reference(s,sTemp);
                     ErrHandler(list_erro);					  
                     printf("\n%ld",sTemp->data);				  
				     break;
				  case 16:
				     printf("\nplease input index of your reference in the list:");    
				     scanf("\n%d",&referenceIndex);
                     sTemp=s;
                     for(i=0;i<referenceIndex;i++)
                         sTemp=sTemp->next;
                     printf("\n%d",list_get_index_of_reference(s,sTemp));
                     ErrHandler(list_erro);					  
				     break;
				  case 17:
				     printf("\nplease input index of the list:");    
				     scanf("\n%d",&index);	
                     printf("\n%ld",list_get_reference_at_index(s,index)->data);	
                     ErrHandler(list_erro);				  
				     break;
				  case 18:
				     printf("\nplease input index of your reference in the list:");    
				     scanf("\n%d",&referenceIndex);
                     sTemp=s;
                     for(i=0;i<referenceIndex;i++)
                         sTemp=sTemp->next;
                      printf("\n%ld",*(list_get_data_at_reference(s,sTemp)));	
                      ErrHandler(list_erro);				  
				     break;
				  case 19:
				     printf("\nplease input index of the list:");    
				     scanf("\n%d",&index);	
				     printf("\n%ld",*(list_get_data_at_index(s,index)));	
				     ErrHandler(list_erro);				  
				     break;
				  case 20:
                     printf("\nplease input inserted data:");    
                     scanf("\n%ld",value0);
                     printf("\nlist_get_index_of_data(s,value0)");	
                     ErrHandler(list_erro);				  
				     break;
				  case 21:
				     do{
				     printf("\nwrong data,please input inserted data:");    
                     scanf("\n%ld",value0);
				     }while(*(value0)<=0);	
                     sTemp=list_get_reference_of_data(s,value0);
                     ErrHandler(list_erro);	
                     printf("\n%ld",sTemp->data);				  
				     break;		
				  case 22:
				     printOutHeadToEnd(s);
				     break;
				  case 23:
				     printOutEndToHead(s);
				     break;
				  case 24:
				     showOperation();
				     break;					     		     				     				     				     				     				     				     				     				     				     				     				     				     				    				     				     				     				     				     				     				      			   
				}
			   
		    }
		    
		    else
		       printf("\nwrong operation number");				
		}
		
		
		
		
	}while(chooseOperation!=-1);
	
    list_free(s);
       
	free(value0);

	return 0;
}


void showOperation(){
  printf("\nchoose your operation:");
  printf("\n-1 :exit");
  printf("\n0 :  alloc list");
  printf("\n1 :free list");
  printf("\n2 :get list size;");
  printf("\n3 :insert data in list with index");
  printf("\n4 :insert data in list with reference");
  printf("\n5 :insert data in a sorted list");
  printf("\n6 :free an element with index");
  printf("\n7 :free an element with reference");
  printf("\n8 :free an element with specific data");
  printf("\n9 :remove an element with index");
  printf("\n10 :remove an element with reference");
  printf("\n11 :remove an element with specific data");
  printf("\n12 :get the first element of the list;");
  printf("\n13 :get the last element of the list");
  printf("\n14 :get the next element next to the reference");
  printf("\n15 :get the previous element of the reference");
  printf("\n16 :get the index of a reference");
  printf("\n17 :get a reference with its index");	
  printf("\n18 :get data of a specific reference");	
  printf("\n19 :get data of a index");	
  printf("\n20 :get the index of a specific data");	
  printf("\n21 :get the refrence of a specific data");
  printf("\n22 :print out list from head to end");
  printf("\n23 :print out list from end to head");	
  printf("\n24 :show operation number");	
}


//----------------------------------------------------------------------------//
void ErrHandler( int erro )
//----------------------------------------------------------------------------//
{
	switch( erro ) {
		case 0:
			break;
		case  1:
			printf("\nMemory problem occured while executing this operation on the list.\n" );
			break;
		case  2:
			printf( "\nCan't execute this operation while the list is empty.\n"  );
			break;
		case  3:
			printf( "\nindex is negative or 0,change index as 1 automaticlly.\n" );
			break;
		case  4:
			printf( "\nindex is larger than list size,change index as the index of its last element automaticlly.\n" );
			break;
		case 5:
			printf( "\nreference is NULL,change reference as the last element automaticlly.\n" );
			break;
		case 6:
		    printf("\ncan't find this data in the whole list\n");	
		    break;
		case 7:
		    printf("\ncan't find this reference in the list\n");	
		    break;		    
        case 8:
		    printf("\nlist is empty now\n");	
		    break;		    
		    
		default: // should never come here
			assert( 1==0 );
	}
}


void printOutHeadToEnd(list_ptr_t list){
	
	list_ptr_t temp;
	temp=list;
	printf("\nsize: %d\n",list_size(list));
	while(temp!=NULL){
	   printf("%5ld",temp->data);
	   temp=temp->next;
	}	
}


void printOutEndToHead(list_ptr_t list){
	
	list_ptr_t temp;
	temp=list;
	printf("\nsize: %d\n",list_size(list));
	while(temp->next!=NULL){temp=temp->next;}
	while(temp!=NULL){
	   printf("%5ld",temp->data);
	   temp=temp->prev;
	}	
}


