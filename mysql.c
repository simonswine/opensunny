#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


MYSQL *conn;
MYSQL_RES *res;
MYSQL_RES *res1;
MYSQL_RES *res2;


void OpenMySqlDatabase (char *server, char *user, char *password, char *database)
{
   
	
	conn = mysql_init(NULL);
   /* Connect to database */
   if (!mysql_real_connect(conn, server,
         user, password, database, 0, NULL, 0)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(0);
   }
}

void CloseMySqlDatabase()
{
   /* Release memory used to store results and close connection */
   mysql_free_result(res);
   mysql_free_result(res1);
   mysql_free_result(res2);
   mysql_close(conn);
}

int DoQuery (char query[1000]){
	/* execute query */
	
	if (mysql_real_query(conn, query, strlen(query))){
		fprintf(stderr, "%s\n", mysql_error(conn));
	}
	res = mysql_store_result(conn);
	return *mysql_error(conn);
}

int DoQuery1 (char query[1000]){
	/* execute query */
	
	if (mysql_real_query(conn, query, strlen(query))){
		fprintf(stderr, "%s\n", mysql_error(conn));
	}
	res1 = mysql_store_result(conn);
	return *mysql_error(conn);
}

int DoQuery2 (char query[1000]){
	/* execute query */
	
	if (mysql_real_query(conn, query, strlen(query))){
		fprintf(stderr, "%s\n", mysql_error(conn));
	}
	res2 = mysql_store_result(conn);
	return *mysql_error(conn);
}
