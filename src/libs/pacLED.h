/*
 * pacLED.h
 *
 *  Created on: Mar 23, 2014
 *      Author: katie
 */

#ifndef PACLED_H_
#define PACLED_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct json_object json_object;

/*
 * Determine if the json file is an pacLED configuration
 */
bool isPacLED (json_object* jobj);

char convertPacLED (json_object* jobj);

bool updateBoardPacLED (json_object* jobj);

#ifdef __cplusplus
}
#endif

#endif /* PACLED_H_ */
