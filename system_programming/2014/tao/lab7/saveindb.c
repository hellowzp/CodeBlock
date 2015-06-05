#include <my_global.h>
#include <mysql.h>
#include "config.h"

#define host     "studev.groept.be"
#define username "a12_syssoft"
#define password "a12_syssoft"
#define Database "bingtao"
#define filename "myfile.dat"

int main(int argc, char **argv)
{

		MYSQL         	*conn;
        MYSQL_RES     	*res_set;
        MYSQL_ROW    	 row;
        MYSQL_FIELD   	*field;
        int          	 i, flag;
        sensor_data_ptr_t  sql;
        FILE          	*fp;
        sensor_data_t 	*buf;
        sensor_data_ptr_t n;
        char          	*end;
        unsigned long 	*length; 
		conn = mysql_init(NULL);

		if (conn == NULL) {
			printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
			exit(1);
		}

		if (mysql_real_connect(conn, host, username, password, NULL, 0, NULL, 0) == NULL) {
			printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
			exit(1);
		}

		if (mysql_query(conn, "create database Database")) {
			printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
			exit(1);
		}
    
   
        
        if ((buf = (sensor_data_ptr_t)malloc(sizeof(sensor_data_t)) == NULL) {
                perror("malloc buf" );
                exit(1);
        }

        if ((fp = fopen(filename, "rb" )) == NULL) {
                perror("fopen file" );
                exit(1);
        }

        
        //copy value of file to sql
        mysql_query(conn, "CREATE TABLE sensor(id int not null primary key auto_increment,sensor_id int,sensor_value float,timestamp varchar(20))");
        while(n = fread(buf, 1, sizeof(sensor_data_t), fp))!=NULL){                 //read the value from file
			
			mysql_query(conn,"insert into sensor(sensor_id sensor_value timestamp) values(n->id,n->value,(char)n->ts)");

			if ((n==NULL) {        
                perror("fread file" );
                exit(1);
			}
			if ((mysql_real_query(conn, "SELECT sensor_id FROM sensor where sensor_value=n->value",(int)(n->id))) != 0) {
                printf("insert failed, %s\n", mysql_error(conn));
                exit(1);
			}
			
			n++;
        }

        fclose(fp);
        mysql_close(conn);
        
        return 0;





}

