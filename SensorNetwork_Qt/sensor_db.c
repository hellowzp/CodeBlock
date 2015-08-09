#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <stdio.h>
#include "sensor_db.h"


MYSQL *init_connection(char clear_up_flag) {
	MYSQL* con = mysql_init(NULL);
	if (con == NULL) {	
        fprintf(stderr, "Error: %d %s\n", mysql_errno(con), mysql_error(con));
        return NULL;
    }
    
    /* Connect to database */
    if (mysql_real_connect(con,HOST,USER,PASSWORD,DATABASE,0,NULL,0) == NULL) {
        fprintf(stderr, "Error: %d %s\n", mysql_errno(con), mysql_error(con));
        return NULL;
    }
    
    if(clear_up_flag==1) {
		char* dropStateMt = malloc(100);
		snprintf(dropStateMt,100,"DROP TABLE IF EXISTS %s",TABLE_NAME);
		if (mysql_query(con, dropStateMt)) {
			fprintf(stderr, "Error: %d %s\n", mysql_errno(con), mysql_error(con));
			free(dropStateMt);
			return NULL;
		}	
		free(dropStateMt);
	}
    
    char* sql = "create TABLE IF NOT EXISTS WANG_ZHIPENG(Id INT PRIMARY KEY AUTO_INCREMENT,\n"
			"sensor_id INT UNSIGNED, sensor_value DECIMAL(4,2), timestamp TIMESTAMP)";			
    if (mysql_query(con, sql)) {
        fprintf(stderr, "CREATE TABLE Error: %d %s\n", mysql_errno(con), mysql_error(con));
        return NULL;
    } else {
		return con;
	}
}
    
void disconnect(MYSQL *con) {
	mysql_close(con);
}

int insert_sensor(MYSQL *con, sensor_id_t id, sensor_value_t value, sensor_ts_t ts) {
	char* insertStateMt = malloc(200);
	snprintf(insertStateMt, 200, "INSERT INTO WANG_ZHIPENG (sensor_id,sensor_value,timestamp)"
							"VALUES(%d,%4.2f,from_unixtime(%ld))", id,(float)value/100.0f,ts);
	printf("insert query: %s\n",insertStateMt);
	if (mysql_query(con,insertStateMt)) {
		fprintf(stderr, "INSERT Error: %d %s\n", mysql_errno(con), mysql_error(con));
		free(insertStateMt);
        return -1;
    } else {
		free(insertStateMt);
		return 0;
	}
} 

MYSQL_RES *find_sensor_all(MYSQL *con) {
	if (mysql_query(con, "SELECT * FROM WANG_ZHIPENG")) {
		fprintf(stderr, "FIND SERSOR Error: %d %s\n", mysql_errno(con), mysql_error(con));
        exit(EXIT_FAILURE);
    }
    
    MYSQL_RES *result = mysql_store_result(con);
	if (result == NULL) {  //see also http://dev.mysql.com/doc/refman/5.6/en/mysql-store-result.html
		fprintf(stderr, "STORE RESULT Error: %d %s\n", mysql_errno(con), mysql_error(con));
		exit(EXIT_FAILURE);
	} else {
		return result;
	}
}

int get_result_size(MYSQL_RES *result) {
	return mysql_num_rows(result);
}

void print_result(MYSQL_RES *result) {
	MYSQL_ROW row;
	printf("sensor id\tsensor value\ttime\n");
	while ((row = mysql_fetch_row(result))) {
		int i, num_fields = mysql_num_fields(result);
		for(i = 0; i < num_fields; i++) {
			printf("%s\t", row[i] ? row[i] : "NULL"); 
		}
		printf("\n"); 
	}
	//mysql_free_result(result);
}

void free_result(MYSQL_RES* result) {
	mysql_free_result(result);
}
  
  
  
  


