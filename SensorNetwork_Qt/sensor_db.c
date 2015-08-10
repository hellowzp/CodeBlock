#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <stdio.h>
#include "sensor_db.h"
#include "util.h"


MYSQL* init_connection() {
	MYSQL* con = mysql_init(NULL);
	if (con == NULL) {	
        fprintf(stderr, "Error: %d %s\n", mysql_errno(con), mysql_error(con));
        return NULL;
    }
    
    /* Connect to database */
    DEBUG_PRINT("Connecting to MySQL: -h %s -u%s -p%s\n", HOST, USER, PASSWORD);
    if (mysql_real_connect(con,HOST,USER,PASSWORD,DATABASE,0,NULL,0) == NULL) {
        fprintf(stderr, "Error: %d %s\n", mysql_errno(con), mysql_error(con));
        return NULL;
    }
    return con;
}
    
void disconnect(MYSQL *con) {
	mysql_close(con);
}

int insert_sensor(MYSQL *con, sensor_id_t id, sensor_value_t value, sensor_ts_t ts) {
	char* insertStateMt = malloc(200);
    snprintf(insertStateMt, 200, "INSERT INTO %s (sensor_id,sensor_value,timestamp) "
                            "VALUES(%d,%3.2f,from_unixtime(%ld))",
                            TABLE_NAME, id,(float)value/10.0f,ts);
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
    char* query = malloc(50);
    snprintf(query, 50, "SELECT * FROM %s", TABLE_NAME);
    if (mysql_query(con, query)) {
        fprintf(stderr, "FIND SENSOR Error: %d %s\n", mysql_errno(con), mysql_error(con));
        exit(EXIT_FAILURE);
    }
    free(query);
    
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
  
  
  
  


