/****************************************
*      Database queryer
****************************************/

/*-----------------------------------------------------------------------------
		include files
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <sys/types.h>
#include <my_global.h>
#include <mysql.h>
#include <time.h>

/*------------------------------------------------------------------------------
		definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/
#define OVER_MAX_VALUE_QUERY "SELECT * FROM a12_syssoft.mingzheng_wang WHERE sensor_value > 3000  ORDER BY timestamp"
#define ALL_DATA_QUERY "SELECT * FROM a12_syssoft.mingzheng_wang ORDER BY timestamp" 
#define DELETE_ALL_QUERY "DELETE FROM a12_syssoft.mingzheng_wang WHERE id > 0"

/*------------------------------------------------------------------------------
		function prototypes
------------------------------------------------------------------------------*/
void get_data_over_max(MYSQL *);
void get_all_data(MYSQL *);
void delete_all_data(MYSQL *conn);
void switcher(MYSQL *conn, char instruction);

//----------------------------------------------------------------------------//
int main()
//----------------------------------------------------------------------------//
{
	MYSQL *conn;
	char instruction;

	printf("MySQL client version: %s\n", mysql_get_client_info());
	//initialize the database
	conn = mysql_init(NULL);
	//exit if the initialization failed
	if (conn == NULL) 
	{
      		printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
      		exit(1);
 	}

	if (mysql_real_connect(conn, "studev.groept.be", "a12_syssoft", "a12_syssoft", "a12_syssoft", 0, NULL, 0) == NULL) 
	{
      		printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
      		exit(1);
  	}

	printf("\n");
	printf("********           Instruction for query                       ********\n");
	printf("******** 'a' to get the data larger than maximun value (3000)  ********\n");
	printf("********      'g' to get all the data ordered by time          ********\n");
	printf("********      'd' to delete all data in the table              ********\n");
	printf("********             'q' to exit the program                   ********\n");
	while (instruction != 'q')
	{
		scanf("%c", &instruction);
		switcher(conn, instruction);
	}

	mysql_close(conn);

	return 0;
}

void switcher(MYSQL *conn, char instruction)
{
	switch (instruction)
	{
		case 'a':
			get_data_over_max(conn);
			return;
		case 'g':
			get_all_data(conn);
			return;
		case 'd':
			delete_all_data(conn);
			return;
		case 'q':
			mysql_close(conn);
			exit(1);
		default:
			mysql_close(conn);
			exit(1);
	}
	return;
}

//----------------------------------------------------------------------------//
void get_data_over_max(MYSQL *conn)
//----------------------------------------------------------------------------//
{
	MYSQL_RES *result;
	MYSQL_ROW row;
	MYSQL_FIELD *field;
	int num_fields;
	int i;
	time_t timestamp;

	//excute the query
	if (mysql_query(conn, OVER_MAX_VALUE_QUERY))
	{
		printf("Query failed\n");
		return;
	}
  	result = mysql_store_result(conn);
	num_fields = mysql_num_fields(result);
	
	//print out the field names
	while((field = mysql_fetch_field(result)))
	{
		printf("%s\t      ", field->name);
	}
	printf("\n");

	while ((row = mysql_fetch_row(result)))
  	{
      		for(i = 0; i < num_fields; i++)
      		{
			if (i == 3)
			{
				//format the timestamp and print
				timestamp = (atoi(row[i]));
				printf("%s", ctime(&timestamp));
			}
			else
         			printf("%s\t\t", row[i] ? row[i] : "NULL");
      		}
 	}
	//free the result set and close the database
	mysql_free_result(result);

}

//----------------------------------------------------------------------------//
void get_all_data(MYSQL *conn)
//----------------------------------------------------------------------------//
{
	MYSQL_RES *result;
	MYSQL_ROW row;
	int num_fields;
	MYSQL_FIELD *field;
	int i;
	time_t timestamp;
	
	//excute the query
	if (mysql_query(conn, OVER_MAX_VALUE_QUERY))
	{
		printf("Query failed\n");
		return;
	}
	result = mysql_store_result(conn);
	num_fields = mysql_num_fields(result);

	//print out the field names
	while((field = mysql_fetch_field(result)))
	{
		printf("%s\t   ", field->name);
	}
	printf("\n");

	while ((row = mysql_fetch_row(result)))
  	{
      		for(i = 0; i < num_fields; i++)
      		{
         		if (i == 3)
			{
				//format the timestamp and print
				timestamp = (atoi(row[i]));
				printf("%s", ctime(&timestamp));
			}
			else
         			printf("%s\t\t", row[i] ? row[i] : "NULL");
      		}
 	}
	//free the result set and close the database
	mysql_free_result(result);
}

//----------------------------------------------------------------------------//
void delete_all_data(MYSQL *conn)
//----------------------------------------------------------------------------//
{
	//query to delete all the data
	if (!mysql_query(conn, DELETE_ALL_QUERY))  //query to delete all data in the table
		printf("Deletion completed\n");
	else 
		printf("Deletion was failed\n");
}





