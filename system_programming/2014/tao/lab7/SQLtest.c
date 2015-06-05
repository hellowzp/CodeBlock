#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
MYSQL mysql;
main()
{
   char host[32]="-host";
   char user[32]="a12_syssoft";
   char passwd[32]="a12_syssoft";
   char dbname[32]="bingtao";   
   
   if( mysql_init(&mysql) == NULL )
   {
      syslog(LOG_USER|LOG_INFO,"inital mysql handle errorn");
      return 1;
   }
   if (mysql_real_connect(&mysql,host,user,passwd,dbname,0,NULL,0) == NULL)
   {
      syslog(LOG_USER|LOG_INFO, "Failed to connect to database: Error: %sn",mysql_error(&mysql));
      return 1;
   }
   else syslog(LOG_USER|LOG_INFO, "connect to database: n");
   find_ps();   
   db_close();
   return 0;
}
int db_close()
{
        mysql_close(&mysql);
         return 0;
}


int  find_ps ()
{
   MYSQL_ROW m_row;
   MYSQL_RES *m_res;
   char sql[1024],username[32];
   int res=1;
   int *id;
   sprintf(sql,"select id from username");
   if(mysql_query(&mysql,sql) != 0)
   {
      syslog(LOG_USER|LOG_INFO, "select ps_info Error: %sn",mysql_error(&mysql));
      return res;
   }
   m_res = mysql_store_result(&mysql);
   if(m_res==NULL)
   {
      syslog(LOG_USER|LOG_INFO, "select username Error: %sn",mysql_error(&mysql));
      res = 3;
      return res;
   }
   if(m_row = mysql_fetch_row(m_res))
   {
      printf("m_row=%dn",atoi(m_row[0]));
      res = 0;      
   }
   mysql_free_result(m_res);
   return res;
}
