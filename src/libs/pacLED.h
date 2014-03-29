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

#define PACLED_VERSION 1
#define PACLED_PRODUCT_STR "1401"

typedef struct json_object json_object;

struct pacLED
{
  bool ledMapState;
  bool ledMapIntensity;
  bool allStates;
  bool allIntensities;
  bool random;
};

/*
 * Determine if the json file is an pacLED configuration
 */
bool isPacLED (json_object* jobj);

void convertIntensityMap (json_object* jobj);

bool updateBoardPacLED (json_object* jobj);

#ifdef __cplusplus
}
#endif

#endif /* PACLED_H_ */
