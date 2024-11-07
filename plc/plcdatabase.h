#ifndef _PLCDATANASE_H_
#define _PLCDATANASE_H_
#include "../database/mysqlbase.h"
#define DATABASE "routerbase"
#define IP "127.0.0.1"
// plcSqlite_free
// plcSqlite_init

struct mysqlStruct* plcDatabase_init();

//第二个参数可以释放结果指针，也可以为NULL自己手动释放
void plcDatabase_free(struct mysqlStruct* mysqldb, struct mysql_res_struct *selectRes);


int updateTableName(struct mysqlStruct* mysqldb, char* oldName, char* newName);
int createTable(struct mysqlStruct* mysqldb, char* name);
int deleteTable(struct mysqlStruct* mysqldb, char* name);
int select_last_data(struct mysqlStruct* mysqldb,char **resArray,char *tableName);
int isIn_tableNames(struct mysqlStruct* mysqldb, char *database, char *tableName);
int delete_all_data(struct mysqlStruct* mysqldb, char* name);



#endif
