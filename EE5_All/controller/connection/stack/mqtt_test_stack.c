/*******************************************************************************
* FILENAME: mqtt_test_stack.c
*
* Implements a stack of hardcoded MQTT messages to be sent back
* to the sensor as a way to test the connection
*
*******************************************************************************/

/*-----------------------------------------------------------------------------
		include files
------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include "stack.h"
#include "../../mqtt_sn_db/mqtt_sn_db.h"



/*------------------------------------------------------------------------------
		definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
		function prototypes
------------------------------------------------------------------------------*/

void destroy_element(element); 
void copy_element(element, element*);
int create_element(element* e, MESSAGE* value);
int db_create_message(MESSAGE** mes, ADDRESS address, DATABASE_ID message_id, EXTERN_ID extern_message_id, EXTERN_ID short_topic_id, MESSAGE_TYPE type, TOPIC_TYPE topic_type, QOS qos, time_t duration, uint32_t connection, FLAG duplicate, FLAG will, FLAG clean, FLAG retain, return_code return_value, blob* b);
int compare_with_top(MESSAGE* m_in);



/*------------------------------------------------------------------------------
		global variable declarations
------------------------------------------------------------------------------*/

int i = 0;
stack s;
element tmp;



/*------------------------------------------------------------------------------
		implementation code
------------------------------------------------------------------------------*/

int main( void ){
	
	//Init the stack
	stack_init(&s, &destroy_element, &copy_element);
	
	//Create 9 MQTT messages
	MESSAGE* m1 = malloc(sizeof(MESSAGE));
	MESSAGE* m2 = malloc(sizeof(MESSAGE));
	MESSAGE* m3 = malloc(sizeof(MESSAGE));
	MESSAGE* m4 = malloc(sizeof(MESSAGE));
	MESSAGE* m5 = malloc(sizeof(MESSAGE));
	MESSAGE* m6 = malloc(sizeof(MESSAGE));
	MESSAGE* m7 = malloc(sizeof(MESSAGE));
	MESSAGE* m8 = malloc(sizeof(MESSAGE));
	MESSAGE* m9 = malloc(sizeof(MESSAGE));
	
	//Fill messages
	db_create_message(&m1, 0x0013A200406FB1E5, 0, 0, 0, GWINFO, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL);
	db_create_message(&m2, 0x0013A200406FB1E5, 0, 0, 0, WILLTOPICREQ, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL);
	db_create_message(&m3, 0x0013A200406FB1E5, 0, 0, 0, WILLMSGREQ, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL);
	db_create_message(&m4, 0x0013A200406FB1E5, 0, 0, 0, CONNACK, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL);
	db_create_message(&m5, 0x0013A200406FB1E5, 0, 0, 1, REGACK, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL);
	db_create_message(&m6, 0x0013A200406FB1E5, 0, 0, 2, REGACK, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL);
	db_create_message(&m7, 0x0013A200406FB1E5, 0, 0, 3, REGACK, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL);
	db_create_message(&m8, 0x0013A200406FB1E5, 0, 0, 4, REGACK, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL);
	db_create_message(&m9, 0x0013A200406FB1E5, 0, 0, 0, DISCONNECT, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL);
	
	//Push them on the stack (reverse order!)
	create_element(&tmp, m9);
	push(s, tmp);
	
	create_element(&tmp, m8);
	push(s, tmp);
	
	create_element(&tmp, m7);
	push(s, tmp);
	
	create_element(&tmp, m6);
	push(s, tmp);
	
	create_element(&tmp, m5);
	push(s, tmp);
	
	create_element(&tmp, m4);
	push(s, tmp);
	
	create_element(&tmp, m3);
	push(s, tmp);
	
	create_element(&tmp, m2);
	push(s, tmp);
	
	create_element(&tmp, m1);
	push(s, tmp);
	
	//Compare
	printf("Compare test 1: %d\n", compare_with_top(m1));
	printf("Compare test 2: %d\n", compare_with_top(m2));
	printf("Compare test 3: %d\n", compare_with_top(m3));
	printf("Compare test 4: %d\n", compare_with_top(m4));
	printf("Compare test 5: %d\n", compare_with_top(m5));
	printf("Compare test 6: %d\n", compare_with_top(m6));
	printf("Compare test 7: %d\n", compare_with_top(m7));
	printf("Compare test 8: %d\n", compare_with_top(m8));
	printf("Compare test 9: %d\n", compare_with_top(m9));
	
	//Clean up
	free(m1);
	free(m2);
	free(m3);
	free(m4);
	free(m5);
	free(m6);
	free(m7);
	free(m8);
	free(m9);
	
	return 0;
}

int compare_with_top(MESSAGE* m_in){
	//Get top of stack
	top(s, &tmp);
	MESSAGE* m_stack = (MESSAGE*) tmp;
	i++;
	
	//Compare headers
	if(m_in->address != m_stack->address){
		printf("ERROR: message %d doesn't match! (address)\n",i);
		printf("       On stack: %ld // Incoming: %ld\n", (long) m_stack->address, (long) m_in->address);
		return 1;
	}
	
	if(m_in->message_id != m_stack->message_id){
		printf("ERROR: message %d doesn't match! (message_id)\n",i);
		printf("       On stack: %d // Incoming: %d\n", (int) m_stack->message_id, (int) m_in->message_id);
		return 1;
	}
	
	if(m_in->extern_message_id != m_stack->extern_message_id){
		printf("ERROR: message %d doesn't match! (extern_message_id)\n",i);
		return 1;
	}
	
	if(m_in->short_topic_id != m_stack->short_topic_id){
		printf("ERROR: message %d doesn't match! (short_topic_id)\n",i);
		return 1;
	}
	
	if(m_in->type != m_stack->type){
		printf("ERROR: message %d doesn't match! (type)\n",i);
		printf("       On stack: %d // Incoming: %d\n", (int) m_stack->type, (int) m_in->type);
		return 1;
	}
	
	if(m_in->topic_type != m_stack->topic_type){
		printf("ERROR: message %d doesn't match! (topic_type)\n",i);
		return 1;
	}
	
	if(m_in->qos != m_stack->qos){
		printf("ERROR: message %d doesn't match! (qos)\n",i);
		return 1;
	}
	
	if(m_in->duration != m_stack->duration){
		printf("ERROR: message %d doesn't match! (duration)\n",i);
		return 1;
	}
	
	if(m_in->duplicate != m_stack->duplicate){
		printf("ERROR: message %d doesn't match! (duplicate)\n",i);
		return 1;
	}
	
	if(m_in->will != m_stack->will){
		printf("ERROR: message %d doesn't match! (will)\n",i);
		return 1;
	}
	
	if(m_in->clean != m_stack->clean){
		printf("ERROR: message %d doesn't match! (clean)",i);
		return 1;
	}
	
	if(m_in->retain != m_stack->retain){
		printf("ERROR: message %d doesn't match! (retain)\n",i);
		return 1;
	}
	
	if(m_in->return_value != m_stack->return_value){
		printf("ERROR: message %d doesn't match! (return_value)\n",i);
		return 1;
	}
	
	if(m_in->length != m_stack->length){
		printf("ERROR: message %d doesn't match! (length)\n",i);
		return 1;
	}
	
	//Compare messages
	if(m_in->message == NULL){
		if(m_stack->message != NULL){
			printf("ERROR: message %d doesn't match! (message)\n",i);
			return 1;
		}
	} else if(m_stack->message == NULL){
		if(m_in->message != NULL){
			printf("ERROR: message %d doesn't match! (message)\n",i);
			return 1;
		}
	} else{
		//Compare the content of the 2 message pointers
		for(unsigned int k=0; k < (m_in->length); k++){
			char* byte1 = (char*) (m_in->message);
			char* byte2 = (char*) (m_stack->message);
			if(*byte1 != *byte2){
				printf("ERROR: message %d doesn't match! (message)\n",i);
				return 1;
			}	
		}
	}
	
	return 0;
}

int db_create_message(MESSAGE** mes, ADDRESS address, DATABASE_ID message_id, EXTERN_ID extern_message_id, EXTERN_ID short_topic_id, MESSAGE_TYPE type, TOPIC_TYPE topic_type, QOS qos, time_t duration, uint32_t connection_id, FLAG duplicate, FLAG will, FLAG clean, FLAG retain, return_code return_value, blob* b){
	MESSAGE* m = (MESSAGE*) malloc (sizeof(MESSAGE));
	m->address = address;
	m->message_id = message_id;
	m->extern_message_id = extern_message_id;
	m->short_topic_id = short_topic_id;
	m->type = type;
	m->topic_type = topic_type;
	m->qos = qos;
	m->duration = duration;
	m->duplicate = duplicate;
	m->will = will;
	m->clean = clean;
	m->retain = retain;
	m->return_value = return_value;

	if(b != NULL){
		m->length = b->size;
		m->message = b->data;
		 free(b);
	} else {
		m->length = 0;
		m->message = NULL;
	}
	*mes = m;
	return 0;
}

int create_element(element* e, MESSAGE* value){
	*e = (element) value;
	return STACK_OK;
}

void destroy_element(element data)  {
	free( data );
}

void copy_element(element data, element* copy) {
	*copy = malloc( sizeof(MESSAGE) );
	*((MESSAGE*) *copy) = *((MESSAGE*) data);
	return;
}
