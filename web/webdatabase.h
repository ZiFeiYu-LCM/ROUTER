#ifndef _WEBDATABASE_H
#define _WEBDATABASE_H

#include "../plc/plcdatabase.h"
#include "../plc/plcctl.h"
#include "../tojson.h"
char *web_pull_plcinfo(char *jsonStr);
char* web_pull_plcinfo_num(char *jsonStr);
char *web_pull_mpinfo(char *jsonStr);
char *web_pull_mpinfo_num(char *jsonStr);
char *web_add_mpinfo(char *jsonStr);
char *web_update_mpinfo(char* jsonStr);







#endif

