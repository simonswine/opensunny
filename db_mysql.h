// mysql.h
#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* define mysql global vars */
MYSQL *conn;
MYSQL_RES *res;
MYSQL_RES *res1;
MYSQL_RES *res2;

void db_mysql_open_db (char *server, char *user, char *password, char *database);
void db_mysql_close_db();
int db_mysql_query (char query[1000]);
int db_mysql_query1 (char query[1000]);
int db_mysql_query2 (char query[1000]);

// mysql.h end
