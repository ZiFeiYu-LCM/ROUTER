#include "plcdatabase.h"


//创建并初始化struct mysqlStruct结构
struct mysqlStruct* plcDatabase_init(){
	struct mysqlStruct *mysqldb = (struct mysqlStruct*)malloc(sizeof(struct mysqlStruct));
	if(mysqldb == NULL){
			return NULL;
	}
	mysqlInit(mysqldb);
	
	return mysqldb;
}

//关闭数据库连接并  释放struct mysqlStruct   也可以释放struct mysql_res_struct结构体
void plcDatabase_free(struct mysqlStruct* mysqldb, struct mysql_res_struct *selectRes){
	if(selectRes != NULL){
		mysqldb->freeRes(selectRes);
	}
	if(mysqldb != NULL){
		mysqldb->closeDb(mysqldb);
		free(mysqldb);
	}
	mysqldb = NULL;
}

//修改表名
int updateTableName(struct mysqlStruct* mysqldb, char* oldName, char* newName){
	char sql[100];
	snprintf(sql,100,"ALTER TABLE %s RENAME TO %s;",oldName,newName);
	return mysqldb->execMysql(mysqldb,sql,NULL);
}
//创建表
int createTable(struct mysqlStruct* mysqldb, char* name){
	char sql[200];
	snprintf(sql, 200, "CREATE TABLE %s (id INT PRIMARY KEY AUTO_INCREMENT, value TEXT, timestamp DATETIME DEFAULT CURRENT_TIMESTAMP);", name);
	return mysqldb->execMysql(mysqldb,sql,NULL);	
}
//删除表
int deleteTable(struct mysqlStruct* mysqldb, char* name){
	char sql[100];
	snprintf(sql, 100, "DROP TABLE %s;", name);
	return mysqldb->execMysql(mysqldb,sql,NULL);	
}

//删除表内所有内容，并充值自增字段
int delete_all_data(struct mysqlStruct* mysqldb, char* name){
	char sql[100];
	//删除表的所有数据
	snprintf(sql, 100, "DELETE FROM %s;", name);
	if(mysqldb->execMysql(mysqldb,sql,NULL)){
		return -1;
	}
	//重置自增字段
	snprintf(sql, 100, "ALTER TABLE %s AUTO_INCREMENT = 1;", name);
	if(mysqldb->execMysql(mysqldb,sql,NULL)){
		return -2;
	}
	
	return 0;
}


//返回最近时间点的一条数据
//data[0]=value1
//data[1]=value2
//data[2]=value3
int select_last_data(struct mysqlStruct* mysqldb,char **resArray,char *tableName){
	char sql[100];
	struct mysql_res_struct data = {NULL,0};
	snprintf(sql,100,"SELECT value, timestamp FROM %s ORDER BY timestamp DESC LIMIT 1;",tableName);

	if(mysqldb->execMysql(mysqldb,sql,&data))
	{
		return -1;	
	}
	
	int resArraySize = mysql_res_to_valArry(&data, &resArray, 0);
	mysqldb->freeRes(&data);
	return resArraySize;
}

//返回符合条件的表个数
int isIn_tableNames(struct mysqlStruct* mysqldb, char *database, char *tableName){
	char sql[200];
	struct mysql_res_struct data = {NULL,0};
	snprintf(sql,200,
	"SELECT COUNT(*) "
	"FROM information_schema.tables "
	"WHERE table_schema = '%s' "
  	"AND table_name = '%s';"
	,database, tableName);

	if(mysqldb->execMysql(mysqldb,sql,&data)){
		return -1;	
	}
	int res = data.row_count;
	mysqldb->freeRes(&data);
	return res;
}



/*
 * ALTER TABLE old_table_name RENAME TO new_table_name;
 * 
 * SELECT * FROM data_table
	ORDER BY timestamp ASC;
	ASC 表示按升序（从旧到新）排序。
	DESC 表示按降序（从新到旧）排序，如果你想要按最新的时间排序，可以使用 DESC。
 * 
 * 
 * */







