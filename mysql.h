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

void OpenMySqlDatabase (char *server, char *user, char *password, char *database);
void CloseMySqlDatabase();
int DoQuery (char query[1000]);
int DoQuery1 (char query[1000]);
int DoQuery2 (char query[1000]);

// mysql.h end
