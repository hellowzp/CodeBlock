#include "config.h"
#include "sensor_db.h"

#include <assert.h>
#include <string.h>


DBCONN * init_connection(char clear_up_flag)
{
    DBCONN * conn;

    if (sqlite3_open(TO_STRING(DB_NAME), &conn) != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(conn));
        sqlite3_close(conn);
        exit(EXIT_FAILURE);
    }

    int length = 256;
    char sql[256];
    char *err_msg;
    if (clear_up_flag)
    {
        snprintf(sql, length, "DROP TABLE IF EXISTS %s;\nCREATE TABLE %s %s",TO_STRING(TABLE_NAME), TO_STRING(TABLE_NAME),
            "(id INTEGER PRIMARY KEY, sensor_id INTEGER, sensor_value DECIMAL(4,2),timestamp TIMESTAMP);");
    }else{
        snprintf(sql, length, "CREATE TABLE IF NOT EXISTs %s%s", TO_STRING(TABLE_NAME),
            "(id INTEGER PRIMARY KEY, sensor_id INTEGER, sensor_value DECIMAL(4,2),timestamp TIMESTAMP);");
    }

    if (sqlite3_exec(conn, sql, NULL, NULL, &err_msg) != SQLITE_OK)
    {
        fprintf(stderr, "SQL connection error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(conn);
        exit(EXIT_FAILURE);
    }

    return conn;
}

void disconnect(DBCONN *conn)
{
    sqlite3_close(conn);
}

int insert_sensor(DBCONN * conn, sensor_id_t id, sensor_value_t value, sensor_ts_t ts)
{
    assert(conn!= NULL);
    int length = 256;
    char sql[length];
    char *err_msg;
    snprintf(sql, length, "INSERT INTO SensorData(sensor_id, sensor_value, timestamp) VALUES(%d, %g, %ld);", id, value, ts);
    if (sqlite3_exec(conn, sql, NULL, NULL, &err_msg) != SQLITE_OK)
    {
        fprintf(stderr, "SQL insert error: %s: %s\n", sql, err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(conn);
        return -1;
    }
    return 0;
}

/**/
int insert_sensor_from_file(DBCONN * conn, FILE * sensor_file)
{
    sensor_data_t data;
    while ( ! feof(sensor_file))
    {
        fread(&data.id, sizeof(sensor_id_t), 1, sensor_file);
        fread(&data.value, sizeof(sensor_value_t), 1, sensor_file);
        fread(&data.ts, sizeof(sensor_ts_t), 1, sensor_file);
        insert_sensor(conn, data.id, data.value, data.ts);
    }
    return 0;
}

void storagemgr_parse_sensor_data(DBCONN * conn, sbuffer_t **sbuffer)
{
    sensor_data_t sensor_data;

    while (1)
    {
        sbuffer_data_t sbuffer_data;
        if(sbuffer_remove(*sbuffer, &sbuffer_data, 5, 0) == SBUFFER_NO_DATA)
            break;
        sensor_data = sbuffer_data.sensor_data;
        insert_sensor(conn, sensor_data.id, sensor_data.value, sensor_data.ts);
    }
}

int find_sensor_all(DBCONN * conn, callback_t f)
{
    int length = 256;
    char sql[length];
    char *err_msg;
    snprintf(sql, length, "SELECT * FROM %s;", TO_STRING(TABLE_NAME));

    if (sqlite3_exec(conn, sql, f, NULL, &err_msg) != SQLITE_OK)
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
    int length = 256;
    char sql[length];
    char *err_msg;
    snprintf(sql, length, "SELECT * FROM %s WHERE sensor_value = %f;", TO_STRING(TABLE_NAME), value);

    if (sqlite3_exec(conn, sql, f, NULL, &err_msg) != SQLITE_OK)
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
    int length = 256;
    char sql[length];
    char *err_msg;
    snprintf(sql, length, "SELECT * FROM %s WHERE sensor_value > %f;", TO_STRING(TABLE_NAME), value);

    if (sqlite3_exec(conn, sql, f, NULL, &err_msg) != SQLITE_OK)
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
    int length = 256;
    char sql[length];
    char *err_msg;
    snprintf(sql, length, "SELECT * FROM %s WHERE timestamp = %ld;", TO_STRING(TABLE_NAME), ts);

    if (sqlite3_exec(conn, sql, f, NULL, &err_msg) != SQLITE_OK)
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
    int length = 256;
    char sql[length];
    char* err_msg;
    snprintf(sql, length, "SELECT * FROM %s WHERE timestamp>%ld;", TO_STRING(TABLE_NAME), ts);

    if (sqlite3_exec(conn, sql, f, NULL, &err_msg) != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(conn);
        return -1;
    }
    return 0;
}
