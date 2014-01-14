
#include <json/json.h>
#include <stdio.h>

#include "ultimarc.h"

void foo(void)
{
	puts("Ulimarc shared library test");
}

int validate_json (char *json)
{

	json_object *jobj = json_tokener_parse(json);
	//json_parse(jobj);

	enum json_type type;
	 json_object_object_foreach(jobj, key, val) {
	 type = json_object_get_type(val);
	 switch (type) {
	 case json_type_boolean: printf("type: json_type_boolean, ");
	 printf("value: %sn", json_object_get_boolean(val)? "true": "false");
	 break;
	 }
	 }

	return 1;
}
