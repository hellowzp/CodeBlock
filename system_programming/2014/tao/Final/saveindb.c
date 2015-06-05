#include <my_global.h>
#include <mysql.h>
#include "saveindb.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>



void save_fun(sensor_data_ptr_t data)
{
        printf("saveindb begin\n");
		MYSQL         	*conn; 
        sensor_data_t 	*buf;
		char 	        array[300];
        //init  database
		conn = mysql_init(NULL);
		printf("saveindb begin01\n");
        buf = data;
		if (conn == NULL) {
			printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
			exit(1);
		}
        //connect the database
		if (mysql_real_connect(conn, host, username, password, database, 0, NULL, 0) == NULL) {
			printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
			exit(1);
		}
		printf("saveindb begin02\n");
        //creat the database
		/*if (mysql_query(conn, "create database Database")) {
			printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
			exit(1);
		}*/

        
        //creat table bingtao and copy value from file to sql
        /*if (mysql_query(conn, "CREATE TABLE btao(id int not null primary key auto_increment,sensor_id int,sensor_value int,timestamp TIMESTAMP)")) {
			
			printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
		}*/
        printf("saveindb begin03\n");
        
		sprintf(array,"INSERT INTO btao (null,sensor_id,sensor_value,timestamp) values(null,%d,%d,%ld)",(int)data->id,(int)data->value,(long int)data->ts);
		
		//copy the string to array
			
		mysql_query(conn,array);
			
		memset(array,0,300);
		
		/*if ((mysql_real_query(conn, "SELECT sensor_id FROM bingtao WHERE sensor_value=%d",(int)(data->id))) != 0) {
                printf("insert failed %s\n", mysql_error(conn));
                exit(1);
		}*/
			
		mysql_close(conn);
		printf("saveindb closed\n");
			
}

        







