
#include <mysql.h>
#include <stdio.h>
#include <mysql/mysql.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define host            "localhost"              //mysql server
#define username        "a12_syssoft"
#define password        "a12_syssoft"
#define database         "www"
   
int get_file_size(char *path, off_t *size)
{
        struct stat file_stats;

        if(stat(path, &file_stats))
                return -1;

        *size = file_stats.st_size;
        return 0;
}
int main(int argc, char *argv[])
{
        char  *filename;
        off_t *size;

        MYSQL            *conn;
        MYSQL_RES    *res_set;
        MYSQL_ROW    row;
        MYSQL_FIELD *field;
        int           i, flag;
        char          *sql;
        FILE          *fp;
        char          *buf;
        int           n=0;
        char          *end;
        unsigned long *length;        
        
        if (argc != 2) {
                printf("Usage: %s srcfile\n", argv[0]);
                exit(1);
        }

        filename = argv[1];
        if ((get_file_size(filename, size)) == -1) {
                perror("get file size" );
                exit(1);
        }
        
        if ((buf = (char *)malloc(sizeof(char)*(*size+1))) == NULL) {
                perror("malloc buf" );
                exit(1);
        }

        if ((fp = fopen(filename, "rb" )) == NULL) {
                perror("fopen file" );
                exit(1);
        }

        if ((n = fread(buf, 1, *size, fp)) < 0) {        //n=*size
                perror("fread file" );
                exit(1);
        }

        sql = (char *)malloc(sizeof(char)*n*2+256);        //2n+1+strlen(other sql)
        if (sql == NULL) {
                perror("malloc sql" );
                exit(1);
        }

        conn = mysql_init(NULL);
        if (conn == NULL) {
                printf("init mysql, %s\n", mysql_error(conn));
                exit(1);
        }

        if ((mysql_real_connect(conn, host, username, password, database, 0, NULL, 0)) == NULL) {
                printf("connect mysql, %s\n", mysql_error(conn));
                exit(1);
        }

        strcpy(sql, "insert into www(id, name, file) values(5, 'peter', " );//用strcpy给sql赋值
        end = sql;
        end += strlen(sql);                //point sql tail
        //convert NUL(ASCII 0)、'\n'、'\r'、'\'’、'''、'"'和Control-Z and so on
        *end++ = '\'';
        end += mysql_real_escape_string(conn, end, buf, n);
        *end++ = '\'';
        *end++ = ')';
        
        flag = mysql_real_query(conn, sql, (unsigned int)(end-sql));
        if (flag != 0) {
                printf("insert failed, %s\n", mysql_error(conn));
                exit(1);
        }

        if ((mysql_real_query(conn, "SELECT file FROM www where id=5", 31)) != 0) {
                printf("insert failed, %s\n", mysql_error(conn));
                exit(1);
        }
        res_set = mysql_store_result(conn);

        fclose(fp);
        fp = NULL;

        fp = fopen("foo.bk", "wb" );
        while ((row = mysql_fetch_row(res_set)) != NULL) {
                length = mysql_fetch_lengths(res_set);
                for (i=0; i<mysql_num_fields(res_set); i++) {
                        fwrite(row[0], 1, length[0], fp);
                }
        }

        fclose(fp);
        mysql_close(conn);
        free(sql);
        sql = NULL;
        
        return 0;
}
