#include <string.h>
#include <sqlite3.h>
#include "errmacros.h"
#include "config.h"
#include "sbuffer.h"
#include "sensor_db.h"

extern FILE *fp_FIFO_w;
DBCONN * init_connection(char clear_up_flag)
{
	sqlite3 *db;
	/* Database filename (UTF-8), 
	 * OUT: SQLite db handle */
	int rc = sqlite3_open(TO_STRING(DB_NAME), &db);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
	
		//flockfile(fp_FIFO_w);
		fprintf(fp_FIFO_w, "Unable to connect to SQL server.\n");
		fflush(fp_FIFO_w);
#ifdef DEBUG		
		fprintf(stderr, "DEBUG log_mssge, Unable to connect to SQL server.\n");
#endif		
		//funlockfile(fp_FIFO_w);

		sqlite3_close(db);
		return NULL;
	}
	//flockfile(fp_FIFO_w);
	fprintf(fp_FIFO_w, "Connection to SQL server established.\n");
	fflush(fp_FIFO_w);
#ifdef DEBUG	
	fprintf(stderr, "DEBUG log_mssge, Connection to SQL server established.\n");
#endif	
	//funlockfile(fp_FIFO_w);

	char sql[256];
	char *err_msg;
	if (clear_up_flag)
	{
		sprintf(sql, "DROP TABLE IF EXISTS %s;\nCREATE TABLE %s%s",TO_STRING(TABLE_NAME), TO_STRING(TABLE_NAME), 
			"(id INTEGER PRIMARY KEY, sensor_id INTEGER, sensor_value DECIMAL(4,2),timestamp TIMESTAMP);");
	}else{
		sprintf(sql, "CREATE TABLE IF NOT EXISTs %s%s", TO_STRING(TABLE_NAME), 
			"(id INTEGER PRIMARY KEY, sensor_id INTEGER, sensor_value DECIMAL(4,2),timestamp TIMESTAMP);");
	}
	
	/* An open database i, 
	 * SQL to be evaluated, 
	 * 1st argument to callback, 
	 * Callback function, 
	 * Error msg written here */
	rc = sqlite3_exec(db, sql, NULL, NULL, &err_msg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", err_msg);
		//flockfile(fp_FIFO_w);
		fprintf(fp_FIFO_w, "Connection to SQL server lost.\n");
		fflush(fp_FIFO_w);
#ifdef DEBUG		
		fprintf(stderr, "DEBUG log_mssge, Connection to SQL server lost.\n");
#endif		
		//funlockfile(fp_FIFO_w);

		sqlite3_free(err_msg);
		sqlite3_close(db);
		return NULL;
	}

	//flockfile(fp_FIFO_w);
	fprintf(fp_FIFO_w, "New table %s created.\n", TO_STRING(TABLE_NAME));
	fflush(fp_FIFO_w);
#ifdef DEBUG	
	fprintf(stderr, "DEBUG log_mssge, New table %s created.\n", TO_STRING(TABLE_NAME));
#endif	
	//funlockfile(fp_FIFO_w);

	return db;
}

void disconnect(DBCONN *conn)
{
	sqlite3_close(conn);
	//flockfile(fp_FIFO_w);
	fprintf(fp_FIFO_w, "Connection to SQL server lost.\n");
	fflush(fp_FIFO_w);
#ifdef DEBUG	
	fprintf(stderr, "DEBUG log_mssge, Connection to SQL server lost.\n");
#endif	
	//funlockfile(fp_FIFO_w);
}

int insert_sensor(DBCONN * conn, sensor_id_t id, sensor_value_t value, sensor_ts_t ts)
{
	char sql[256];
	char *err_msg;
	int rc = 0;
	sprintf(sql, "INSERT INTO SensorData(sensor_id, sensor_value, timestamp) VALUES(%d, %g, %ld);", id, value, ts);
	rc = sqlite3_exec(conn, sql, NULL, NULL, &err_msg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
		sqlite3_close(conn);
		return -1;
	}
	return 0;
}
/**/
void storagemgr_parse_sensor_data(DBCONN * conn, sbuffer_t **sbuffer)
{
	sensor_data_t * measurement = NULL;
	measurement = malloc(sizeof(sensor_data_t));
	MALLOC_ERROR(measurement);

	while (1)
	{
		sbuffer_data_t sbuffer_data;
		int retval = sbuffer_remove(*sbuffer, &sbuffer_data, 5, 0);
		if(retval == SBUFFER_NO_DATA)
			break;
		*measurement = sbuffer_data.sensor_data;
		insert_sensor(conn, measurement->id, measurement->value, measurement->ts);
	} 
	free(measurement);
}

int find_sensor_all(DBCONN * conn, callback_t f)
{
	char sql[256];
	char *err_msg;
	int rc = 0;
	sprintf(sql, "SELECT * FROM %s;", TO_STRING(TABLE_NAME));

	rc = sqlite3_exec(conn, sql, f, NULL, &err_msg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
		sqlite3_close(conn);
		return -1;
	}

	return 0;
}


int find_sensor_by_value(DBCONN * conn, sensor_value_t value, callback_t f)
{
	char sql[256];
	char *err_msg;
	int rc = 0;
	sprintf(sql, "SELECT * FROM %s WHERE sensor_value==%f;", TO_STRING(TABLE_NAME), value);
	
	rc = sqlite3_exec(conn, sql, f, NULL, &err_msg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
		sqlite3_close(conn);
		return -1;
	}
	return 0;
}

int find_sensor_exceed_value(DBCONN * conn, sensor_value_t value, callback_t f)
{
	char sql[256];
	char *err_msg;
	int rc = 0;
	sprintf(sql, "SELECT * FROM %s WHERE sensor_value>%f;", TO_STRING(TABLE_NAME), value);

	rc = sqlite3_exec(conn, sql, f, NULL, &err_msg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
		sqlite3_close(conn);
		return -1;
	}
	return 0;
}


int find_sensor_by_timestamp(DBCONN * conn, sensor_ts_t ts, callback_t f)
{
	char sql[256];
	char *err_msg;
	int rc = 0;
	sprintf(sql, "SELECT * FROM %s WHERE timestamp==%ld;", TO_STRING(TABLE_NAME), ts);

	rc = sqlite3_exec(conn, sql, f, NULL, &err_msg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
		sqlite3_close(conn);
		return -1;
	}
	return 0;
}

int find_sensor_after_timestamp(DBCONN * conn, sensor_ts_t ts, callback_t f)
{
	char sql[256];
	char *err_msg;
	int rc = 0;
	sprintf(sql, "SELECT * FROM %s WHERE timestamp>%ld;", TO_STRING(TABLE_NAME), ts);

	rc = sqlite3_exec(conn, sql, f, NULL, &err_msg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
		sqlite3_close(conn);
		return -1;
	}
	return 0;
}
