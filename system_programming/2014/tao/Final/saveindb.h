#ifndef _SAVEINDB_H_
#define _SAVEINDB_H_


#include <my_global.h>
#include <mysql.h>
#include "DataServer.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

/*------------------------------------------------------------------------------
		definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/
#define host     "studev.groept.be"
#define username "a12_syssoft"
#define password "a12_syssoft"
#define database "a12_syssoft"
#define filename "myfile.data"






/*----------------------------------------------------
 * function prototype
 * --------------------------------------------------*/
 void save_fun(sensor_data_ptr_t data);
 //void copy_fun(MYSQL *conn,sensor_data_t data);
 
#endif
