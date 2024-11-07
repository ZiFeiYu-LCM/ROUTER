#include "webdatabase.h"



char* web_pull_plcinfo(char *jsonStr){
	char *topic = "web/pull_dev_info";
	printf("%s\n",topic);
	paraData* data = NULL;
	int listSize = jsonToParam(jsonStr,&data);
	printf("%s   %s   %d    %d\n",data->dataArray,data->msg,data->code,data->dataArrySize);
	int index = 0;
	if(data->dataArray != NULL){
		index = atoi(data->dataArray);
	}else{
		return "error:dataArray is NULL";
	}

	printf("断点-1\n");
	printf("index = %d\n",index);
	char sql[100];
	snprintf(sql,100,"SELECT * FROM plcdevices LIMIT 10 OFFSET %d;",index);
	printf("%s\n",sql);
	struct mysql_res_struct resdata = {NULL,0};
	struct mysqlStruct* routerBase = plcDatabase_init();
	printf("断点-2\n");
	if(routerBase == NULL){
		return "error: routerBase init failed";
	}
	if(routerBase->connectDb(routerBase, DATABASE, IP)){
		fprintf(stderr,"connect to database %s failed!\n",DATABASE);
		return "error: connect to database failed";
	}
	if(routerBase->execMysql(routerBase,sql,&resdata)){
		return "error: execute sql failed";
	}
	for (size_t i = 0; i < resdata.row_count; i++)
	{
		printf("%s\n",resdata.rows[i]);
	}
	
	printf("这里row_count = %d\n",resdata.row_count);
	
	struct dev_info_brief *infoList = resdata.row_count>0?(struct dev_info_brief *)malloc(sizeof(struct dev_info_brief)*resdata.row_count):NULL;
	char *value = NULL;
	for(int i=0;i<resdata.row_count;i++){
		value = findValuebyKey(&resdata,"devid",i);
		infoList[i].devid = atoi(value);
		free(value);
		
		value = findValuebyKey(&resdata,"devname",i);
		strcpy(infoList[i].devname,value);
		free(value);
		
		value = findValuebyKey(&resdata,"ip",i);
		strcpy(infoList[i].ip,value);
		free(value);
		
		value = findValuebyKey(&resdata,"note",i);
		strcpy(infoList[i].note,value);
		free(value);
		
		value = findValuebyKey(&resdata,"protocol",i);
		strcpy(infoList[i].protocol,value);
		free(value);	
	}
	
	

	paraData *paradata = paramInit(topic,resdata.row_count,200);
	paradata->dataArray = infoList;
	printf("%s\n", ((struct dev_info_brief*)paradata->dataArray)[0].ip);
	char *resStr = paramToJson(paradata);
	paramFree(paradata);
	paramFree(data);
	routerBase->freeRes(&resdata);
	plcDatabase_free(routerBase, NULL);
	printf("%s\n",resStr);
	return resStr;
}

char* web_pull_plcinfo_num(char *jsonStr){
	char *topic = "test/web/pull_dev_info_allnum";
	printf("%s\n",topic);
	char sql[100];
	snprintf(sql,100,"SELECT COUNT(*) AS total_count FROM plcdevices;");
	struct mysql_res_struct resdata = {NULL,0};
	struct mysqlStruct* routerBase = plcDatabase_init();
	if(routerBase == NULL){
		return "error: routerBase init failed";
	}
	if(routerBase->connectDb(routerBase, DATABASE, IP)){
		fprintf(stderr,"connect to database %s failed!\n",DATABASE);
		return "error: connect to database failed";
	}
	if(routerBase->execMysql(routerBase,sql,&resdata)){
		return "error: execute sql failed";
	}

	char *value = findValuebyKey(&resdata,"total_count",0);
	paraData *paradata = paramInit(topic,0,0);
	paradata->dataArray = (char*)malloc(sizeof(char)*10);
	snprintf(paradata->dataArray,9,"%s",value);
	printf("total_count = %s\n",paradata->dataArray);
	char *resStr = paramToJson(paradata);

	free(value);
	routerBase->freeRes(&resdata);
	plcDatabase_free(routerBase, NULL);
	return resStr;
}

char *web_pull_mpinfo(char *jsonStr){
	char *topic = "web/pull_mp_info";
	printf("%s\n",topic);
	paraData* data = NULL;
	jsonToParam(jsonStr,&data);
	// printf("----------%s   %s   %d    %d   %d\n",data->dataArray,data->msg,data->code,data->dataArrySize,strlen(data->dataArray));
	int index=0,startIndex=0;
	if(strlen(data->dataArray) > 0){
		char *indexStr = (char*)malloc(sizeof(char)*(strlen(data->dataArray)+2));
		strcpy(indexStr,data->dataArray);
		
		index = atoi(strtok(indexStr,","));
		startIndex = atoi(strtok(NULL,","));
		free(indexStr);
	}

	char sql[600];
	snprintf(sql, sizeof(sql),
    "SELECT "
    "pd.devid, pd.devname, pd.ip, pd.pollcycle, pd.ismultipoll, pd.pollcycle2, pd.istimeout, pd.collecttime, pd.connecttime, "
    "dg.groupname, dg.uploadperiod, dg.maxitem, "
    "pmp.mpid, pmp.mpname, pmp.addresstype, pmp.dbindex, pmp.time, pmp.address, pmp.valuetype, pmp.mpnote, pmp.mode, pmp.addresstype, "
    "pw.warnname, pw.warnlevel, pw.wcvalue1, pw.wcvalue2, pw.warncontext "
    "FROM plcdevices pd "
    "INNER JOIN plcmp pmp ON pd.devid = pmp.devid "
    "LEFT JOIN devgroup dg ON pd.groupid = dg.groupid "
    "LEFT JOIN plcwarn pw ON pmp.mpid = pw.mpid "
    "WHERE pd.devid = %d "
	"LIMIT 10 OFFSET %d;", index, startIndex);
	printf("%s\nstrlen(sql) = %d\n",sql,strlen(sql));

	struct mysql_res_struct resdata = {NULL,0};
	struct mysqlStruct* routerBase = plcDatabase_init();
	if(routerBase == NULL){
		return "error: routerBase init failed";
	}
	if(routerBase->connectDb(routerBase, DATABASE, IP)){
		fprintf(stderr,"connect to database %s failed!\n",DATABASE);
		return "error: connect to database failed";
	}
	if(routerBase->execMysql(routerBase,sql,&resdata)){
		return "error: execute sql failed";
	}
	struct mp_info_brief *infoList =  resdata.row_count>0?(struct mp_info_brief *)malloc(sizeof(struct mp_info_brief)*resdata.row_count):NULL;

	char *value = NULL;
	for(int i=0;i<resdata.row_count;i++){
		value = findValuebyKey(&resdata,"devid",i);
		infoList[i].devid = atoi(value);free(value);
		value = findValuebyKey(&resdata,"mpid",i);
		infoList[i].mpid = atoi(value);free(value);
		value = findValuebyKey(&resdata,"groupid",i);
		infoList[i].groupid = atoi(value);free(value);
		value = findValuebyKey(&resdata,"valuetype",i);
		infoList[i].valuetype = atoi(value);free(value);	
		value = findValuebyKey(&resdata,"mpname",i);
		strcpy(infoList[i].mpname,value);free(value);
		value = findValuebyKey(&resdata,"groupname",i);
		strcpy(infoList[i].groupname,value);free(value);
		value = findValuebyKey(&resdata,"time",i);
		strcpy(infoList[i].time,value);free(value);
		value = findValuebyKey(&resdata,"address",i);
		strcpy(infoList[i].address,value);free(value);
		value = findValuebyKey(&resdata,"mpnote",i);
		strcpy(infoList[i].mpnote,value);free(value);	
			
		value = findValuebyKey(&resdata,"mode",i);
		infoList[i].mode = atoi(value);free(value);	
		value = findValuebyKey(&resdata,"addresstype",i);
		strcpy(infoList[i].addresstype,value);free(value);	
		value = findValuebyKey(&resdata,"dbindex",i);
		infoList[i].dbindex = atoi(value);free(value);	
	}
	
	
	paraData *paradata = paramInit(topic,resdata.row_count,0);
	paradata->dataArray = infoList;
	char *resStr = paramToJson(paradata);
	paramFree(paradata);
	paramFree(data);
	routerBase->freeRes(&resdata);
	plcDatabase_free(routerBase, NULL);
	return resStr;
}

char *web_pull_mpinfo_num(char *jsonStr){
	char *topic = "web/pull_mp_info_allnum";
	printf("%s\n",topic);

	paraData* data = NULL;
	jsonToParam(jsonStr,&data);
	int index = 0;
	if(strlen(data->dataArray) > 0){
		index = atoi(data->dataArray);
	}

	char sql[100];
	snprintf(sql,100,"SELECT COUNT(*) AS total_count FROM plcmp WHERE devid = %d;",index);
	struct mysql_res_struct resdata = {NULL,0};
	struct mysqlStruct* routerBase = plcDatabase_init();
	if(routerBase == NULL){
		return "error: routerBase init failed";
	}
	if(routerBase->connectDb(routerBase, DATABASE, IP)){
		fprintf(stderr,"connect to database %s failed!\n",DATABASE);
		return "error: connect to database failed";
	}
	if(routerBase->execMysql(routerBase,sql,&resdata)){
		return "error: execute sql failed";
	}

	char *value = findValuebyKey(&resdata,"total_count",0);
	paraData *paradata = paramInit(topic,0,0);
	paradata->dataArray = (char*)malloc(sizeof(char)*10);
	snprintf(paradata->dataArray,9,"%s",value);
	printf("total_count = %s\n",paradata->dataArray);
	char *resStr = paramToJson(paradata);

	free(value);
	routerBase->freeRes(&resdata);
	plcDatabase_free(routerBase, NULL);
	return resStr;

}

char *web_add_mpinfo(char *jsonStr){
	char *topic = "web/add_mp_info";
	paraData* data = NULL;
	jsonToParam(jsonStr,&data);
	if(data->dataArrySize <= 0){
		return "error:mpinfo is NULL";
	}

	struct mp_info_brief *mpinfoStructure = (struct mp_info_brief *)data->dataArray;
	char sql[200];
	snprintf(sql, sizeof(sql),
    "INSERT INTO plcmp (mpname, addresstype, dbindex, address, valuetype, mode, mpnote, devid) "
    "VALUES ('%s', '%s', %d, '%s', %d, %d, '%s', %d);", mpinfoStructure->mpname, mpinfoStructure->addresstype,mpinfoStructure->dbindex,mpinfoStructure->address,mpinfoStructure->valuetype,mpinfoStructure->mode,mpinfoStructure->mpnote,mpinfoStructure->devid);
	printf("%s\nstrlen(sql) = %d\n",sql,strlen(sql));
	struct mysqlStruct* routerBase = plcDatabase_init();
	if(routerBase == NULL){
		return "error: routerBase init failed";
	}
	if(routerBase->connectDb(routerBase, DATABASE, IP)){
		fprintf(stderr,"connect to database %s failed!\n",DATABASE);
		return "error: connect to database failed";
	}
	if(routerBase->execMysql(routerBase,sql,NULL)){
		return "error: execute sql failed";
	}
	plcDatabase_free(routerBase, NULL);
	paramFree(data);
	return NULL;
}

char *web_update_mpinfo(char* jsonStr){
	char *topic = "web/update_mp_info";
	printf("%s\n",topic);
	if(jsonStr == NULL){
		return NULL;
	}
	paraData* data = NULL;
	jsonToParam(jsonStr,&data);
	if(data->dataArrySize <=0){
		return NULL;
	}
	struct mp_info_brief* mpinfoStructure = data->dataArray;
	char sql[300];
	snprintf(sql,sizeof(sql),
		"UPDATE plcmp "
		"SET mpname = '%s', "
		"addresstype = '%s', "
		"address = '%s', "
		"valuetype = %d, "
		"mode = %d, "
		"mpnote = '%s' "
		"WHERE mpid = %d AND devid = %d;",
		mpinfoStructure->mpname,mpinfoStructure->addresstype,mpinfoStructure->address,mpinfoStructure->valuetype,mpinfoStructure->mode,mpinfoStructure->mpnote,
		mpinfoStructure->mpid, mpinfoStructure->devid);
	printf("%s\n",sql);
	struct mysqlStruct* routerBase = plcDatabase_init();
	if(routerBase == NULL){
		return "error: routerBase init failed";
	}
	if(routerBase->connectDb(routerBase, DATABASE, IP)){
		fprintf(stderr,"connect to database %s failed!\n",DATABASE);
		return "error: connect to database failed";
	}
	if(routerBase->execMysql(routerBase,sql,NULL)){
		return "error: execute sql failed";
	}
	plcDatabase_free(routerBase, NULL);
	paramFree(data);
	return NULL;
}

















